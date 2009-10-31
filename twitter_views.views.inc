<?php
// $Id$

/**
 * Twitter API views search.
 */

function twitter_views_views_data() {
  $data['twitter_api']['table']['group']  = t('Twitter_API');

  $data['twitter_api']['table']['base'] = array(
    'title' => t('Twitter_API'),
    'help' => t('blub'),
    'query class' => 'twitter_query'
  );
  
  // String
  $data['twitter_api']['string'] = array(
    'title' => t('String'),
    'help' => t('Search for a string on twitter'),
    'field' => array(
      'handler' => 'twitter_views_handler_field',
    ),
    'filter' => array(
      'handler' => 'twitter_views_handler_filter_string',
    ),
  );

  $data['twitter_api']['text'] = array(
    'title' => t('Text'),
    'help' => t('Display the text of a twitter message'),
    // Shouldn't there be twitter_views_handler_field_markup??
    'field' => array(
      'handler' => 'twitter_views_handler_field_markup',
    ),
  );
  
  $data['twitter_api']['source'] = array(
    'title' => t('Source'),
    'help' => t('Display the source of a tweet'),
    'field' => array(
      'handler' => 'twitter_views_handler_field_source',
    ),
  );

  $data['twitter_api']['from_user'] = array(
    'title' => t('Tweet Author'),
    'help' => t('Display the author of the tweet'),
    'field' => array(
      'handler' => 'twitter_views_handler_field_from_user',
     ),
     'filter' => array(
      'handler' => 'twitter_views_handler_filter_from_user',
     ),
  );

  $data['twitter_api']['to_user'] = array(
    'title' => t('Tweet Recipient'),
    'help' => t('Display the recipient of the tweet'),
    'field' => array(
      'handler' => 'twitter_views_handler_field_to_user',
     ),
     'filter' => array(
      'handler' => 'twitter_views_handler_filter_to_user',
     ),
  );


  $data['twitter_api']['reference'] = array(
    'title' => t('Tweet reference'),
    'help' => t('Display the  referencing person  of the tweet'),
    'field' => array(
      'handler' => 'twitter_views_handler_field_reference',
     ),
     'filter' => array(
      'handler' => 'twitter_views_handler_filter_reference',
     ),
  );


  $data['twitter_api']['hash'] = array(
    'title' => t('Tweet hash'),
    'field' => array(
      'handler' => 'twitter_views_handler_field_hash',
     ),
     'filter' => array(
      'handler' => 'twitter_views_handler_filter_hash',
     ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function twitter_views_views_handlers() {
  return array(
    'handlers' => array(
      // Fields
      'twitter_views_handler_field' => array(
        'parent' => 'views_handler_field'
      ),
      'twitter_views_handler_field_source' => array(
        'parent' => 'twitter_views_handler_field',
      ),

      'twitter_views_handler_field_markup' => array(
        'parent' => 'twitter_views_handler_field',
      ),      
      'twitter_views_handler_field_from_user' => array(
        'parent' => 'twitter_views_handler_field'
      ),
      
      // Filters
      'twitter_views_handler_filter' => array(
        'parent' => 'views_handler_filter'
      ),
      'twitter_views_handler_filter_string' => array(
        'parent' => 'twitter_views_handler_filter'
      ),
      'twitter_views_handler_filter_from_user' => array(
        'parent' => 'twitter_views_handler_filter'
      ),
      'twitter_views_handler_filter_hash' => array(
        'parent' => 'twitter_views_handler_filter'
      ),
    ),
  );
}

/**
 * Implementation of hook_views_plugins().
 */
function twitter_views_views_plugins() {
  return array(
    'query' => array(
      'twitter_query' => array(
        'title' => t('Twitter Query'),
        'help' => t('Query will be generated and run using Twitter API.'),
        'handler' => 'twitter_views_plugin_query_twitter'
      ),
    ),
  );
}
