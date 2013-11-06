<?php

/**
 * @file
 * Drush integration file for the Islandora BagIt module.
 *
 * Usage: drush --user=UID create-islandora-bag [object|collection] PID
 *   where UID is the user ID or user name of the fedoraAdmin user 
 *   (or equivalent), 'object' or 'collection' indicates whether you want to
 *   create a Bag for a single object or a Bag for every member of a
 *   collection, and PID is the PID of the Islandora object.
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
      '[object|collection]'    => dt('Whether you want to create a single Bag for an object or Bags for all members of a collection.'),
      'PID'    => dt('The PID for the object or collection you want to create a Bag for.'),
    ),
    'examples' => array(
      'Standard example' => 'drush --user=fedoraAdmin create-islandora-bag object islandora:190',
      'Standard example (for collection)' => 'drush --user=fedoraAdmin create-islandora-bag collection islandora:sp_basic_image_collection',
      'Alias example' => 'drush --user=fedoraAdmin cib object islandora:190',
    ),
    'aliases' => array('cib'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Callback function for drush create-islandora-bag. 
 *
 * @param string $type
 *   Either 'object' (for a single Islandora object) or 'collection'
 *     (for all objects in a collection).
 *
 * @param string $pid
 *   The PID of the Islandora object to create a Bag for.
 */
function drush_islandora_bagit_create_islandora_bag($type = 'object', $pid = NULL) {
  // Validate the arguments.
  if (!in_array($type, array('object', 'collection'))) {
    drush_set_error('BagIt type not specified', "Sorry, type argument must be either 'object' or 'collection'.");
    return FALSE;
  }
  if (!$pid) {
    drush_set_error('Islandora PID not specified', "Sorry, you need to supply a PID.");
    return FALSE;
  }

  global $user;
  $fedora_url = variable_get('islandora_base_url', 'http://localhost:8000/fedora');

  // Use Tuque to connect to Fedora repo.
  islandora_bagit_drush_include_tuque();
  $connection = new RepositoryConnection($fedora_url, $user->name, $user->pass);
  $connection->reuseConnection = TRUE;
  $repository = new FedoraRepository(new FedoraApi($connection), new SimpleCache());

  // List of objects to create Bags for.
  $objects_to_bag = array();

  // To create a Bag for every object in a collection, query the rindex to get all
  // the objects that have a 'isMemberOfCollection' relationship with the specified
  // collection and add all their PIDs to the list..
  if ($type == 'collection') { 
    $ri_query = 'select $object from <#ri>
      where  $object <fedora-rels-ext:isMemberOfCollection> <info:fedora/' . $pid . '>';
    $members = $repository->ri->itqlQuery($ri_query, 'unlimited');
    foreach ($members as $member) {
      $objects_to_bag[] = $member['object']['value'];
   }
  }
  // If we're dealing with a single object, just add its PID to the list.
  if ($type == 'object') {
    $objects_to_bag  = array($pid);
  }

  if (count($objects_to_bag) === 0) {
    drush_set_error('No objects to Bag', "Sorry, there are no objects to Bag. Please check your command-line arguments.");
    return FALSE;
  }

  if ($connection) {
    foreach ($objects_to_bag as $object_to_bag_pid) {    
      try {
        $islandora_object = $repository->getObject($object_to_bag_pid);
        islandora_bagit_create_bag($islandora_object);
      }
      catch (RepositoryException $e) {
        drush_print("Sorry, Islandora cannot create the Bag: " . $e->getMessage());
      }
    }
  }
}

/**
 * Includes Tuque files.
 *
 * @return bool
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
