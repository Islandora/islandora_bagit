<?php

/**
 * @file
 * 'Datastram copy' plugin for the Islandora BagIt Drupal module.
 *
 * Registers all the datastreams in an Islandora object so they are
 * copied into the 'data' directory.
 */

/**
 * Returns an array of source and destination file paths. Returns FALSE if no 
 * datastream files are present.
 */
function islandora_bagit_plugin_copy_ds_basic_init($islandora_object, $tmp_ds_directory) {
  $files_to_add = array();
  $ds_files = islandora_bagit_get_ds_files($tmp_ds_directory);

  // Add file source and dest to the $files_to_add array. $files_to_add['dest']
  // must be relative to the Bag's dest subdirectory.
  foreach ($ds_files as $ds_filename) {
    // Add each file in the directory to $files_to_add.
    $source_file_to_add = $tmp_ds_directory . DIRECTORY_SEPARATOR . $ds_filename;
    if (file_exists($source_file_to_add) && is_file($source_file_to_add)) {
      $files_to_add[] = array(
        'source' => $source_file_to_add,
        'dest' => $ds_filename,
      );
    }
  }

  if (count($files_to_add)) {
    return $files_to_add;
  }
  else {
    return FALSE;
  }
}

