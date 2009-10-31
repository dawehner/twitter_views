<?php
// $Id$

class twitter_views_handler_field_source extends twitter_views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return html_entity_decode($value);
  }
}
