<?php

/**
 * @file
 * Main include file for better_node_admin_content defining functions 
 * required for admin/content/node form.
 */

/**
 * Form definition for content admin screen.  Allows you to filter and sort
 * nodes by various criteria and then apply operations to selected nodes.
 */
function better_node_admin_content(&$form_state) {  
  //start with basic form array
  $form = array();
  
  // if form is being rebuilt for confirmation then display confirmation text
  if (isset($form_state['storage']['confirmation'])) {
    return confirm_form($form, $form_state['storage']['confirmation'], 'admin/content/node');
  }

  else{
    //build form elements for selecting filters
    $filters = module_invoke_all('node_admin_filters'); //better_node_admin_content_form_filters();
    $form['filters'] = array(
      '#type' => 'fieldset',
      '#title' => t('Content Filter(s)'),
      '#description' => t('Show only items where all of the following conditions are true.'),
      '#attributes' => array('id' => 'filters'),
    );
    foreach ($filters as $id => $filter) {
      $form['filters'][$id] = array(
        '#type' => 'fieldset',
        '#title' => t($filter['title']),
        '#weight' => isset($filter['weight']) ? $filter['weight'] : 0,
        $id .'_is' => array(
          '#type' => 'select',
          '#options' => array('1' => 'is', '0' => 'is not'),
        ),
        $id .'_filter' => array(
          '#type' => 'select',
          '#options' => array('' => '')+$filter['options'],
          '#default_value' => "",
        ),
      );
    }
    $form['filters']['filter'] = array(
      '#type' => 'submit',
      '#value' => 'Apply filters',
      '#weight' => 100,
    );
  
    //set default values is filter has been run previously
    if (isset($_SESSION['node-admin-filters'])) {
      foreach ($_SESSION['node-admin-filters'] as $filter => $value) {
        $form['filters'][$filter][$filter .'_filter']['#default_value'] = $value['filter'];
        $form['filters'][$filter][$filter .'_is']['#default_value'] = $value['is'];
      }
    }
  }
  
  //add node table
  $options = array();
  $columns = module_invoke_all('node_admin_columns');
  uasort($columns, 'element_sort');
  $result = pager_query(db_rewrite_sql(better_node_admin_content_query($form_state)), 50, 0, NULL);
  while ($row = db_fetch_object($result)) {    
    $options[$row->nid] = array(l(theme('image',  drupal_get_path('module', 'better_node_admin_content') .'/icon-edit.gif', 'edit',  t('Click to edit this node')),  'node/'. $row->nid .'/edit',  array('html' => TRUE)));        
    foreach ($columns as $column) {
      $field = $column['field'];
      if (preg_match('/\.(.*?)$/', $field, $match)) {
        $field = $match[1];
      }
      //convert to array to lookup by value of $field
      $array = get_object_vars($row); 
      $field_value = $array[$field];
      
      if (isset($column['#theme'])) {
        $options[$row->nid][] = theme($column['#theme'], $field_value, $row);
      }
      else{
        $options[$row->nid][] = $field_value;
      }
    }
  }
    
  $form['nodes'] = array(
    '#type' => 'checkboxes', 
    '#options' => $options,
    '#theme' => 'better_node_admin_nodes',
  );
  
  //add pager for node table
  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));
    
  //node operations
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Actions'),
    '#description' => t('Select an action below to perform on the checked nodes above.'),
    '#attributes' => array('id' => 'operations'),
  );

  $options = array();
  foreach (better_node_operations() as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#options' => $options,
  );

  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update selected nodes'),
  );
  
  return $form;
}

/**
 * Form submit callback for content admin screen.  Behavior changes based
 * upon the button used to reach this function:
 *  'Apply filters': 
 *    Stores the selected filters into the users session and redisplays form
 *    with the new filters.
 *  'Update selected nodes': 
 *    If a confirmation is set for the selected operation then a confirmation 
 *    form is displayed, otherwise the operation callback function is called
 *    with the selected nodes.
 *  'Confirm':
 *    For operations which require a confirmation the operation callback 
 *    function is called with the selected nodes.
 */
