<?php

/**
 * @file
 * 'Copy datastream' plugin for the Islandora BagIt Drupal module.
 *
 * Creates Bags compatible with Archivematica (http://archivematica.org) 
 * transfer Bags by creating 'metadata' and 'objects' subdirectories, then
 * moving the DC datastream into the 'metadata' directory and all other
 * files into the 'objects' directory.
 */

/**
 * Returns an array of source and destination file paths. Returns FALSE if no 
 * datastream files are present.
 */
function islandora_bagit_plugin_copy_archivematica_transfers_init($islandora_object, $tmp_ds_directory) {
  $files_to_add = array();
  $ds_files = islandora_bagit_get_ds_files($tmp_ds_directory);

  // Create a subdirectory in $tmp_ds_directory.
  mkdir($tmp_ds_directory . '/metadata', 0777, TRUE);
  mkdir($tmp_ds_directory . '/objects', 0777, TRUE);
  // Move the DC and MODs datastreams into this subdirectory and add the
  // new files to $files_to_add.
  rename($tmp_ds_directory . '/DC.xml', $tmp_ds_directory . '/metadata/dublin_core.xml');
  $files_to_add[] = array('source' => $tmp_ds_directory . '/metadata/dublin_core.xml', 
    'dest' => 'metadata/dublin_core.xml');

  foreach ($ds_files as $ds_filename) {
    $ds_file = $tmp_ds_directory . DIRECTORY_SEPARATOR . $ds_filename;
    if (file_exists($ds_file) && is_file($ds_file)) {
    rename($ds_file, $tmp_ds_directory . '/objects/' . $ds_filename);
      // Add each file in the directory to $files_to_add.
      $files_to_add[] = array(
        'source' => $tmp_ds_directory . '/objects/' . $ds_filename,
        'dest' => 'objects/' . $ds_filename,
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

