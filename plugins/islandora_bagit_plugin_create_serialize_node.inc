<?php

/**
 * @file
 * File creation plugin for the BagIt Drupal module.
 *
 * Uses the XML Serializer library to dump an XML version of the incoming node
 * to a temporary directory, then returns an array containing an associative
 * array with the path and name. 
 */

/**
 * File creation plugin init function.
 */
function islandora_bagit_plugin_create_serialize_node_init($node) {
  // Include the XML_Serializer class file.
  require_once('XML/Serializer.php');
  $options = array(
    "indent"    => "    ",
    "linebreak" => "\n",
    "typeHints" => FALSE,
    "addDecl"   => TRUE,
    "encoding"  => "UTF-8",
    "rootName"   => "node",
  );

  // All file creation plugins should create a temp directory to write files to.
  // This directory and its contents will be deleted after the files in it are
  // added to the Bag.
  $bag_tmp_dir = variable_get('islandora_bagit_bag_tmp_dir', file_directory_temp()) .
    '/' . $node->nid;
  if (!file_exists($bag_tmp_dir)) {
    mkdir($bag_tmp_dir);
  }

  // Create serializer object.
  $serializer = new XML_Serializer($options);

  // Perform serialization.
  if ($serializer->serialize($node)) {
    $file_to_add = array();
    $file_to_add['name'] = 'serialized_node_' . $node->nid . '.xml';
    $temp_file_path = $bag_tmp_dir . '/' . $file_to_add['name'];
    $file_to_add['path'] = $temp_file_path;
    $output = $serializer->getSerializedData();
    file_put_contents($temp_file_path, $output);
    return array($file_to_add); 
  }
  else {
    return FALSE;
  }
}
