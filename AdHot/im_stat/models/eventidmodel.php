<?php
class Eventidmodel extends CI_Model{

	
    	function __construct()
    	{
        	parent::__construct();
    	}
  public function get($array)
  {
    $all_data = $this->mongo_db->get('user');
    return $all_data;
  }
  public function event_id_insert($array)
  {
		$res = $this->mongo_db->insert("user",$array);
	}

  public function weather_in_db($array){
    $new_array['event_id'] = $array['event_id'];
    $res = $this->mongo_db->get_where('user',$new_array);
    if($res)
      return true;
    else
      return false;
  }
  public function version_list($array)
  {
	$this->mongo_db->switch_db('test');
	$start = $array['id'] * 100000;
	$end = $array['id'] * 100000 + 99999;
  $mo = new Mongo();
  $res = $mo->test->command(array("distinct" => "stat", "key" => "version","query" => array("event_id"=>array('$gte'=>$start,'$lte'=>$end)))); 
  $index=0;
  $res_json = array();
  foreach($res['values'] as $key=>$val){
    $res_json[$index] = $val;
    $index += 1;
  }
  return $res_json;
  }
  public function monitor_version_list($array)
  {
  $this->mongo_db->switch_db('test');
  $start = $array['id'] * 100000;
  $end = $array['id'] * 100000 + 99999;
  $mo = new Mongo();
  $res = $mo->test->command(array("distinct" => "monitor", "key" => "version","query" => array("event_id"=>array('$gte'=>$start,'$lte'=>$end)))); 
  $index=0;
  $res_json = array();
  foreach($res['values'] as $key=>$val){
    $res_json[$index] = $val;
    $index += 1;
  }
  return $res_json;
  }


public function subver_list($array)
  {
  $index['version'] = $array['version'];
  $start = $array['id'] * 100000;
  $end = $array['id'] * 100000 + 99999;
  $mo = new Mongo();
  if($index['version'] == "0")
    $res = $mo->test->command(array("distinct" => "stat", "key" => "subver","query" => array("event_id"=>array('$gte'=>$start,'$lte'=>$end))));
  else
    $res = $mo->test->command(array("distinct" => "stat", "key" => "subver","query" => array("event_id"=>array('$gte'=>$start,'$lte'=>$end),"version"=>$index['version'])));
    $index=0;
    $res_json = array();
    foreach($res['values'] as $key=>$val){
            $res_json[$index] = $val;
                $index += 1;
              }
    return $res_json;
  }


public function monitor_subver_list($array)
  {
  $index['version'] = $array['version'];
  $start = $array['id'] * 100000;
  $end = $array['id'] * 100000 + 99999;
  $mo = new Mongo();
  if($index['version'] == "0")
    $res = $mo->test->command(array("distinct" => "monitor", "key" => "subver","query" => array("event_id"=>array('$gte'=>$start,'$lte'=>$end))));
  else
    $res = $mo->test->command(array("distinct" => "monitor", "key" => "subver","query" => array("event_id"=>array('$gte'=>$start,'$lte'=>$end),"version"=>$index['version'])));
  $index=0;
  $res_json = array();
  foreach($res['values'] as $key=>$val){
    $res_json[$index] = $val;
    $index += 1;
    }
  return $res_json;
  }

public function startver_list($array)
  {
  $end = $array['id'] * 100000 + 99999;
  $start = $array['id'] * 100000;
  $end = $array['id'] * 100000 + 99999;
  $s = "{$start}";
  $e = "{$end}";
  $mo = new Mongo();
  $res = $mo->test->command(array("distinct" => "updata", "key" => "start_ver","query" => array("event_id"=>array('$gte'=>$s,'$lte'=>$e))));
  $index=0;
  $res_json = array();
  foreach($res['values'] as $key=>$val){
         $res_json[$index] = $val;
                  $index += 1;
                }   
  return $res_json;
  }


public function endver_list($array)
  {
  $end = $array['id'] * 100000 + 99999;
  $start = $array['id'] * 100000;
  $end = $array['id'] * 100000 + 99999;
  $s = "{$start}";
  $e = "{$end}";
  $mo = new Mongo();
  $res = $mo->test->command(array("distinct" => "updata", "key" => "end_ver","query" => array("event_id"=>array('$gte'=>$s,'$lte'=>$e))));
  $index=0;
  $res_json = array();
  foreach($res['values'] as $key=>$val){
    $res_json[$index] = $val;
    $index += 1;
          }
  return $res_json;
  }

  public function fliter_select($array)
  {
   $this->mongo_db->switch_db('test'); 
   $start = $array['start'];
   $end = $array['end'];
   $s = "{$start}";
   $e = "{$end}"; 
   $this->mongo_db->order_by(array("event_id" => 1));
   $res = $this->mongo_db->where_between('event_id',$s,$e)->get('user');
   $index = 0;
   $res_json = array();
   foreach($res as $col)
   {
     $a = array();
     $a['event_id'] = $col['event_id'];
     $a['describe'] = $col['describe'];
     $a['value_type'] = $col['value_type'];
     $a['aggregate_type'] = $col['aggregate_type'];
     $a['manager'] = $col['manager'];
     $res_json[$index] = $a; 
     $index += 1;
  }
   return $res_json;
    
  }

