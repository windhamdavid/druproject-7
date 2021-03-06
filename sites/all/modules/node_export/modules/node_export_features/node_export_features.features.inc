<?php 

/**
 * Implementation of hook_features_export_options()
 */
function node_export_features_features_export_options() {
  $options = array();

  $types = node_get_types('names');

  $query = 'SELECT n.nid, n.title, n.type, u.uuid
    FROM {node} n INNER JOIN {uuid_node} u 
    ON n.nid = u.nid ';
  
  $ommited_types = array_filter(variable_get('node_export_omitted', array()));
  if (!empty($ommited_types)) {
    $exclude = "'" . implode("', '", $ommited_types) . "'"; 
    $query .= ' WHERE n.type NOT IN (' . $exclude . ') ';
  }

  $result = db_query($query);
  
  $query .= ' ORDER BY n.type, n.title ASC';
  while ($node = db_fetch_object($result)) {
    $options[$node->uuid] = t('@type: @title', array(
      '@type' => $types[$node->type], 
      '@title' => $node->title,
    )); 
  }

  return $options;
  
}

/**
 * Implementation of hook_features_export()
 */
function node_export_features_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['module'] = 'node_export_features';

  foreach ($data as $uuid) {
    $node = node_get_by_uuid($uuid);

    $export['features']['node_export_features'][$uuid] = $uuid;
    $pipe['node'][$node->type] = $node->type;
  }

  return $pipe;
  
}

/**
 * Implementation of hook_features_export_render() 
 */
function node_export_features_features_export_render($module, $data, $export = NULL) {
  foreach ($data as $uuid) {
    $nodes[] = node_get_by_uuid($uuid);
  }
  drupal_alter('node_export', $nodes, 'export');
  
  $node_export_code = '  $nodes = ' . features_var_export($nodes, '  ') . ';';
  $node_export_code .= PHP_EOL . '  return $nodes;';
  return array('node_export_features_default' => $node_export_code);
}

/**
 * Implementation of hook_feature_revert()
 */
function node_export_features_features_revert($module = NULL) {
  node_export_features_features_rebuild($module);
}

/**
 * Implementation of hook_feature_rebuild()
 */
function node_export_features_features_rebuild($module) {
  $nodes = features_get_default('node_export_features', $module);
  // $nodes = features_get_normal('smart_node_export', $module);
  if (!empty($nodes)) {
    module_load_include('inc', 'node', 'node.pages');
    
    drupal_alter('node_export', $nodes, 'import', $used_format);
    
    foreach ($nodes as $data) {
      $original_node = (object)$data;
      node_object_prepare($node);

      $node = node_export_node_clone($original_node);
      
      drupal_alter('node_export_node', $node, $original_node, 'import');
      
      $node = node_submit($node);
      node_save($node);
      $new_nodes[] = $node;
    }
    
    drupal_alter('node_export', $new_nodes, 'after import', 'node_code');
  }
}

