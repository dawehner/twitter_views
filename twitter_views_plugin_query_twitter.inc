<?php
/**
 * TODO:
 * Support atom and json and xml etc.
 */

// $Id$
class twitter_views_plugin_query_twitter extends views_plugin_query {

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table, $base_field) {
    module_load_include('inc', 'twitter');

    $this->api_url = variable_get('twitter_views_api_url', 'http://search.twitter.com/search.json');
  }

  /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
    $view->build_info['query'] = $this->query($view);
    $view->build_info['count_query'] = $this->query($view, TRUE);
    $view->build_info['query_args'] = $this->get_where_args();
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   * 
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  function execute(&$view) {
    $query = $view->build_info['query'];

    if ($query) {
      $replacements = module_invoke_all('views_query_substitutions', $view);
      $query = str_replace(array_keys($replacements), $replacements, $query);


      $url = $this->api_url .'?'. $query;
      dsm($url);

      $start = views_microtime();

      $results = drupal_http_request($url, array(), 'GET');

      if (_twitter_request_failure($results)) {
        return FALSE;
      }

      $results = json_decode($results->data, TRUE);

      if ($results['results']) {
        $view->result = $results['results'];

        // $this->results are arrays.
        foreach ($view->result as $key => $value) {
          $view->result[$key] = (object) $value;
        }

        // Save the metadata into the object
        unset($results['results']);
        foreach ($results as $key => $value) {
          $this->$key = $value;
        }

        $this->execute_time = $this->completed_in;

        // FIXME
        $this->total_rows = $this->results_per_page * 2;
        $this->pager['current_page'] = $this->page -1;
      }
    }

    $view->execute_time = views_microtime() - $start;
  }


  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   */
  function query($view, $get_count = FALSE) {

    $where = $this->condition_query();

    $params = array();
    if (!empty($view->pager['items_per_page'])) {
      $params['rpp'] = $view->pager['items_per_page'] <= 100 ? $view->pager['items_per_page'] : 100;
    }
    $params['q'] = $where;

    // Build the rest of the code.
    $items = array();
    if ($params) {
      foreach ($params as $key => $value) {
        if ($key == 'q') {
          array_unshift($items, 'q='. urlencode($value));
        }
        else {
          $items[] = urlencode($key .'='. $value);
        }
      }
    }
    $params = implode('&', $items);

    return $params;
  }

  /**
   * Construct the "WHERE" or "HAVING" part of the query.
   *
   * @param $where
   *   'where' or 'having'.
   */
  function condition_query($where = 'where') {
    $clauses = array();
    foreach ($this->$where as $group => $info) {
      $clause = implode($info['type'], $info['clauses']);
      if (count($info['clauses']) > 1) {
        $clause = $clause;
      }
      $clauses[] = $clause;
    }

    if ($clauses) {
//       $keyword = drupal_strtoupper($where);
      if (count($clauses) > 1) {
        return implode(")" . $this->group_operator , $clauses);
      }
      else {
        return array_shift($clauses);
      }
    }
    return "";
  }

  /**
   * Get the arguments attached to the WHERE and HAVING clauses of this query.
   */
  function get_where_args() {
    $args = array();
    foreach ($this->where as $group => $where) {
      $args = array_merge($args, $where['args']);
    }
 
    if ($this->having) {
      foreach ($this->having as $group => $having) {
        $args = array_merge($args, $having['args']);
      }
    }
    return $args;
  }

  /**
   * Create a new grouping for the WHERE or HAVING clause.
   *
   * @param $type
   *   Either 'AND' or 'OR'. All items within this group will be added
   *   to the WHERE clause with this logical operator.
   * @param $group
   *   An ID to use for this group. If unspecified, an ID will be generated.
   * @param $where
   *   'where' or 'having'.
   *
   * @return $group
   *   The group ID generated.
   */
  function set_where_group($type = 'AND', $group = NULL, $where = 'where') {
    // Set an alias.
    $groups = &$this->$where;

    if (!isset($group)) {
      $group = empty($groups) ? 1 : max(array_keys($groups)) + 1;
    }

    if ($type === 'AND') {
      $type = ' ';
    }

    // Create an empty group
    if (empty($groups[$group])) {
      $groups[$group] = array('clauses' => array(), 'args' => array());
    }

    $groups[$group]['type'] = strtoupper($type);
    return $group;
  }

  function add_where($group, $clause) {
      $args = func_get_args();
      array_shift($args); // ditch $group
      array_shift($args); // ditch $clause

  // Expand an array of args if it came in.
    if (count($args) == 1 && is_array(reset($args))) {
      $args = current($args);
    }

    // Ensure all variants of 0 are actually 0. Thus '', 0 and NULL are all
    // the default group.
    if (empty($group)) {
      $group = 0;
    }

    // Check for a group.
    if (!isset($this->where[$group])) {
      $this->set_where_group('AND', $group);
    }

    // Add the clause and the args.
    if (is_array($args)) {
      $this->where[$group]['clauses'][] = $clause;
      // we use array_values() here to prevent array_merge errors as keys from multiple
      // sources occasionally collide.
      $this->where[$group]['args'] = array_merge($this->where[$group]['args'], array_values($args));
    }
  }

  /**
   * Let modules modify the query just prior to finalizing it.
   */
  function alter(&$view) {
    foreach (module_implements('twitter_views_query_alter') as $module) {
      $function = $module . '_twitter_views_query_alter';
      $function($view, $this);
    }
  }

  /**
   * Add a signature to the query, if such a thing is feasible.
   *
   * This signature is something that can be used when perusing query logs to
   * discern where particular queries might be coming from.
   */
  function add_signature(&$view) {
    //$view->query->add_field(NULL, "'" . $view->name . ':' . $view->current_display . "'", 'view_name');
  }
}
