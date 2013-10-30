README.txt for the BagIt module.
================================

Introduction
============

BagIt (https://wiki.ucop.edu/display/Curation/BagIt) is a specification for
packaging content and metadata about that content into a format that can be
shared between applications. This module provides a "BagIt" tab for nodes that
allows the packaging of the node and any files attached to the node into a Bag.

Installation
============

This module uses the Scholars' Lab BagItPHP library, found at
https://github.com/scholarslab/BagItPHP, which is integrated with the
BagIt module using the Libraries API. The library has two dependencies,
http://pear.php.net/package/Archive_Tar and
http://pear.php.net/package/XML_Serializer.

To install the BagIt module:

1) Install http://pear.php.net/package/Archive_Tar. This package is required by
   PEAR so if you have PEAR installed on your system, you won't need to install
   Archive_Tar separately.
2) Install http://pear.php.net/package/XML_Serializer.
3) Install the Libraries API contrib module (http://drupal.org/project/libraries).
4) Unzip this module into your site's modules directory as you would any other
   contrib module.
5) Install the BagItPHP library by entering your site's sites/all/libraries
   directory and issuing the following command:

   git clone git://github.com/scholarslab/BagItPHP.git

6) Enable the Libraries and BagIt modules like you would any other contrib
   modules.
7) Configure the BagIt module by going to admin/settings/bagit.
8) Configure user permissions like you would for any other contrib module.

Extending and customizing the BagIt module
=========================================

The files that are added to a node's Bag are managed by plugins. There are
three types of plugins:

1) file creation plugins
2) file copy plugins
3) file fetch plugins

If you need to create a specific representation of the node to add to the Bag,
or if you have other requirements not covered by the supplied plugins, you can
use the supplied plugins described above to base your own on. If you write
your own plugins, follow these guidelines:

a) Begin plugin filenames with 'bagit_plugin_create_', 'bagit_plugin_copy_',
   or 'bagit_plugin_fetch_', depending on what type of plugin you are writing.
   Plugins must end in '.inc'.
b) Every plugin has a function that is named the same as the filename
   (minus the '.inc'), ending in '_init().' All init functions take a $node
   object as the only parameter.
c) Plugins complete all file writing and copying tasks before they return
   the file's paths (or URLs, for fetch plugins) and names to the bagit module.
d) If you write a file copy plugin, you can add 'extra' information to the
   array passed back to the bagit.module that might be useful for implementors
   of hook_bagit_filter_files(). See the function definition for
   bagit_build_file_extra() in bagit.module, and instances of this function
   in the bundled file copy plugins, for details.
e) Plugins return FALSE if there is an error in copying or writing files,
   or if they do no need to write files based on properties of the node.
  

Modifying a Bag from your own modules
=====================================

This module provides two hooks that let other modules modify Bags.

The first is drupal_alter(), which allows other modules to use
hook_bagit_alter($bag, $node). Your module can modify the current Bag using any
of the methods provided by the BagItPHP library. Each implementation of this
hook must take $bag and $node as parameters; $node is provided so you can use
access properties of the node in your module easily. A typical implementation
looks like this:

/**
 * Implementation of hook_bagit_alter().
 *
 * @param $bag
 *   A BagIt object instantiated in the BagIt module.
 *
 * @param $node
 *  The current $node object.
 */
function mymodule_bagit_alter($bag, $node) {
  // Add some custom metadata to bag-info.txt.
  $bag->bagInfoData['Some-Arbitrary-Field'] = 'Foo bar baz';
  // Add a file that is not managed by a plugin.
  $bag->addFile('/path/to/file.txt', 'myfile.txt');
  // Update the Bag (this is required).
  $bag->update();
}

Note that implementations of hook_bagit_alter() must call $bag->update()
themselves, typically at the very end of the function.

The second is hook_bagit_filter_files(), which allows modules to control
which files copy plugins add to the Bag (hook_bagit_alter() doesn't let
you do this). Parameters are $plugin_name, $files, and $node; implementations
must return the modified $files array. As with hook_bagit_alter(), $node is
provided so you can use access properties of the node in your module.
An example implentation of this hook is:

/**
 * Implementation of hook_bagit_filter_files().
 *
 * @param $plugin_name
 *   The name of the copy plugin that is generating the $files list.
 *
 * @param $files
 *  An array containing the files manged by the copy plugin.
 *
 * @param $node
 *  The current node.
 *
 * @return
 *  The modified $files array.
 */
function mymodule_bagit_filter_files($plugin_name, $files, $node) {
  if ($plugin_name == 'bagit_plugin_copy_filefield') {
    foreach ($files as $index => $file_info) {
      // Check for files managed in a specific field.
      if ($file_info['extra']['field'] == 'field_foo') {
        unset($files[$index]);
      }
      // Check to see if a file is from a specific directory.
      if (preg_match('/unwantedfiles/', $file_info['path'])) {
        unset($files[$index]);
      }
    }
  }
  return $files;
}

Permissions and security
========================

This module is intended for users who have a fairly high level of permissions
on a Drupal site. Because the goal is to package up an entire node and all or
some of the files attached to that node, users who can create and download Bags
will be able given access to the content protected by field-level permissions,
and in the case of downloading serialized Bags, might bypass Drupal's node
access checks if they know the download path to the Bag file, whether Drupal is
configured to use public or private filesystem. Users who do not have 'create
BagIt bags' permission cannot download serialized Bags when Drupal is configured
to use a private file system.

Do not grant permission to create BagIt Bags to users who should not view nodes
on your site that you have configured the module to create Bags for.

Author/maintainer
=================
Mark Jordan <mjordan at sfu dot ca>
