<?php
class Quality_monitor extends CI_Controller {
  function __construct()
  {
    parent::__construct();
    $this->load->model('qualitymodel', 'quality');
    $this->load->helper('html');
  }
  public function index()
  {
    $this->load->view('qualityview');
  }
  public function typevalue_select()
  {
    $monitor_list = $this->quality->value_type_select();
    echo json_encode($monitor_list);
  }
}
?>
