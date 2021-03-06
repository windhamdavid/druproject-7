<?php

/**
 * @file
 * Drush support for the migrate module
 */

/**
 * Implementation of hook_drush_help().
 */
function migrate_drush_help($section) {
  switch ($section) {
    case 'drush:migrate-rollback':
      return dt('Rollback the destination objects from a given migration');
    case 'drush:migrate-import':
      return dt('Perform a given migration');
    case 'drush:migrate-stop':
      return dt('Stop an active migration');
    case 'drush:migrate-reset-status':
      return dt('Reset a migration\'s status to idle');
    case 'drush:migrate-status':
      return dt('List all migrations with current status');
    case 'drush:migrate-fields-destination':
      return dt('List the fields available for mapping to a destination');
    case 'drush:migrate-fields-source':
      return dt('List the fields available for mapping from a source');
    case 'drush:migrate-descriptions':
      return dt('View descriptions for a migration and all its mappings');
    case 'drush:migrate-wipe':
      return dt('Delete all nodes from specified content types.');
  }
}

/**
 * Implementation of hook_drush_command().
 */
function migrate_drush_command() {
  static $commands = FALSE;

  $migration_options = array(
    '--itemlimit' => 'The maximum number of items to migrate. If unspecified, all are migrated',
    '--feedback' => 'Frequency of progress messages, in seconds or items processed',
    '--idlist' => 'A comma delimited list of ids to import or rollback. If unspecified, migrate imports all pending items or rolls back all items for the content set.',
    '--all' => 'Process all migrations that come after the specified migration. If no value is supplied, all migrations are processed.',
    '--instrument' => 'Capture performance information (timer, memory, or all)',
    '--force' => 'Force an operation to run, even if all dependencies are not satisfied',
  );
  $items['migrate-status'] = array(
    'description' => 'List all migrations with current status.',
    'options' => array(
      'refresh' => 'Recognize new migrations and update counts',
    ),
    'arguments' => array(
      'migration' => 'Restrict to a single migration. Optional',
    ),
    'examples' => array(
      'migrate-status' => 'Retrieve status for all migrations',
      'migrate-status BeerNode' => 'Retrieve status for just one migration',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('ms'),
  );
  $items['migrate-fields-destination'] = array(
    'description' => 'List the fields available for mapping in a destination.',
    'arguments' => array(
      'migration' => 'Name of the migration or destination class to query for fields',
      'bundle' => 'For a destination class, an optional bundle',
    ),
    'examples' => array(
      'migrate-fields-destination MyNode' => 'List fields for the destination in the MyNode migration',
      'migrate-fields-destination node article' => 'List fields available for migrating into the Article node type',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mfd'),
  );
  $items['migrate-fields-source'] = array(
    'description' => 'List the fields available for mapping from a source.',
    'arguments' => array(
      'migration' => 'Name of the migration or destination class to query for fields',
    ),
    'examples' => array(
      'migrate-fields-destination MyNode' => 'List fields in the source query for the MyNode migration',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mfs'),
  );
  $items['migrate-descriptions'] = array(
    'description' => 'View descriptions for a migration and all its mappings.',
    'arguments' => array(
      'migration' => 'Name of the migration',
    ),
    'examples' => array(
      'migrate-descriptions MyNode' => 'Show descriptions for the MyNode migration',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('md'),
  );
  $items['migrate-rollback'] = array(
    'description' => 'Roll back the destination objects from a given migration',
    'options' => $migration_options,
    // We will bootstrap to login from within the command callback.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'arguments' => array(
      'migration' => 'Name of migration(s) to roll back. Delimit multiple using commas.',
    ),
    'examples' => array(
      'migrate-rollback Article' => 'Roll back the article migration',
      'migrate-rollback Article --idlist=4,9' => 'Roll back two articles. The ids refer to the value of the primary key in base table',
      'migrate-rollback User --itemlimit=50' =>
        'Roll back up to 50 items from the migration named User',
      'migrate-rollback User --feedback="60 seconds"' => 'Display a progress message every 60 seconds or less',
      'migrate-rollback Article --feedback="1000 items"' => 'Display a progress message every 1000 processed items or less',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mr'),
  );
  $migration_options['--update'] = 'In addition to processing unimported items from the source, update previously-imported items with new data';
  $migration_options['--needs-update'] =
    'Reimport up to 10K records where needs_update=1. This option is only needed when your Drupal DB is on a different DB server from your source data. Otherwise, these records get migrated with just migrate-import.';
  $migration_options['--stop'] = 'Stop specified migration(s) if applicable.';
  $migration_options['--rollback'] = 'Rollback specified migration(s) if applicable.';
  $migration_options['--file_function'] = 'Override file function to use when migrating images.';
  $items['migrate-import'] = array(
    'description' => 'Perform one or more migration processes',
    'options' => $migration_options,
    'arguments' => array(
      'migration' => 'Name of migration(s) to import. Delimit multiple using commas.',
    ),
    'examples' => array(
      'migrate-import Article' => 'Import new articles',
      'migrate-import Article --update' => 'Import new items, and also update previously-imported items',
      'migrate-import Article --idlist=4,9' => 'Import two specific articles. The ids refer to the value of the primary key in base table',
      'migrate-import Article --itemlimit=50 --stop --rollback' =>
        'Import up to 50 items after stopping and rolling back the Article migration.',
      'migrate-import User --feedback="60 seconds"' => 'Display a progress message every 60 seconds or less',
      'migrate-import User --feedback="1000 items"' => 'Display a progress message every 1000 processed items or less',
      'migrate-import --all=User' => 'Perform User migrations and all that follow it.',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mi'),
  );
  $items['migrate-stop'] = array(
    'description' => 'Stop an active migration operation',
    'options' => array('--all' => 'Stop all active migration operations'),
    'arguments' => array(
      'migration' => 'Name of migration to stop',
    ),
    'examples' => array(
      'migrate-stop Article' => 'Stop any active operation on the Article migration',
      'migrate-stop --all' => 'Stop all active migration operations',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mst'),
  );
  $items['migrate-reset-status'] = array(
    'description' => 'Reset a active migration\'s status to idle',
    'options' => array('--all' => 'Reset all active migration operations'),
    'arguments' => array(
      'migration' => 'Name of migration to reset',
    ),
    'examples' => array(
      'migrate-reset-status Article' => 'Reset any active operation on the Article migration',
      'migrate-reset-status --all' => 'Reset all active migration operations',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mrs'),
  );
  $items['migrate-wipe'] = array(
    'description' => 'Delete all nodes from specified content types.',
    'examples' => array(
       "migrate-wipe story article" => 'Delete all story and article nodes.',
    ),
    'arguments' => array(
      'type' => 'A space delimited list of content type machine readable Ids.',
    ),
    'drupal dependencies' => array('migrate'),
    'aliases' => array('mw'),
  );
  return $items;
}

/**
 * Get the value of all migrate related options. Used when spawning a subshell.
 * Don't pass along stop, update, and rollback options.
 *
 * @return
 *   An array of command specific options and their values.
 */
function drush_migrate_get_options() {
  $options = array();
  $blacklist = array('stop', 'rollback', 'update');
  $command = drush_parse_command();
  foreach ($command['options'] as $key => $value) {
    // Strip leading --
    $key = ltrim($key, '-');
    if (!in_array($key, $blacklist)) {
      $value = drush_get_option($key);
      if (isset($value)) {
        $options[$key] = $value;
      }
    }
  }
  return $options;
}

/*
 * Spawn a subshell which runs the same command we are currently running.
 */
function drush_migrate_backend_invoke() {
  $args = drush_get_arguments();
  $options = drush_migrate_get_options();
  // @todo: use drush_backend_invoke_args() as per http://drupal.org/node/658420.
  return drush_backend_invoke(implode(' ', $args), $options);
}

/**
 * A simplified version of the dashboard page.
 */
function drush_migrate_status($name = NULL) {
  try {
    $refresh = drush_get_option('refresh');

    // Validate input and load Migration(s).
    if ($name) {
      if ($migration = MigrationBase::getInstance($name)) {
        $migrations = array($migration);
      }
      else {
        return drush_set_error(dt('Unrecognized migration: !cn', array('!cn' => $name)));
      }
    }
    else {
      $migrations = migrate_migrations();
    }

    $table[] = array(dt('Name'), dt('Total'), dt('Imported'), dt('Unimported'),
      dt('Status'), dt('Last imported'));
    foreach ($migrations as $migration) {
      $has_counts = TRUE;
      if (method_exists($migration, 'sourceCount')) {
        $total = $migration->sourceCount($refresh);
      }
      else {
        $has_counts = FALSE;
        $total = dt('N/A');
      }
      if (method_exists($migration, 'importedCount')) {
        $imported = $migration->importedCount();
      }
      else {
        $has_counts = FALSE;
        $imported = dt('N/A');
      }
      if ($has_counts) {
        $unimported = $total - $imported;
      }
      else {
        $unimported = dt('N/A');
      }
      $status = $migration->getStatus();
      switch ($status) {
        case MigrationBase::STATUS_IDLE:
          $status = dt('Idle');
          break;
        case MigrationBase::STATUS_IMPORTING:
          $status = dt('Importing');
          break;
        case MigrationBase::STATUS_ROLLING_BACK:
          $status = dt('Rolling back');
          break;
        case MigrationBase::STATUS_STOPPING:
          $status = dt('Stopping');
          break;
        case MigrationBase::STATUS_DISABLED:
          $status = dt('Disabled');
          break;
        default:
          $status = dt('Unknown');
          break;
      }
      $table[] = array($migration->getMachineName(), $total, $imported, $unimported, $status,
        $migration->getLastImported());
    }
    drush_print_table($table, TRUE);
    }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

// TODO: Use drush_choice for detailed field info
function drush_migrate_fields_destination() {
  try {
    $args = func_get_args();
    $machine_name = array_shift($args);
    $migration = MigrationBase::getInstance($machine_name);
    if ($migration) {
      $destination = $migration->getDestination();
    }
    else {
      drush_log(dt('No migration found matching !machine_name',
                   array('!machine_name' => $machine_name)),
                'error');
      return;
    }
    if (method_exists($destination, fields)) {
      $table = array();
      foreach ($destination->fields() as $machine_name => $description) {
        $table[] = array($description, $machine_name);
      }
      drush_print_table($table);
    }
    else {
      drush_print(dt('!class has no fields', array('!class' => $class_name)));
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

function drush_migrate_fields_source() {
  try {
    $args = func_get_args();
    $machine_name = array_shift($args);
    $migration = MigrationBase::getInstance($machine_name);
    if ($migration) {
      $source = $migration->getSource();
    }
    else {
      drush_log(dt('No migration found matching !machine_name',
                   array('!machine_name' => $machine_name)),
                'error');
      return;
    }
    if (method_exists($source, 'fields')) {
      $table = array();
      foreach ($source->fields() as $machine_name => $description) {
        $table[] = array($description, $machine_name);
      }
      drush_print_table($table);
    }
    else {
      drush_print(dt('!class has no fields', array('!class' => $class_name)));
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

function drush_migrate_descriptions() {
  try {
    $args = func_get_args();
    $machine_name = array_shift($args);
    $migration = MigrationBase::getInstance($machine_name);
    if (!$migration) {
      return drush_set_error(dt('No class found matching !machine_name',
                   array('!machine_name' => $machine_name)));
    }
    drush_print($migration->getDescription());
    if (method_exists($migration, 'getFieldMappings')) {
      // First group the mappings
      $descriptions = array();
      foreach ($migration->getFieldMappings() as $mapping) {
        $descriptions[$mapping->getIssueGroup()][] = $mapping;
      }
      // Put out each group header
      $table = array();
      $table[] = array(dt('Destination'), dt('Source'), dt('Default'),
        dt('Description'));
      $first = TRUE;

      foreach ($descriptions as $group => $mappings) {
        if ($first) {
          $first = FALSE;
        }
        else {
          $table[] = array(' ');
        }
        // Attempt to offset and highlight the group header a bit so it stands out
        $group_header = '--- ' . strtoupper($group) . ' ---';
        $table[] = array('', $group_header);
        foreach ($mappings as $mapping) {
          if (is_array($mapping->getDefaultValue())) {
            $default = implode(',', $mapping->getDefaultValue());
          }
          else {
            $default = $mapping->getDefaultValue();
          }
          $table[] = array($mapping->getDestinationField(), $mapping->getSourceField(),
            $default, $mapping->getDescription());
        }
      }

      drush_print_table($table, TRUE);
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Roll back one specified migration
 */
function drush_migrate_rollback($args = NULL) {
  try {
    $migrations = drush_migrate_get_migrations($args);

    // Rollback in reverse order
    $migrations = array_reverse($migrations, TRUE);

    $options = array();
    if ($idlist = drush_get_option('idlist', FALSE)) {
      $options['idlist'] = $idlist;
    }
    if ($itemlimit = drush_get_option('itemlimit', FALSE)) {
      $options['itemlimit'] = $itemlimit;
    }
    $feedback = drush_get_option('feedback');
    if ($feedback) {
      $parts = explode(' ', $feedback);
      $options['feedback']['frequency'] = $parts[0];
      $options['feedback']['frequency_unit'] = $parts[1];
      if ($options['feedback']['frequency_unit'] != 'seconds' &&
          $options['feedback']['frequency_unit'] != 'items') {
        drush_set_error(NULL, dt("Invalid feedback frequency unit '!unit'",
          array('!unit' => $options['feedback']['frequency_unit'])));
        return;
      }
    }

    $instrument = drush_get_option('instrument');
    global $_migrate_track_memory, $_migrate_track_timer;
    switch ($instrument) {
      case 'timer':
        $_migrate_track_timer = TRUE;
        break;
      case 'memory':
        $_migrate_track_memory = TRUE;
        break;
      case 'all':
        $_migrate_track_timer = TRUE;
        $_migrate_track_memory = TRUE;
        break;
    }

    foreach ($migrations as $migration) {
      drush_log(dt("Rolling back '!description' migration",
        array('!description' => $migration->getMachineName())));
      $return = $migration->processRollback($options);
      // If it couldn't finish (presumably because it was appraoching memory_limit),
      // continue in a subprocess
      if ($return == MigrationBase::RESULT_INCOMPLETE) {
        drush_migrate_backend_invoke();
      }
      // If stopped, don't process any further
      elseif ($return == MigrationBase::RESULT_STOPPED) {
        break;
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
  if ($_migrate_track_memory) {
    drush_migrate_print_memory();
  }
  if ($_migrate_track_timer && !drush_get_context('DRUSH_DEBUG')) {
    drush_print_timers();
  }
}

function drush_migrate_get_migrations($args) {
  $migration_objects = migrate_migrations();

  if ($start = drush_get_option('all')) {
    // Handle custom first migration when --all=foo is supplied.
    $seen = $start === TRUE ? TRUE : FALSE;

    foreach ($migration_objects as $name => $migration) {
      if (!$seen && (strtolower($start) . 'migration' == strtolower($name))) {
        // We found our starting migration. $seen is always TRUE now.
        $seen = TRUE;
      }
      if (!$migration->getEnabled() || !$seen) {
        // This migration is disabled or is before our starting migration.
        unset($migration_objects[$name]);
      }
    }
  }
  else {
    $named_migrations = array();
    foreach (explode(',', $args) as $name) {
      $found = FALSE;
      foreach ($migration_objects as $machine_name => $migration) {
        if (strtolower($name) == strtolower($machine_name)) {
          if ($migration->getEnabled()) {
            $named_migrations[$name] = $migration;
            $found = TRUE;
            break;
          }
          else {
            drush_log(dt('Migration !name is disabled', array('!name' => $name)), 'warning');
          }
        }
      }
      if (!$found) {
        drush_log(dt('No migration with machine name !name found', array('!name' => $name)), 'error');
      }
    }
    $migration_objects = $named_migrations;
  }
  return $migration_objects;
}

// Implement drush_hook_COMMAND_validate().
function drush_migrate_import_validate($args = NULL) {
  return drush_migrate_validate_common($args);
}

// Implement drush_hook_COMMAND_validate().
function drush_migrate_stop_validate($args = NULL) {
  return drush_migrate_validate_common($args);
}

// Implement drush_hook_COMMAND_validate().
function drush_migrate_reset_status_validate($args = NULL) {
  return drush_migrate_validate_common($args);
}

// Implement drush_hook_COMMAND_validate().
function drush_migrate_rollback_validate($args = NULL) {
  return drush_migrate_validate_common($args);
}

function drush_migrate_validate_common($args) {
  if (drush_get_option('all')) {
    if (!empty($args)) {
      return drush_set_error(NULL, dt('You must specify either a migration name or --all, not both'));
    }
  }
  else {
    if (empty($args)) {
      return drush_set_error(NULL, dt('You must specify either a migration name or the --all option'));
    }
    $machine_names = explode(',', $args);

    foreach ($machine_names as $machine_name) {
      $machine_name = trim($machine_name);
      $class_name = db_select('migrate_status', 'ms')
                    ->fields('ms', array('class_name'))
                    ->condition('machine_name', $machine_name)
                    ->execute()
                    ->fetchField();
      if (!$class_name || !class_exists($class_name)) {
        drush_set_error(dt('Unrecognized migration: !name', array('!name' => $machine_name)));
      }
    }
  }

  $feedback = drush_get_option('feedback');
  if ($feedback) {
    $parts = explode(' ', $feedback);
    $options['feedback']['frequency'] = $parts[0];
    $options['feedback']['frequency_unit'] = $parts[1];
    if ($options['feedback']['frequency_unit'] != 'seconds' &&
        $options['feedback']['frequency_unit'] != 'items') {
      drush_set_error(NULL, dt("Invalid feedback frequency unit '!unit'",
        array('!unit' => $options['feedback']['frequency_unit'])));
      return;
    }
  }
}

/*
 * A 'pre' callback for migrate-import command.
 * Call migrate-stop and migrate-rollback commands if requested.
 */
function drush_migrate_pre_migrate_import($args = NULL) {
  if (drush_get_option('stop')) {
    drush_invoke('migrate-stop', $args);
  }
  if (drush_get_option('rollback')) {
    drush_invoke('migrate-rollback', $args);
  }
}

/**
 * Perform import on one or more migrations.
 *
 * @param $machine_names
 *  A comma delimited list of machine names, or the special name 'all'
 */
function drush_migrate_import($args = NULL) {
  try {
    $migrations = drush_migrate_get_migrations($args);

    $options = array();
    if ($idlist = drush_get_option('idlist', FALSE)) {
      $options['idlist'] = $idlist;
    }
    if ($itemlimit = drush_get_option('itemlimit', FALSE)) {
      $options['itemlimit'] = $itemlimit;
    }
    if ($file_function = drush_get_option('file_function', '')) {
      $options['file_function'] = $file_function;
    }
    if (drush_get_option('force', FALSE) == 1) {
      $options['force'] = TRUE;
    }
    $feedback = drush_get_option('feedback');
    if ($feedback) {
      $parts = explode(' ', $feedback);
      $options['feedback']['frequency'] = $parts[0];
      $options['feedback']['frequency_unit'] = $parts[1];
      if ($options['feedback']['frequency_unit'] != 'seconds' &&
          $options['feedback']['frequency_unit'] != 'items') {
        drush_set_error(NULL, dt("Invalid feedback frequency unit '!unit'",
          array('!unit' => $options['feedback']['frequency_unit'])));
        return;
      }
    }
    $instrument = drush_get_option('instrument');
    global $_migrate_track_memory, $_migrate_track_timer;
    switch ($instrument) {
      case 'timer':
        $_migrate_track_timer = TRUE;
        break;
      case 'memory':
        $_migrate_track_memory = TRUE;
        break;
      case 'all':
        $_migrate_track_timer = TRUE;
        $_migrate_track_memory = TRUE;
        break;
    }

    $stop = FALSE;
    foreach ($migrations as $machine_name => $migration) {
      drush_log(dt("Importing '!description' migration",
        array('!description' => $machine_name)));
      if (drush_get_option('update')) {
        $migration->prepareUpdate();
      }
      if (drush_get_option('needs-update')) {
        $needs_update = db_select('migrate_map_' . strtolower($machine_name), 'map')
                          ->fields('map', array('sourceid1'))
                          ->condition('needs_update', 1)
                          ->range(0, 10000)
                          ->execute()
                          ->fetchAllKeyed(0, 0);
        $options['idlist'] = implode(',', $needs_update);
      }
      // The goal here is to do one migration in the parent process and then
      // spawn subshells as needed when memory is depleted. We show feedback
      // after each subshell depletes itself. Best we can do in PHP.
      $i = 0;
      if ($i == 0 && !drush_get_context('DRUSH_BACKEND')) {
        // Our first pass and in the parent process. Run a migration right here.
        $return = $migration->processImport($options);
        if ($return == MigrationBase::RESULT_SKIPPED) {
          drush_log(dt('Skipping migration !name due to unfulfilled dependencies -
              use the --force option to run it anyway',
            array('!name' => $machine_name)), 'warning');
        }
        elseif ($return == MigrationBase::RESULT_STOPPED) {
          break;
        }
        $i++;
      }

      if (!drush_get_context('DRUSH_BACKEND')) {
        // Subsequent run in the parent process. Spawn subshells ad infinitum.
        while ($return == MigrationBase::RESULT_INCOMPLETE) {
          $return = drush_migrate_backend_invoke();
          // 'object' holds the return code we care about.
          $return = $return['object'];
          if ($return == MigrationBase::RESULT_SKIPPED) {
            drush_log(dt('Skipping migration !name due to unfulfilled dependencies',
              array('!name' => $machine_name)), 'warning');
          }
          elseif ($return == MigrationBase::RESULT_STOPPED) {
            $stop = TRUE;
            break;
          }
          $i++;
        }
      }
      else {
        // I'm in a subshell. Import then set return value so parent process can respawn or move on.
        $return = $migration->processImport($options);
        if ($return == MigrationBase::RESULT_SKIPPED) {
          drush_log(dt('Skipping migration !name due to unfulfilled dependencies',
            array('!name' => $machine_name)), 'warning');
        }
        drush_backend_set_result($return);
      }
      if ($i > 1) {
        drush_log(dt('Completed import of !name in !i passes.', array('!name' => $machine_name, '!i' => $i)), 'debug');
      }
      if ($stop) {
        break;
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }

  if ($_migrate_track_memory) {
    drush_migrate_print_memory();
  }

  if ($_migrate_track_timer && !drush_get_context('DRUSH_DEBUG')) {
    drush_print_timers();
  }
}

//**
// * Stop clearing or importing a given content set.
// *
// * @param $content_set
// *  The name of the Migration
// */
function drush_migrate_stop($args = NULL) {
  try {
    $migrations = drush_migrate_get_migrations($args);
    foreach ($migrations as $migration) {
      drush_log(dt("Stopping '!description' migration", array('!description' => $migration->getMachineName())));
      $migration->stopProcess();
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
// * Reset the status of a given content set.
// *
// * @param $content_set
// *  The name of the Migration
// */
function drush_migrate_reset_status($args = NULL) {
  try {
    $migrations = drush_migrate_get_migrations($args);
    foreach ($migrations as $migration) {
      drush_log(dt("Resetting '!description' migration",
        array('!description' => $migration->getMachineName())));
      $migration->resetStatus();
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * A drush command callback.
 */
function drush_migrate_wipe() {
  $types = func_get_args();
  $nids = db_select('node', 'n')
          ->fields('n', array('nid'))
          ->condition('type', $types, 'IN')
          ->execute()
          ->fetchCol();
  // TODO: Call node_delete_multiple in batches of 500 or so
  migrate_instrument_start('node_delete');
  foreach ($nids as $nid) {
    node_delete($nid);
  }
  migrate_instrument_stop('node_delete');
}

// Print all timers for the request.
function drush_migrate_print_memory() {
  global $_migrate_memory;
  $temparray = array();
  foreach ((array)$_migrate_memory as $name => $memoryrec) {
    // We have to use timer_read() for active timers, and check the record for others
    if (isset($memoryrec['start'])) {
      $temparray[$name] = migrate_memory_read($name);
    }
    else {
      $temparray[$name] = $memoryrec['bytes'];
    }
  }
  // Go no farther if there were no timers
  if (count($temparray) > 0) {
    // Put the highest cumulative times first
    arsort($temparray);
    $table = array();
    $table[] = array('Name', 'Cum (bytes)', 'Count', 'Avg (bytes)');
    foreach ($temparray as $name => $memory) {
      $count = $_migrate_memory[$name]['count'];
      if ($count > 0) {
        $avg = round($memory/$count, 0);
      }
      else {
        $avg = 'N/A';
      }
      $table[] = array($name, $memory, $count, $avg);
    }
    drush_print_table($table, TRUE);
  }
}
