<?php

/**
 * @file
 * Plugin for the BagIt Drupal module that adds URLs managed by the contrib
 * Link module (http://drupal.org/project/link) to the Bag's fetch.txt file.
 */

/**
 * Returns an array of file URLs managed by link fields.
 * Returns FALSE if no URLs are present or if Link is not enabled.
 */
function islandora_bagit_plugin_fetch_link_init($node) {
  $urls_to_add = array();
  if (!module_exists('link')) {
    return FALSE;
  }

  $link_fields = islandora_bagit_plugin_fetch_link_get_fields();
  // Check for the existence of each field in the node, and if it exists,
  // get the URLs managed by each field and add them and their names to
  // $links_to_add.
  foreach ($link_fields as $field) {
    if (isset($node->$field) && count($node->$field)) {
      // Each field can manage more than one URL.
      if (count($node->{$field}['und'])) {
        foreach ($node->{$field}['und'] as $url) {
          if (isset($url['url'])) {
            $name = islandora_bagit_plugin_fetch_link_get_name($url['url']);
            $urls_to_add[] = array('url' => $url['url'], 'name' => $name);
          }
        }
      }
    }
  }

  if (count($urls_to_add)) {
    return $urls_to_add;
  }
  else {
    return FALSE;
  }
}

/**
 * Get list of fields that are of 'link_field' type.
 */
function islandora_bagit_plugin_fetch_link_get_fields() {
  $fields = array();
  $result = db_query("SELECT field_name FROM {field_config} WHERE type = 'link_field'");
  foreach ($result as $row) {
    $fields[] = $row->field_name;
  }
  return $fields;
}

/**
 * Create a name to correspond to the incoming URL, to use in the URL's fetch.txt
 * entry. Very lazy -- just generates a sha1 hash from the URL and returns the
 * first five characters with an extension of '.dat'. Plugin writers will likely
 * want to create names that are more useful.
 */
function islandora_bagit_plugin_fetch_link_get_name($url) {
  $sha1 = sha1($url);
  return substr($sha1, 0, 5) . '.dat';
}

