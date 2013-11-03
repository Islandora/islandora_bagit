<?php

/**
 * @file
 * Plugin for the Islandora BagIt Drupal module that adds files attached to nodes using
 * the core Drupal 7 File or Image modules.
 */

/**
 * Returns an array of file paths and names. Returns FALSE if no file are
 * present or if FileField is not enabled.
 */
function islandora_bagit_plugin_add_foo_init($islandora_object, $tmp_ds_directory) {
  $files_to_add = array();

  // Get the directory listing for $tmp_ds_directory.
  $ds_files = scandir($tmp_ds_directory);
  // Get rid of the . and .. entries.
  unset($ds_files[0]);
  unset($ds_files[1]);

  file_put_contents($tmp_ds_directory . '/foo.txt', implode("\n", $ds_files));

  $files_to_add[] =  array(
      'path' => $tmp_ds_directory . '/foo.txt',
      'name' => 'foo.txt',
  );

  if (count($files_to_add)) {
    return $files_to_add;
  }
  else {
    return FALSE;
  }
}

