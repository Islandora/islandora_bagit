<?php

/**
 * @file
 * Plugin for the Islandora BagIt Drupal module.
 *
 * Registers all the datastreams in an Islandora object so they are
 * copied into the 'data' directory.
 *
 * Also creates a file called 'foo.xml' and adds it to the list of files
 * to be included in the Bag.
 */

/**
 * Returns an array of source and destination file paths. Returns FALSE if no 
 * datastream files are present.
 */
function islandora_bagit_plugin_copy_ds_add_file_init($islandora_object, $tmp_ds_directory) {
  $files_to_add = array();
  $ds_files = islandora_bagit_get_ds_files($tmp_ds_directory);

  // Generate a file called foo.xml and it to the list of files that
  // should be added to the Bag. Adding this extra file is the only
  // difference between this plugin and plugin_copy_ds_basic.inc.
  $fooxml = islandora_bagit_generate_foo_xml($ds_files);
  $fooxml_path = $tmp_ds_directory . DIRECTORY_SEPARATOR . 'foo.xml';
  file_put_contents($fooxml_path, $fooxml);
  $files_to_add[] = array(
    'source' => $fooxml_path,
    'dest' => 'foo.xml',
  );

  // Add file source and dest paths for each datastream to the $files_to_add array.
  // $files_to_add['dest'] must be relative to the Bag's data subdirectory.
  foreach ($ds_files as $ds_filename) {
    // Add each file in the directory to $files_to_add.
    $source_file_to_add = $tmp_ds_directory . DIRECTORY_SEPARATOR . $ds_filename;
    if (file_exists($source_file_to_add) && is_file($source_file_to_add)) {
      $files_to_add[] = array(
        'source' => $source_file_to_add,
        // We use filename here since there are no subdirectories in in the Bag's 
        // 'data' directory.
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

/**
 * Uses XMLWriter to generate the METS XML.
 */
function islandora_bagit_generate_foo_xml($file_list) {
  $foo = new XMLWriter();
  $foo->openMemory();
  $foo->setIndent(TRUE);
  $foo->startDocument('1.0', 'utf-8', 'yes');
  $foo->startElement('files');
  foreach ($file_list as $filename) {
    $foo->writeElement('file', $filename);
  }
  $foo->endElement();
  $foo->endDocument();
  return $foo->outputMemory();
}


