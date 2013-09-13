<?php

/**
 * @file
 * Provides new hook implementations defined in this module on behalf of 
 * taxonomy module
 */

/**
 * Implementation of hook_node_admin_filters() on behalf of taxonomy module
 */
function taxonomy_node_admin_filters() {
  $filters = array();
  foreach (module_invoke('taxonomy', 'get_vocabularies') as $vocabulary) {
    //get options
    $options = array();
    foreach (module_invoke('taxonomy', 'get_tree', $vocabulary->vid) as $term) {
      $options[$term->tid] = str_repeat('-', $term->depth) . $term->name;
    }
    
    $filters['vocabulary_'. $vocabulary->vid] = array(
      'title' => $vocabulary->name,
      'callback' => 'taxonomy_node_filter',
      'callback arguments' => array($vocabulary->vid),
      'options' => $options,
    );
  }
  return $filters;  
}

/**
 * Callback function for taxonomy filters
 *  @param $tid
 *    The taxonomy term id to filter by
 *  @param $is
 *		Boolean value to determine to filter by nodes with the given taxonomy
 *    term (if true) or without (if false)
 *
 *  @return
 *    Array containing SQL query parts to add to filter query
 */
function taxonomy_node_filter($tid, $is, $vid) {
  if ($is) {
    static $tn_count = -1;
    ++$tn_count;
    return array(
      'JOIN' => array("INNER JOIN term_node tn$tn_count ON tn$tn_count.nid=n.nid AND tn$tn_count.tid=$tid"),
    );
  }
  else{
    return array(
      'WHERE' => array(
        "n.nid NOT IN(SELECT n.nid FROM {node} n INNER JOIN {term_node} tn ON tn.nid = n.nid WHERE tn.tid=$tid)"
      ),
    );
  }
}
