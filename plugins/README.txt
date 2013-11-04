Islandora BagIt Plugins
=======================

Plugins will be fired in the order in which they sort. In general,
file addition plugins should fire before datastream copy plugins.
To ensure this happens, name file addition plugins using 'plugin_add_'
and datastream copy plugins using 'plugin_copy_'.
