<?php


class Event_check extends CI_Controller {

	function __construct()
 	{
		  parent::__construct();
   	  $this->load->model('eventidmodel', 'showall');
 		  $this->load->helper('html');
      $this->load->library('parser');

 	}
public function index()
        {
          $Data = $this->showall->get('user');
          $res['data'] = $Data;
        }

public function version_list()
{
	$client['id'] = $_GET['id'];
  $versions = $this->showall->version_list($client);
  echo json_encode($versions);
	return 0;
}
public function monitor_version_list()
{
  $client['id'] = $_GET['id'];
  $versions = $this->showall->monitor_version_list($client);
  echo json_encode($versions);
  return 0;
}
public function subver_list()
  {
    $client['id'] = $_GET['id'];
    $client['version'] = $_GET['version'];
    $subvers = $this->showall->subver_list($client);
    echo json_encode($subvers);
    return 0;
  }

public function monitor_subver_list()
   {
    $client['id'] = $_GET['id'];
    $client['version'] = $_GET['version'];
    $subvers = $this->showall->monitor_subver_list($client);
    echo json_encode($subvers);
    return 0;
   }

public function startver_list()
    {
    $client['id'] = $_GET['id'];
    $subvers = $this->showall->startver_list($client);
    echo json_encode($subvers);
    return 0;
   }   
public function endver_list()
   {
   $client['id'] = $_GET['id'];
   $subvers = $this->showall->endver_list($client);
   echo json_encode($subvers);
   return 0;
  }


public function total_select()
{
	$client = $_GET;
	$table_return = $this->showall->total_select($client);
  echo json_encode($table_return);
}
public function monitor_select()
{
  $client = $_GET;
  $table_return = $this->showall->monitor_select($client);
  echo json_encode($table_return);
}

  public function check_id()
  {
    $data['event_id'] = $_GET['id'];
    $weatherinDB = $this->showall->weather_in_db($data);
    if($weatherinDB)
    {
      echo '0';
    }

    else
    {
      echo '1';
    }
    return;

  }
}
?>