  public function monitor_select($array)
  {
  $start = "{$array['start']}";
  $end = "{$array['end']}";
  $eid_start = (int)$array['eid_start'];
  $eid_end =  (int)$array['eid_end'];
  $start =  substr_replace($start,"-",10, 1);
  $end =  substr_replace($end,"-",10, 1);
  $client = "{$array['client']}";
  $version = "{$array['version']}";
  $subver = "{$array['subver']}";
  $all_event = $this->mongo_db->get('user');
  foreach($all_event as $key=>$val)
  {
    $contr[$val['event_id']] = $val['aggregate_type'];
    $descr[$val['event_id']] = $val['describe'];
  }
  $mo = new Mongo();
  $coll = $mo->test->monitor;
  if($version == "0" && $subver == "0")
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end));
  else if ($version == "0")
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end),"subver"=>$subver);
  else if ($subver == "0")
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end),"version"=>$version);
  else
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end),"version"=>$version,"subver"=>$subver);
  $fields = array("time","event_id","cnt","val");
  $cursor = $coll->find($query,$fields);
  $index = 0;
  foreach($cursor as $key=>$val)
   {
   $id = $val['event_id'];
   if(array_key_exists($id,$contr))
    {
    $tol_id[$id] = 1; //时间段内的所有的EVENT_id;
    }
   }
  $final = array();

  foreach($cursor as $key=>$val)
   {
   $eid = $val['event_id'];
   $time = $val['time'];
   $time = substr_replace($time, " ", 10, 1);
   $value = $val['cnt'];
   $v = array($val['event_id']=>$val['val']);
   if(array_key_exists($eid,$contr))
      {
    $final[$time][$eid] = $value;
     }
   }

   $data_list = array();
   foreach($cursor as $key=>$val)
    {
    $eid = $val['event_id'];
    $time = $val['time'];
    $cnt = $val['cnt'];
    $val = $val['val'];
    if(!array_key_exists($eid,$data_list))
    {
      $data_list[$eid] = array();
      $data_list[$eid][$val] = array();
      $data_list[$eid][$val][$time] = $cnt;
    }
    else
    {
      if(!array_key_exists($val,$data_list[$eid]))
      {
       $data_list[$eid][$val] = array();
       $data_list[$eid][$val][$time] = $cnt;
      }
    else
      {
       $data_list[$eid][$val][$time] = 0;
       $data_list[$eid][$val][$time] = (int)$data_list[$eid][$val][$time] + (int)$cnt;
      }
    }
  }

