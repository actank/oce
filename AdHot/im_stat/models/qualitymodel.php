<?php
class Qualitymodel extends CI_Model{
  function __construct()
  {
    	parent::__construct();
  }

  public function value_type_select()
  {
    $all_event = $this->mongo_db->get('user');

    foreach($all_event as $key=>$val) { 
      $descr[$val['event_id']] = $val['describe'];
    }   

    $mo = new Mongo();
    $events_coll = $mo->test->user;
    $monitor_coll= $mo->test->quality_monitor_data;

    $query = array("value_type" => "1");
    $fields = array("event_id");
    $monitor_events = $events_coll->find($query, $fields);
    $monitor_list = array();
    foreach($monitor_events as $key => $val)
    {
       $id = "{$val['event_id']}";
       $monitor_list[$id]["desc"] = array();
       array_push($monitor_list[$id]["desc"], $descr[$id]);

       $monitor_list[$id]["time"] = array();
       $monitor_list[$id]["cnt"] = array();
       $query_quality = array("event_id"=>$id); 
       $fields_quality = array("time","val");

       $cursor_quality = $monitor_coll->find($query_quality, $fields_quality);
       foreach($cursor_quality as $key_quality=>$val_quality)
       {
         array_push($monitor_list[$id]["time"], $val_quality['time']);
         array_push($monitor_list[$id]["cnt"], $val_quality['val']);
       }
    }
    return $monitor_list;
  }

  public function version_select($array)
  {
    $this->mongo_db->switch_db('quality_control');
    $start = $array['id'] * 100000;
    $end = $array['id'] * 100000 + 99999;
    $s = "{$start}";
    $e = "{$end}";
    $mo = new Mongo();
    $coll = $mo->test->quality_monitor_data;
    $res = $mo->test->command(array("distinct" => "quality_monitor_data", "key" => "version","query" => array("event_id"=>array('$gte'=>$s,'$lte'=>$e)))); 
    $index=0;
    $res_json = array();
    foreach($res['values'] as $key=>$val){
      $res_json[$index] = $val;
      $index += 1;
    }
    return $res_json;
  }

  public function subver_select($array)
  {
    $index['version'] = $array['version'];
    $start = $array['id'] * 100000;
    $end = $array['id'] * 100000 + 99999;
    $s = "{$start}";
    $e = "{$end}";
    $mo = new Mongo();
    if($index['version'] == "0")
      $res = $mo->test->command(array("distinct" => "quality_monitor_data", "key" => "subver","query" => array("event_id"=>array('$gte'=>$s,'$lte'=>$e))));
    else
      $res = $mo->test->command(array("distinct" => "quality_monitor_data", 
         "key" => "subver","query" => array("event_id"=>array('$gte'=>$s,'$lte'=>$e),
         "version"=>$index['version'])));

    $index=0;
    $res_json = array();
    foreach($res['values'] as $key=>$val) {
      $res_json[$index] = $val;
      $index += 1;
    }
    return $res_json;
  }
}
?>
