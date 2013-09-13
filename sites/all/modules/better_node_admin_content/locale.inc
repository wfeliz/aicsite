<?php

/**
 * @file
 * Provides new hook implementations defined in this module on behalf of 
 * locale module
 */

/**
 * Implementation of hook_node_admin_filters() on behalf of locale module
 */
function locale_node_admin_filters() {
  //get language list for options
  return array(
    'language' => array(
      'title' => t('Language'),
      'callback' => 'locale_node_filter',
      'options' => module_invoke('locale', 'language_list'),
      'weight' => 7,
    ),
  );
}

/**
 * Callback function for language filter
 *  @param $lang
 *    The language code to filter by
 *  @param $is
 *		Boolean value to determine how to filter:
 *      TRUE: node is in the language $lang
 *      FALSE: node is not in the language $lang
 *
 *  @return
 *    Array containing SQL query parts to add to filter query
 */
function locale_node_filter($lang, $is) {
  return array(
    'WHERE' => array("n.language='$lang'"),
  );
}

/**
 * Implementation of hook_node_admin_columns.
 *
 *  @return
 *    Array containing columns from locale module to be used on the table on 
 *    the node admin form.  Currently just returns a single language column.
 */
function locale_node_admin_columns() {
  return array(
    array(
      'data' => 'Language',
      'sort' => 'asc',
      'field' => 'n.language',
      '#theme' => 'node_admin_column_language',
      '#weight' => 10,
    ),
  );
}

function theme_node_admin_column_language($code) {
  if (!$code) {
    return t('n/a');
  }
  $languages = module_invoke('locale', 'language_list');
  return $languages[$code];
}