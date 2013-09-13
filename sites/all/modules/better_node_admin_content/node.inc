<?php

/**
 * @file
 * Provides new hook implementations defined in this module on behalf of 
 * node module
 */

/**
 * Implementation of hook_node_admin_filters() on behalf of node module
 */
function node_node_admin_filters() {
  return array(
    'status' => array(
      'title' => t('Publishing Status'),
      'callback' => 'node_filter_status',
      'weight' => -10,
      'options' => array(
        'status' => t('Published'),
        'promote' => t('Promoted to front page'),
        'sticky' => t('Sticky'),
      ),
    ),
    'type' => array(
      'title' => t('Content Type'),
      'weight' => -9,
      'options' => node_get_types('names'),
      'callback' => 'node_filter_type',
    ),
  );  
}

/**
 * Callback function for node status filter
 *  @param $column
 *    The column in the node field to filter by should be one of the following:
 *      + status: published or unpublished
 *			+ sticky: sticky to top of lists or not
 *      + promote: promoted to front page or not
 *  @param $is
 *		Boolean value to determine how to filter, checks values in node column:
 *      TRUE: 1
 *      FALSE: 0
 *
 *  @return
 *    Array containing SQL query parts to add to filter query
 */
function node_filter_status($column, $is) {
  return array(
    'WHERE' => array('n.'. $column .'='. ($is ? 1:0)),
  );
}

/**
 * Callback function for node type filter
 *  @param $type
 *    The node type to filter by
 *  @param $is
 *		Boolean value to determine how to filter:
 *      TRUE: Show nodes that are of type $type
 *      FALSE: Show nodes which are not of type $type
 *
 *  @return
 *    Array containing SQL query parts to add to filter query
 */
function node_filter_type($type, $is) {
  return array(
    'WHERE' => array('n.type'. ($is ? '=':'<>') ." '". $type ."'"),
  );
}

/**
 * Implementation of hook_node_admin_columns.
 *
 *  @return
 *    Array containing columns from node module to be used on the table on 
 *    the node admin form.
 */
function node_node_admin_columns() {
  return array(
    array(
      'data' => 'Title',
      'sort' => 'asc',
      'field' => 'n.title',
      '#theme' => 'node_admin_column_title',
      '#weight' => -7,
    ),
    array(
      'data' => 'Type',
      'sort' => 'asc',
      'field' => 'n.type',
      '#theme' => 'node_admin_column_type',
      '#weight' => -5,
    ),
    array(
      'data' => 'Author',
      'sort' => 'asc',
      'field' => 'u.name',
      '#join' => 'INNER JOIN {users} u ON n.uid = u.uid',
      '#select' => 'u.uid',
      '#theme' => 'node_admin_column_author',
      '#weight' => -3,
    ),
    array(
      'data' => 'Status',
      'sort' => 'desc',
      'field' => 'n.status',
      '#theme' => 'node_admin_column_status',
      '#weight' => -1,
    ),
    array(
      'data' => 'Created',
      'sort' => 'asc',
      'field' => 'n.created',
      '#theme' => 'node_admin_column_date',
      '#weight' => 20,
    ),
    array(
      'data' => 'Modified',
      'sort' => 'asc',
      'field' => 'n.changed',
      '#theme' => 'node_admin_column_date',
      '#weight' => 20,
    ),
  );
}

/**
 * Theme function for node title column
 */
function theme_node_admin_column_title($title, $row) {
  return l($title, 'node/'. $row->nid);
}

/**
 * Theme function for node type column
 */
function theme_node_admin_column_type($type) {
  return node_get_types('name', $type);
}

/**
 * Theme function for node author column
 */
function theme_node_admin_column_author($name, $row) {
  return l($name, 'user/'. $row->uid);
}

/**
 * Theme function for node status column
 */
function theme_node_admin_column_status($status) {
  return $status ? t('Published') : t('Unpublished');
}

/**
 * Theme function for dates in column
 */
function theme_node_admin_column_date($date) {
  static $now, $today, $yesterday;
  if (!$now) {
    $now = time();
  }
  $time_diff = $now-$date;
  
  //if difference is less then an hour display in minutes
  if ($time_diff<3600) {
    return round(($time_diff)/60) .' '. t('minutes ago');
  }
  
  //if difference is les then 6 hours then display in hours
  else if ($time_diff<21600) {
    return round(($time_diff)/3600) .' '. t('hours ago');
  }
  
  else{
    if (!$today) {
      $today = format_date(time(), 'custom', 'n/d/y');
      $yesterday = format_date(strtotime('yesterday'), 'custom', 'n/d/y');      
    }
    $formatted_date = format_date($date, 'custom', 'n/d/y');
    
    //if date is today then show time created
    if ($formatted_date == $today) {
      return format_date($date, 'custom', 'g:i a');
    }
    
    //if date is yesterday then show word yesterday
    else if ($formatted_date == $yesterday) {
      return t('Yesterday');
    }
    
    //all other cases show the extra-short formatted date
    return $formatted_date;
  }
  
  switch ($date) {
    case $today:
      return t('Today');
    case $yesterday:
      return t('Yesterday');
    default:
      return $date;
  }
}

/**
 * hook_node_operation callback delete operation
 */
function node_mass_delete($nodes) {
  foreach ($nodes as $nid => $value) {
    node_delete($nid);
  }
  drupal_set_message(t('The items have been deleted.'));
}