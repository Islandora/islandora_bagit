<?php

/**
 * @file
 * File creation plugin for the BagIt Drupal module.
 *
 * Creates a simple METS representation of the node.
 *
 * This plugin is meant to illustrate how a METS document is created for
 * a node and is not meant to be normative. Also, it assumes that all files
 * attached to the node are added to the Bag via some other plugin, such as
 * the bagit_plugin_copy_fileimage plugin.
 */

/**
 * File creation plugin init function.
 */
function islandora_bagit_plugin_create_mets_init($node) {
  // All file creation plugins should create a temp directory to write files to.
  // This directory and its contents will be deleted after the files in it are
  // added to the Bag.
  $bag_tmp_dir = variable_get('islandora_bagit_bag_tmp_dir', file_directory_temp()) .
    '/' . $node->nid;
  if (!file_exists($bag_tmp_dir)) {
    mkdir($bag_tmp_dir);
  }

  // Generate the METS document.
  if ($node->title) {
    $file_to_add = array();
    $file_to_add['name'] = 'mets_node_' . $node->nid . '.xml';
    $temp_file_path = $bag_tmp_dir . '/' . $file_to_add['name'];
    $file_to_add['path'] = $temp_file_path;
    $output = islandora_bagit_generate_mets($node);
    file_put_contents($temp_file_path, $output);
    return array($file_to_add); 
  }
  else {
    return FALSE;
  }
}

/**
 * Uses XMLWriter to generate the METS XML.
 */
function islandora_bagit_generate_mets($node) {
  $mets = new XMLWriter();
  $mets ->openMemory();
  $mets->setIndent(TRUE);
  $mets->startDocument('1.0', 'utf-8', 'yes');

  $mets->startElementNS('mets', 'mets', 'http://www.loc.gov/METS/');
  $mets->writeattribute('xmlns:dc', 'http://purl.org/dc/elements/1.1/');
  $mets->writeattribute('xmlns:xlink', 'http://www.w3.org/1999/xlink');

  // Generate creator and metsHdr.
  $mets->startElementNS('mets', 'metsHdr', NULL);
  $mets->writeattribute('CREATEDATE', date(DATE_ATOM, time()));
  $mets->startElementNS('mets', 'agent', NULL);
  $mets->writeattribute('ROLE', 'creator');
  $mets->writeElementNS('mets', 'name', NULL, htmlspecialchars($node->name));
  $mets->endElement(); // agent
  $mets->endElement(); // metsHdr

  // Generate dmdSec, using the node title, body, creation date.
  $mets->startElementNS('mets', 'dmdSec', NULL);
  $mets->writeattribute('ID', 'DMD1');
  $mets->startElementNS('mets', 'mdWrap', NULL);
  $mets->writeattribute('MIMETYPE', 'text/xml');
  $mets->writeattribute('MDTYPE', 'DC');
  $mets->writeattribute('LABEL', 'Dublin Core record');
  $mets->startElementNS('mets', 'xmlData', NULL);
  $mets->writeElementNS('dc', 'title', NULL, htmlspecialchars($node->title));
  // We will use the language-neutral 'und' value for the body, which assumes that
  // we are in a unilingual environment.
  $mets->writeElementNS('dc', 'description', NULL, htmlspecialchars($node->body['und'][0]['value']));
  $mets->writeElementNS('dc', 'date', NULL, date('Y-m-d', $node->created));
  $mets->endElement(); // xmlData
  $mets->endElement(); // mdWrap 
  $mets->endElement(); // dmdSec 

  // Generate fileSec.
  $mets->startElementNS('mets', 'fileSec', NULL);
  $mets->startElementNS('mets', 'fileGrp', NULL);
  $mets->writeattribute('ID', 'FGRP1');
  // Get all the files attached to this node (in this case, of type 'file'
  // or 'image') and generate mets:file markup for each one.
  $file_fields = islandora_bagit_get_fields_fileimage();
  $file_id = 0;
  // We store the file IDs in an array for populating the structMap.
  $file_ids = array();
  // Check for the existence of each field in the node, and if any exist,
  // wrap their paths in METS file, FLocat, etc. markup. This plugin doesn't
  // copy files, it leaves that to the bagit_plugin_copy_fileimage plugin.
  foreach ($file_fields as $field) {
    if (isset($node->$field) && count($node->$field)) {
      // Each field can manage more than one file.
      if (count($node->{$field}['und'])) {
        foreach ($node->{$field}['und'] as $file) {
          $file_id++;
          $file_ids[] = $file_id;
          $mets->startElementNS('mets', 'file', NULL);
          $mets->writeattribute('ID', 'File_' . $file_id);
          $mimetype = file_get_mimetype($file['uri']);
          $mets->writeattribute('MIMETYPE', $mimetype);
          $mets->startElementNS('mets', 'FLocat', NULL);
          // In our Bag, filenames are relative to this METS document.
          $mets->writeattribute('xlink:href', $file['filename']);
          $mets->writeattribute('LOCTYPE', 'OTHER');
          $mets->endElement(); // FLocat
          $mets->endElement(); // file
        }
      }
    }
  }
  $mets->endElement(); // fileGrp
  $mets->endElement(); // fileSec 

  // Generate structMap.
  $mets->startElementNS('mets', 'structMap', NULL);
  $mets->writeattribute('TYPE', 'physical');
  $mets->startElementNS('mets', 'div', NULL);
  foreach ($file_ids as $id) {
    $mets->startElementNS('mets', 'fptr', NULL);
    $mets->writeattribute('FILEID', 'File_' . $id);
    $mets->endElement(); // fptr
  }
  $mets->endElement(); // div
  $mets->endElement(); // structMap 

  $mets->endDocument();
  return $mets->outputMemory();
}
