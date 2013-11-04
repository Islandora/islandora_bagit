<?php

/**
 * @file
 * Drush integration file for the Islandora BagIt module.
 */

/**
 * Implements hook_drush_help().
 */
function islandora_bagit_drush_help($command) {
  switch ($command) {
    case 'drush:create-islandora-bag':
      return dt('Create a Bag for an Islandora object');
  }
}

/**
 * Implements hook_drush_command().
 */
function islandora_bagit_drush_command() {
  $items = array();
  $items['create-islandora-bag'] = array(
    'description' => dt('Creates a Bag for an Islandora object.'),
    'arguments'   => array(
      'arg1'    => dt('The PID for the object you want to create a Bag for.'),
    ),
    'examples' => array(
      'Standard example' => 'drush create-islandora-bag islandora:190',
    ),
    'aliases' => array('cib'),
  );
  return $items;
}

/**
 * Callback function for drush create-islandora-bag. 
 *
 * @param $pid
 *   The PID of the Islandora object to create a Bag for.
 */
function drush_islandora_bagit_create_islandora_bag($pid = NULL) {
  // Placeholder code: check if the argument was passed in and just print it out.
  if (isset($pid)) {
   drush_print($pid);
  }
 
  //log to the command line with an OK status
  drush_log('Running create-islandora-bag', 'ok');
}
