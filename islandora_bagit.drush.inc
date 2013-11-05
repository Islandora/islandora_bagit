<?php

/**
 * @file
 * Drush integration file for the Islandora BagIt module.
 *
 * Usage: drush --user=UID create-islandora-bag PID
 *   where UID is the user ID or user name of the fedoraAdmin user 
 *   (or equivalent) and PID is the PID of the Islandora object.
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
      'PID'    => dt('The PID for the object you want to create a Bag for.'),
    ),
    'examples' => array(
      'Standard example' => 'drush --user=fedoraAdmin create-islandora-bag islandora:190',
      'Alias example' => 'drush --user=fedoraAdmin cib islandora:190',
    ),
    'aliases' => array('cib'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
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
  global $user;
  $fedora_url = variable_get('islandora_base_url', 'http://localhost:8000/fedora');

  // Use Tuque to connect to Fedora repo and connect.
  islandora_bagit_drush_include_tuque();
  $connection = new RepositoryConnection($fedora_url, $user->name, $user->pass);
  $connection->reuseConnection = TRUE;
  $repository = new FedoraRepository(
       new FedoraApi($connection),
       new SimpleCache());

  // Invoke Bag creation function from islandora_bagit module.
  if ($islandora_object = $repository->getObject($pid)) {
    islandora_bagit_create_bag($islandora_object);
  }
}

/**
 * Includes Tuque files.
 *
 * @return Boolean
 *   TRUE if the API was included, FALSE otherwise.
 */
function islandora_bagit_drush_include_tuque() {
  if (!file_exists('sites/all/libraries/tuque')) {
    return drush_set_error(DRUSH_FRAMEWORK_ERROR, dt('Tuque API files not found.'));
  }
  @include_once 'sites/all/libraries/tuque/Datastream.php';
  @include_once 'sites/all/libraries/tuque/FedoraApi.php';
  @include_once 'sites/all/libraries/tuque/FedoraApiSerializer.php';
  @include_once 'sites/all/libraries/tuque/Object.php';
  @include_once 'sites/all/libraries/tuque/RepositoryConnection.php';
  @include_once 'sites/all/libraries/tuque/Cache.php';
  @include_once 'sites/all/libraries/tuque/RepositoryException.php';
  @include_once 'sites/all/libraries/tuque/Repository.php';
  @include_once 'sites/all/libraries/tuque/FedoraRelationships.php';
  return TRUE;
}
