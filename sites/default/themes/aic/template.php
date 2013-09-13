<?php
// $Id: template.php,v 1.17.2.1 2009/02/13 06:47:44 johnalbin Exp $

/**
 * @file
 * Contains theme override functions and preprocess functions for the theme.
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. You can add new regions for block content, modify
 *   or override Drupal's theme functions, intercept or make additional
 *   variables available to your theme, and create custom PHP logic. For more
 *   information, please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/theme-guide
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   The Drupal theme system uses special theme functions to generate HTML
 *   output automatically. Often we wish to customize this HTML output. To do
 *   this, we have to override the theme function. You have to first find the
 *   theme function that generates the output, and then "catch" it and modify it
 *   here. The easiest way to do it is to copy the original function in its
 *   entirety and paste it here, changing the prefix from theme_ to STARTERKIT_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: aic_breadcrumb()
 *
 *   where STARTERKIT is the name of your sub-theme. For example, the
 *   zen_classic theme would define a zen_classic_breadcrumb() function.
 *
 *   If you would like to override any of the theme functions used in Zen core,
 *   you should first look at how Zen core implements those functions:
 *     theme_breadcrumbs()      in zen/template.php
 *     theme_menu_item_link()   in zen/template.php
 *     theme_menu_local_tasks() in zen/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called template suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node-forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and template suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440
 *   and http://drupal.org/node/190815#template-suggestions
 */

// Auto-rebuild the theme registry during theme development.
if (theme_get_setting('zen_rebuild_registry')) {
  drupal_rebuild_theme_registry();
}
/**
 * Implements HOOK_theme().
 */
function aic_theme(&$existing, $type, $theme, $path) {
  if (!db_is_active()) {
    return array();
  }
  include_once './' . drupal_get_path('theme', 'aic') . '/template.theme-registry.inc';
  return _aic_theme($existing, $type, $theme, $path);
}
/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 * @return
 *   A string containing the breadcrumb output.
 */
function aic_breadcrumb($breadcrumb) {
  // Determine if we are to display the breadcrumb.
  $show_breadcrumb = theme_get_setting('zen_breadcrumb');
  if ($show_breadcrumb == 'yes' || $show_breadcrumb == 'admin' && arg(0) == 'admin') {

    // Optionally get rid of the homepage link.
    $show_breadcrumb_home = theme_get_setting('zen_breadcrumb_home');
    if (!$show_breadcrumb_home) {
      array_shift($breadcrumb);
    }

    // Return the breadcrumb with separators.
    if (!empty($breadcrumb)) {
      $breadcrumb_separator = theme_get_setting('zen_breadcrumb_separator');
      $trailing_separator = $title = '';
      if (theme_get_setting('zen_breadcrumb_title')) {
        $trailing_separator = $breadcrumb_separator;
        $title = menu_get_active_title();
      }
      elseif (theme_get_setting('zen_breadcrumb_trailing')) {
        $trailing_separator = $breadcrumb_separator;
      }
      return '<div class="breadcrumb">' . implode($breadcrumb_separator, $breadcrumb) . "$trailing_separator$title</div>";
    }
  }
  // Otherwise, return an empty string.
  return '';
}

/**
 * Implements theme_menu_item_link()
 */
function aic_menu_item_link($link) {
  if (empty($link['localized_options'])) {
    $link['localized_options'] = array();
  }

  // If an item is a LOCAL TASK, render it as a tab
  if ($link['type'] & MENU_IS_LOCAL_TASK) {
    $link['title'] = '<span class="tab">' . check_plain($link['title']) . '</span>';
    $link['localized_options']['html'] = TRUE;
  }

  return l($link['title'], $link['href'], $link['localized_options']);
}

/**
 * Duplicate of theme_menu_local_tasks() but adds clear-block to tabs.
 */
function aic_menu_local_tasks() {
  $output = '';

  if ($primary = menu_primary_local_tasks()) {
    $output .= '<ul class="tabs primary clear-block">' . $primary . '</ul>';
  }
  if ($secondary = menu_secondary_local_tasks()) {
    $output .= '<ul class="tabs secondary clear-block">' . $secondary . '</ul>';
  }

  return $output;
}


