<?php

/**
 * @file
 * Plugin for the Islandora BagIt Drupal module.
 *
 * Registers all the datastreams  the immediate children of an Islandora 
 * compound object so they are copied into the 'data' directory.
 */

/**
 * Returns an array of source and destination file paths.
 *
 * @param object $islandora_object
 *   The Islandora object to create a Bag for.
 *
 * @param string $tmp_ds_directory
 *   The temporary directory where the datastream files have been downloaded.
 *
 * @return array|bool
 *   An array of source and destination file paths, or FALSE
 *   if no datastream files are present.
 */
function islandora_bagit_plugin_object_ds_compound_init($islandora_object, $tmp_ds_directory) {
  // What Content Models are compound?
  $supported_cmodels = array('islandora:compoundCModel');
  $found_cmodels = array_intersect($islandora_object->models, $supported_cmodels);
  if (empty($found_cmodels)) {
    return FALSE;
  }
  if (module_load_include('module', 'islandora_compound_object', 'islandora_compound_object') === FALSE) {
    return FALSE;
  }
  $files_to_add = array();
  $children = islandora_compound_object_get_parts($islandora_object->id);
  $children_count = 0;
  foreach ($children as $child_id) {
    $child_object = islandora_object_load($child_id);
    $children_count++;
    $tmp_ds_subdir = $tmp_ds_directory . DIRECTORY_SEPARATOR . 'c' . $children_count;
    if (!file_exists($tmp_ds_subdir)) {
      mkdir($tmp_ds_subdir, 0777, TRUE);
    }
    $ds_files = islandora_bagit_retrieve_datastreams($child_object, $tmp_ds_subdir);

    // Add file source and dest paths for each datastream to the $files_to_add
    // array. $files_to_add['dest'] must be relative to the Bag's data
    // subdirectory.
    foreach ($ds_files as $ds_filename) {
      // Add each file in the directory to $files_to_add.
      $source_file_to_add = $ds_filename;
      if (file_exists($source_file_to_add) && is_file($source_file_to_add)) {
        $files_to_add[] = array(
          'source' => $source_file_to_add,
          // Each page becomes a subdirectory in the Bag's 'data' directory.
          'dest' => str_replace(array(':', '-'), '_', $child_id) . DIRECTORY_SEPARATOR . basename($ds_filename),
        );
      }
    }
  }

  if (count($files_to_add)) {
    return $files_to_add;
  }
  else {
    return FALSE;
  }
}
