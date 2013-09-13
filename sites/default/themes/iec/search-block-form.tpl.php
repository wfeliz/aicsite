<?php
// $Id: search-block-form.tpl.php,v 1.1 2007/10/31 18:06:38 dries Exp $

/**
 * @file search-block-form.tpl.php
 * Default theme implementation for displaying a search form within a block region.
 *
 * Available variables:
 * - $search_form: The complete search form ready for print.
 * - $search: Array of keyed search elements. Can be used to print each form
 *   element separately.
 *
 * Default keys within $search:
 * - $search['search_block_form']: Text input area wrapped in a div.
 * - $search['submit']: Form submit button.
 * - $search['hidden']: Hidden form elements. Used to validate forms when submitted.
 *
 * Since $search is keyed, a direct print of the form element is possible.
 * Modules can add to the search form so it is recommended to check for their
 * existance before printing. The default keys will always exist.
 *
 *   <?php if (isset($search['extra_field'])): ?>
 *     <div class="extra-field">
 *       <?php print $search['extra_field']; ?>
 *     </div>
 *   <?php endif; ?>
 *
 * To check for all available data within $search, use the code below.
 *
 *   <?php print '<pre>'. check_plain(print_r($search, 1)) .'</pre>'; ?>
 *
 * @see template_preprocess_search_block_form()
 */
?>
<div class="container-inline">
	<table border="0" cellpadding="0" cellspacing="0" height="20"><tbody><tr>
		 <!--[if lte IE 7]><td valign="top">  <input type="image" class="search-submit" value="" id="edit-submit" name="op" src="/sites/default/themes/ipc/images/search_btn_ie.png" style="padding:1px 0 0 0;" /></td> <![endif]-->
		<td valign="top" class="iehide" height="20">  <input type="image" class="search-submit" value="" id="edit-submit" name="op" src="/sites/default/themes/ipc/images/search_btn.png"/></td>
		 <td valign="top" height="20"> <input type="text" name="search_block_form" id="edit-search-theme-form-l" size="25" value="" title="Enter your search terms" class="search-text" onclick="value='';" /></td>
		</tr></tbody></table>
		
  <?php
  print $search["hidden"];
   ?>
</div>