function better_node_admin_content_submit($form, &$form_state) {
  switch ($form_state['clicked_button']['#value']) {
    // If submit is from the 'Appy filters' button adjust filter
    // values stored in the session object
    case 'Apply filters':
      $_SESSION['node-admin-filters'] = array();
      foreach (module_invoke_all('node_admin_filters') as $filter_id => $filter) {
        if ($form_state['values'][$filter_id .'_filter']) {
          $_SESSION['node-admin-filters'][$filter_id] = array(
            'filter' => $form_state['values'][$filter_id .'_filter'],
            'is' => $form_state['values'][$filter_id .'_is'],
          );
        }
      }
      break;
      
    // If submit is from the 'Update selected nodes' button then call the
    // appropriate callback function or confirmation form
    case 'Update selected nodes':
      //lookup selected operation
      $operations = better_node_operations();
      $operation = $operations[$form_state['values']['operation']];
      //lookup selected nodes
      $nodes = array_filter($form_state['values']['nodes']);
      // if confirmation is required store form contents and rebuild form
      // with confirmation message
      if ( isset($operation['confirmation']) ) {
        $form_state['storage']['confirmation'] = $operation['confirmation'];
        $form_state['storage']['nodes'] = $nodes;
        $form_state['storage']['callback'] = $operation['callback'];
        if (isset($operation['callback arguments'])) {
          $form_state['storage']['callback arguments'] = array_merge(array($nodes), $operation['callback arguments']);
        }
        $form_state['rebuild'] = TRUE;
        return;
      }
      //perform callback operation on the selected nodes and clear cache
      if ($function = $operation['callback']) {
        // Add in callback arguments if present.
        if (isset($operation['callback arguments'])) {
          $args = array_merge(array($nodes), $operation['callback arguments']);
        }
        else {
          $args = array($nodes);
        }
        call_user_func_array($function, $args);
        cache_clear_all();
      }
      break;
    
    // If the submit is from the confirmation form then apply the appropriate
    // callback functions
    case 'Confirm':
      //perform callback operation on the selected nodes and clear cache
      if ($function = $form_state['storage']['callback']) {
        // Add in callback arguments if present.
        if (isset($form_state['storage']['callback arguments'])) {
          $args = array_merge(array($form_state['storage']['nodes']), $form_state['storage']['callback arguments']);
        }
        else {
          $args = array($form_state['storage']['nodes']);
        }
        call_user_func_array($function, $args);
        cache_clear_all();
      }
    //clear out form storage values to redisplay of full form
    $form_state['storage'] = array();
  } 
}

/**
 * Builds a SQL query to select nodes for display in the node table as part 
 * of the admin/content/node form.  This query is dynamically built out based
 * upon:
 *  Selected nodes: If a nodes are on a form submission this will limit the
 *  query to these nodes ignoring any other filters.  This would allow the 
 *  display of a node table on confirmation pages although it is not 
 *  currently being used.
 *
 *  Filters: If filters are saved in the $_SESSION object then they are used
 *  to generate sql WHERE and JOIN clauses in order to limit the results to
 *  that table.
 *
 *  Columns: Modules which implement hook_node_admin_columns can provide 
 *  additional sql parts here to select the columns they are adding to the
 *  table.
 */
