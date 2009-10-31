<?php
// $ID$
class twitter_views_handler_filter_hash extends twitter_views_handler_filter {
    function query() {
    // Replace @ with '' and add to the query.
    $this->value = str_replace('#', '', $this->value);
    $this->query->add_where($this->options['group'], "#$this->value");
  }
}
