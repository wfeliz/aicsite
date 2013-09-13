<?php

/**
 * @file
 * Provides new hook implementations defined in this module on behalf of 
 * translation module
 */

/**
 * Implementation of hook_node_admin_filters() on behalf of translation module
 */
function translation_node_admin_filters() {
  return array(
    'translate' => array(
      'title' => t('Translation'),
      'callback' => 'translation_node_filter',
      'weight' => 7,
      'options' => array(
        'translate' => t('Up to date'),
      ),
    ),
  );
}

/**
 * Callback function for translation node filter
 *  @param $value
 *    This has no effect on translation filter as there is only one option
 *  @param $is
 *		Boolean value to determine how to filter:
 *      TRUE: translation is up to date
 *      FALSE: translation is out of date
 *
 *  @return
 *    Array containing SQL query parts to add to filter query
 */
function translation_node_filter($value, $is) {
  //note: n.translate=0 means up to date
  return array(
    'WHERE' => array('n.translate='. ($is ? 0:1)),
  );
}