//   print_r($data_list);
  date_default_timezone_set('UTC');
  $start = strtotime($start);
  $end = strtotime($end);
  for(;$start<=$end;$start+=60){
    $time_list[] = date('Y-m-d-H:i',$start);
  }
  $json_table = array();
  foreach($tol_id as $key=>$val)
   {
    $xindex = 0;
    $json_table[$index] = array();
    $json_table[$index]['time'] = $time_list;
    $json_table[$index]['events'] = array();
    $json_table[$index]['events']['id'] = $key;
    $json_table[$index]['events']['desc'] = $descr[$key];
    $json_table[$index]['events']['aggregate_type'] = $contr[$key];
    $json_table[$index]['events']['series'] = array();
    $json_table[$index]['events']['series'][$xindex] = array();
    foreach($data_list[$key] as $keyx=>$valx)  //keyx 就是VAL
      {
        $json_table[$index]['events']['series'][$xindex]['name'] = $keyx;
        foreach($time_list as $tim)
         {
        if(array_key_exists($tim,$data_list[$key][$keyx]))
          {
          $json_table[$index]['events']['series'][$xindex]['data'][] = (int)$data_list[$key][$keyx][$tim];
          }
        else
          {
          $json_table[$index]['events']['series'][$xindex]['data'][] =  0;
          }
       }
       $xindex ++;
     }
   $index ++;
   }
 return $json_table;
  }

  public function total_select($array)
  {
	$start = "{$array['start']} -00:00";
  $xstart = "{$array['start']}";

  $eid_start = (int)$array['eid_start'];
  $eid_end =  (int)$array['eid_end'];

	$end = "{$array['end']} -23:59";
	$xend = "{$array['end']}";
  $client = "{$array['client']}";
  $version = "{$array['version']}";
  $subver = "{$array['subver']}";
  $all_event = $this->mongo_db->get('user');
  foreach($all_event as $key=>$val)
  {
    $contr[$val['event_id']] = $val['aggregate_type'];
    $descr[$val['event_id']] = $val['describe'];
  }
  $mo = new Mongo();
  $coll = $mo->test->stat;
  if($version == "0" && $subver == "0")
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end));
  else if ($version == "0")
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end),"subver"=>$subver);
  else if ($subver == "0")
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end),"version"=>$version);
  else
    $query = array("time"=>array('$gte'=>$start,'$lte'=>$end),"event_id"=>array('$gte'=>$eid_start,'$lte'=>$eid_end),"version"=>$version,"subver"=>$subver);
  $fields = array("time","event_id","cnt","val");
  $cursor = $coll->find($query,$fields);
  $index = 0;
  foreach($cursor as $key=>$val)
  {
    $id = $val['event_id'];
     if(array_key_exists($id,$contr))
     {
    $tol_id[$id] = 1; //时间段内的所有的EVENT_id;
     }
  }
  $final = array();
  foreach($cursor as $key=>$val)
  {
      $eid = $val['event_id'];
      $time = $val['time'];
      $time = substr_replace($time, " ", 10, 1);
      $value = $val['cnt'];
      $v = array($val['event_id']=>$val['val']);
      if(array_key_exists($eid,$contr))
      {
      $final[$time][$eid] = $value;
      }
  }
  //测试新TABLE
  $data_list = array();
  foreach($cursor as $key=>$val)
  {
    $eid = $val['event_id'];
    $time = $val['time'];
    $cnt = $val['cnt'];
    $val = $val['val'];

    if(!array_key_exists($eid,$data_list))
    {
      $data_list[$eid] = array();
      $data_list[$eid][$val] = array();
      $data_list[$eid][$val][$time] = $cnt;
    }  
    else
    {
      if(!array_key_exists($val,$data_list[$eid]))
      {
        $data_list[$eid][$val] = array();
        $data_list[$eid][$val][$time] = $cnt;
      }
      else
      {
        $data_list[$eid][$val][$time] = 0;
        $data_list[$eid][$val][$time] = (int)$data_list[$eid][$val][$time] + (int)$cnt;
      }
    }
  }

    $new_table = array();
    $x = 0;
    $y = 0;
    $new_table[$x][$y] = "时间";
    foreach($tol_id as $key=>$val)
      {
      $y += 1;
      $new_table[$x][$y] = $descr[$key];
       }

    $x += 1;
    $y = 0;
    foreach($final as $key=>$val)
       {
      $new_table[$x][$y] = $key;
      foreach($tol_id as $key1=>$val1)
          {
         $y += 1;
         if(array_key_exists($key1,$val)){
           $new_table[$x][$y] = $val[$key1];}
         else
           $new_table[$x][$y] = "\t";
         }
      $x += 1;
      $y = 0;
   }
  date_default_timezone_set('prc');
  $start = strtotime($xstart);
  $end = strtotime($xend);
  for(;$start<=$end;$start+=24*3600){
    $time_list[] = date('Y-m-d',$start - 28800);
  }
  $json_table = array();
  foreach($tol_id as $key=>$val)
  {
    $xindex = 0;
    $json_table[$index] = array();
    $json_table[$index]['time'] = $time_list;
    $json_table[$index]['events'] = array();
    $json_table[$index]['events']['id'] = $key;
    $json_table[$index]['events']['desc'] = $descr[$key];
    $json_table[$index]['events']['aggregate_type'] = $contr[$key];
    $json_table[$index]['events']['series'] = array();
    $json_table[$index]['events']['series'][$xindex] = array();
    foreach($data_list[$key] as $keyx=>$valx)  //keyx 就是VAL
    {

      $json_table[$index]['events']['series'][$xindex]['name'] = $keyx;
        foreach($time_list as $tim)
        {
          if(array_key_exists($tim,$data_list[$key][$keyx]))
          {
              $json_table[$index]['events']['series'][$xindex]['data'][] = (int)$data_list[$key][$keyx][$tim];
          }
        else
          {
              $json_table[$index]['events']['series'][$xindex]['data'][] =  0; 
          }
        }
      $xindex ++;
    }
    $index ++;
  }
 return $json_table;
}


/*


  $new_table = array();
  $x = 0;
  $y = 0;
  $new_table[$x][$y] = "时间";
  foreach($tol_id as $key=>$val)
    {
      $y += 1;
      $new_table[$x][$y] = $descr[$key];
    }

  $x += 1;
  $y = 0;
  foreach($final as $key=>$val)
  {

      $new_table[$x][$y] = $key;
      foreach($tol_id as $key1=>$val1)
        {   
        $y += 1;
     if(array_key_exists($key1,$val)){
          $new_table[$x][$y] = $val[$key1];}
             else
     $new_table[$x][$y] = "\t";
       }   
          $x += 1;
          $y = 0;
  }
  return $new_table;
  }
 */
  public function remove_valuetype($array)
  {
 //   $option['event_id'] = $array['event_id'];
    $mo = new Mongo();
    $coll = $mo->test->user;
    $field = array("event_id"=>$array['event_id']);
    $options = array("safe"=>true);
    $res = $coll->remove($field,$options);
    return $res ? 1 : 0;
  }

}
?>
