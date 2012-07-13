<?php


class Event extends CI_Controller {

  public $table = 'user';
	function __construct()
 	{
		  parent::__construct();
   	  $this->load->model('eventidmodel', 'showall');
 		  $this->load->helper('html');
//		  $this->load->view('event_id');

 	}
  public function index()
  {
    $Data = $this->showall->get('user');
    $res['data'] = $Data;
    $this->load->view('event_id', $res);
  }

public function eventid_insert()
       {
          $event_id = $_GET['event_id'];
          $describe = $_GET['describe'];
          $value_type = $_GET['value_type'];
          $aggregate_type = $_GET['aggregate_type'];
          $manager=$_GET['manager'];
          $data['event_id'] = $event_id;
          $data['describe'] = $describe;
          $data['aggregate_type'] = $aggregate_type;
          $data['value_type'] = $value_type;
          $data['manager']= $manager;
          $weatherinDB = $this->showall->weather_in_db($data);
          if($weatherinDB)
          {
            redirect('/event/','refresh');
          }
          else
          {
            $Data = $this->showall->event_id_insert($data);
            echo "{\"event_id\":\"".$event_id."\",\"describe\":\"".$describe."\",\"value_type\":\"".$value_type."\",\"aggregate_type\":\"".$aggregate_type."\",\"manager\":\"".$manager."\"}"; 
            redirect('/event/','refresh');
          }
       }

  public function event_remove($event_id)
  {
      $data['event_id'] = $event_id;
      echo $this->showall->remove_valuetype($data);
  }
  public function filter_select()
  {
    $data['start'] = $_GET['start'];
    $data['end'] = $_GET['end'];
    $fliter = $this->showall->fliter_select($data);
    echo json_encode($fliter);
    return 0;
  }
  public function check_id()
  {
    $data['event_id'] = $_GET['id'];
    $weatherinDB = $this->showall->weather_in_db($data);
    echo 'xxx';
    if($weatherinDB)
    {
      echo "{'rest': true, 'msg':'成功'}";
    }

    else
    {
      echo "{'rest': true, 'msg':'失败'}";
    }
    return;

  }
}
?>
