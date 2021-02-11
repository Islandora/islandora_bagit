<?php

/**
 * @file
 * Plugin for the Islandora BagIt Drupal module.
 *
 * Registers all the datastreams in pages of an Islandora object so they are
 * copied into the 'data' directory.
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
function islandora_bagit_plugin_object_ds_paged_init($islandora_object, $tmp_ds_directory) {
  if (module_load_include('inc', 'islandora_paged_content', 'includes/utilities') === FALSE) {
    return FALSE;
  }
  $files_to_add = array();
  $pages = islandora_paged_content_get_pages($islandora_object);
  foreach ($pages as $page) {
    $page_object = islandora_object_load($page['pid']);
    $ds_files = islandora_bagit_retrieve_datastreams($page_object, $tmp_ds_directory);

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
          'dest' => str_replace(array(':', '-'), '_', $page['pid']) . DIRECTORY_SEPARATOR . basename($ds_filename),
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