function better_node_admin_content_query($form_state) { 
  //default values: no joins, no wheres
  $filters = module_invoke_all('node_admin_filters');
  $additional_where = array();
  $additional_join = array();
  $additional_select = array();
  
  //if form is being rebuilt show the nodes from before
  if (isset($form_state['storage']['nodes'])) {
    $additional_where[] = '(n.nid='. implode(' OR n.nid=', $form_state['storage']['nodes']) .')';
  }
  
  //apply custom filters if applied in session object
  else if (isset($_SESSION['node-admin-filters'])) {
    foreach ($_SESSION['node-admin-filters'] as $filter => $value) {
      $additional_sql_args = array($value['filter'], (bool)$value['is']);
      if (isset($filters[$filter]['callback arguments'])) {
        $additional_sql_args = array_merge($additional_sql_args, $filters[$filter]['callback arguments']);
      }
      $additional_sql = call_user_func_array($filters[$filter]['callback'], $additional_sql_args);

      if (isset($additional_sql['WHERE'])) {
        foreach ($additional_sql['WHERE'] as $sql) {
          $additional_where[] = $sql;
        }
      }

      if (isset($additional_sql['JOIN'])) {
        foreach ($additional_sql['JOIN'] as $sql) {
          $additional_join[] = $sql;
        }
      }
    }
  }
  
  foreach (module_invoke_all('node_admin_columns') as $column) {
    $additional_select[] = $column['field'];
    if (isset($column['#select'])) {
      $additional_select[] = $column['#select'];
    }
  }
  
  $where = '';
  if ($additional_where) {
      $where = 'WHERE '. implode(' AND ', $additional_where);
  }
  
  $join = '';
  if ($additional_join) {
      $join = implode(' ', $additional_join);
  }
  
  $select = 'n.nid';
  if ($additional_select) {
      $select .= ', '. implode(', ', $additional_select);
  }
    
  return 'SELECT '. $select .' FROM {node} n INNER JOIN {users} u ON n.uid = u.uid '. $join .' '. $where . tablesort_sql(better_node_admin_content_headers());
}

/**
 * Theme function for $form['nodes']
 *  @return HTML markup for table representation of nodes (with checkboxes)
 */
function theme_better_node_admin_nodes($element) {
  $rows = array();
  $headers = better_node_admin_content_headers();
  
  //add a row for each node
  foreach ($element['#options'] as $nid => $row) {
    $checkbox = array(
        '#type' => 'checkbox',
        '#value' => 0,
        '#id' => 'edit-nodes-'. $nid,
        '#name' => "nodes[$nid]",
        '#parents' => array('better_node_admin_content'),
    );
    array_unshift($row, drupal_render($checkbox));
    $rows[] =  $row;
  }
  
  //if there are no rows then dislplay message
  if (count($rows)==0) {
    $rows[] = array(
      array(
        'data' => t('There are no nodes which match the filter criteria you have selected '),
        'colspan' => count($headers),
      ),
    );
    
  }
  return theme('table', $headers, $rows);
}

/**
 * Helper function for theme_better_node_admin_nodes taking advantage of new
 *  hook_node_admin_columns to prepare headers for the node table.
 *  @return Array with appropriate format for $header variable in theme_table()
 */
function better_node_admin_content_headers() {
  //get all column information
  $columns = module_invoke_all('node_admin_columns');
  
  //sort headers by weight
  uasort($columns, 'element_sort');
  
  //remove extra column info that isn't used by tablesort_sql, currently this
  //is indicated by keys that start with #
  $size = count($columns);
  for ($c=0; $c<$size; $c++) {
    foreach (array_keys($columns[$c]) as $key) {
      if (preg_match('/^#/', $key)) {
        unset($columns[$c][$key]);
      }
    }
  }
  
  //append extra headers
  return array_merge(array('', ''), $columns);
}

//include original include file
include_once('modules/node/node.admin.inc');

//include hook implementations on behalf of node module
include_once(drupal_get_path('module', 'better_node_admin_content') .'/node.inc');

//include hook implementations on behalf of taxonomy module
if (module_exists('taxonomy')) {
  include_once(drupal_get_path('module', 'better_node_admin_content') .'/taxonomy.inc');
}

//include hook implementations on behalf of locale module
if (module_exists('locale')) {
  include_once(drupal_get_path('module', 'better_node_admin_content') .'/locale.inc');
}

//include hook implementations on behalf of translation module
if (module_exists('translation')) {
  include_once(drupal_get_path('module', 'better_node_admin_content') .'/translation.inc');
}