/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
function aic_preprocess_page(&$vars, $hook) {
  // Add conditional stylesheets.
  if (!module_exists('conditional_styles')) {
    $vars['styles'] .= $vars['conditional_styles'] = variable_get('conditional_styles_' . $GLOBALS['theme'], '');
  }

  // Classes for body element. Allows advanced theming based on context
  // (home page, node of certain type, etc.)
  $classes = split(' ', $vars['body_classes']);
  // Remove the mostly useless page-ARG0 class.
  if ($index = array_search(preg_replace('![^abcdefghijklmnopqrstuvwxyz0-9-_]+!s', '', 'page-'. drupal_strtolower(arg(0))), $classes)) {
    unset($classes[$index]);
  }
  if (!$vars['is_front']) {
    // Add unique class for each page.
    $path = drupal_get_path_alias($_GET['q']);
    $classes[] = aic_id_safe('page-' . $path);
    // Add unique class for each website section.
    list($section, ) = explode('/', $path, 2);
    if (arg(0) == 'node') {
      if (arg(1) == 'add') {
        $section = 'node-add';
      }
      elseif (is_numeric(arg(1)) && (arg(2) == 'edit' || arg(2) == 'delete')) {
        $section = 'node-' . arg(2);
      }
    }
    $classes[] = aic_id_safe('section-' . $section);
  }
  if (theme_get_setting('zen_wireframes')) {
    $classes[] = 'with-wireframes'; // Optionally add the wireframes style.
  }
  $vars['body_classes_array'] = $classes;
  $vars['body_classes'] = implode(' ', $classes); // Concatenate with spaces.
}

/**
 * Override or insert variables into the node templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
function aic_preprocess_node(&$vars, $hook) {
  // Special classes for nodes
  $classes = array('node');
  if ($vars['sticky']) {
    $classes[] = 'sticky';
  }
  if (!$vars['status']) {
    $classes[] = 'node-unpublished';
    $vars['unpublished'] = TRUE;
  }
  else {
    $vars['unpublished'] = FALSE;
  }
  if ($vars['uid'] && $vars['uid'] == $GLOBALS['user']->uid) {
    $classes[] = 'node-mine'; // Node is authored by current user.
  }
  if ($vars['teaser']) {
    $classes[] = 'node-teaser'; // Node is displayed as teaser.
  }
  // Class for node type: "node-type-page", "node-type-story", "node-type-my-custom-type", etc.
  $classes[] = aic_id_safe('node-type-' . $vars['type']);
  $vars['classes'] = implode(' ', $classes); // Concatenate with spaces
  
  
   if(drupal_is_front_page())
                {
                    $vars['template_files'][] = 'node-front';
                }else{
    $vars['template_files'][] = 'node-' . $vars['nid'];
	 }
    return $vars;
}

/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
function aic_preprocess_comment(&$vars, $hook) {
  include_once './' . drupal_get_path('theme', 'zen') . '/template.comment.inc';
  _zen_preprocess_comment($vars, $hook);
}

/**
 * Override or insert variables into the block templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
function aic_preprocess_block(&$vars, $hook) {
  $block = $vars['block'];

  // Special classes for blocks.
  $classes = array('block');
  $classes[] = 'block-' . $block->module;
  $classes[] = 'region-' . $vars['block_zebra'];
  $classes[] = $vars['zebra'];
  $classes[] = 'region-count-' . $vars['block_id'];
  $classes[] = 'count-' . $vars['id'];

  $vars['edit_links_array'] = array();
  $vars['edit_links'] = '';
  if (theme_get_setting('zen_block_editing') && user_access('administer blocks')) {
    include_once './' . drupal_get_path('theme', 'zen') . '/template.block-editing.inc';
    zen_preprocess_block_editing($vars, $hook);
    $classes[] = 'with-block-editing';
  }

  // Render block classes.
  $vars['classes'] = implode(' ', $classes);
}

/**
 * Converts a string to a suitable html ID attribute.
 *
 * http://www.w3.org/TR/html4/struct/global.html#h-7.5.2 specifies what makes a
 * valid ID attribute in HTML. This function:
 *
 * - Ensure an ID starts with an alpha character by optionally adding an 'id'.
 * - Replaces any character except alphanumeric characters with dashes.
 * - Converts entire string to lowercase.
 *
 * @param $string
 *   The string
 * @return
 *   The converted string
 */
function aic_id_safe($string) {
  // Replace with dashes anything that isn't A-Z, numbers, dashes, or underscores.
  $string = strtolower(preg_replace('/[^a-zA-Z0-9-]+/', '-', $string));
  // If the first character is not a-z, add 'id' in front.
  if (!ctype_lower($string{0})) { // Don't use ctype_alpha since its locale aware.
    $string = 'id' . $string;
  }
  return $string;
}