<?php

/**
 * @file
 * Plugin for the BagIt Drupal module that adds files attached to nodes using
 * the core Drupal 7 File or Image modules.
 */

/**
 * Returns an array of file paths and names. Returns FALSE if no file are
 * present or if FileField is not enabled.
 */
function islandora_bagit_plugin_copy_fileimage_init($node) {
  $files_to_add = array();
  if (!module_exists('file') && !module_exists('image')) {
    return FALSE;
  }

  $file_fields = islandora_bagit_get_fields_fileimage();
  // Check for the existence of each field in the node, and if it exists,
  // get the files managed by each field and add their paths and names to
  // $files_to_add.
  foreach ($file_fields as $field) {
    if (isset($node->$field) && count($node->$field)) {
      // Each field can manage more than one file.
      if (count($node->{$field}['und'])) {
        foreach ($node->{$field}['und'] as $file) {
          $extras = islandora_bagit_build_file_extra(array($file));
          $full_path = drupal_realpath($file['uri']);
          $files_to_add[] = array(
            'path' => $full_path,
            'name' => $file['filename'],
            'extra' => $extras,
          );
        }
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

/**
 * Get list of fields that are of 'file' or 'image' type.
 */
function islandora_bagit_plugin_copy_fileimage_get_fields() {
  $fields = array();
  $result = db_query("SELECT field_name FROM {field_config} WHERE type = 'file' OR type = 'image'");
  foreach ($result as $row) {
    $fields[] = $row->field_name;
  }
  return $fields;
}
