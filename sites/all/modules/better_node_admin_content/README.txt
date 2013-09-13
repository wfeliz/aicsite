
Introduction
-----------------------------------------------------------------------------
The better_node_admin_content project aims to provide usability improvements
and extensibility through additional hooks to the node_admin_content form 
found at admin/content/node.  To facilitate this goal the project is 
currently focussed on creating a module for Drupal 6 to implement the form 
improvements.  Once this module has reached a stable state it will be the 
base for a Drupal 7 patch.  

Usability improvements
-----------------------------------------------------------------------------
- Provides additional node filters
- Each filter can be set to 'is' or 'is not'
- Add ability to sort by column headers on node table
- Recent times are displayed as minutes/hours ago
 
Extensibility improvements
-----------------------------------------------------------------------------
- NEW HOOK: hook_node_admin_filters - Allows modules to add additional  
  filters to the form.
- NEW HOOK: hook_node_admin_columns - Allows modules to add additional  
  columns to the node table.  Cells in the column are theme-able.
- UPDATED HOOK: node_operations - Operations can now define a 
  'confirmation' attribute which creates a confirmation form before the 
  operation is applied to the selected nodes.

Installation
-----------------------------------------------------------------------------
Copy better_node_admin_content.module to your module directory and enable it
on the admin modules page.

Author
------
Kevin Stock
kevin@toolhouse.com
