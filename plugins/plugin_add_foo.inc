<?php

/**
 * @file
 * File add plugin for the Islandora BagIt Drupal module. Illustrates creating
 * a file called 'foo.xml' that lists all the datastream files for the object.
 * 
 * File add plugins create and write files to the Bag's temporary directory, and
 * are fired before the file copy plugins. Any files they write to the temporary 
 * directory are added to the Bag by the copy plugin.
 */

/**
 * Always returns FALSE.
 */
function islandora_bagit_plugin_add_foo_init($islandora_object, $tmp_ds_directory) {
  $ds_files = islandora_bagit_get_ds_files($tmp_ds_directory);

  $fooxml = islandora_bagit_generate_foo_xml($ds_files);
  file_put_contents($tmp_ds_directory . DIRECTORY_SEPARATOR . 'foo.xml', $fooxml);

  return FALSE;
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

