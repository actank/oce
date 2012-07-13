<?php


class Stat extends CI_Controller {

	function __construct()
 	{
		  parent::__construct();
      
			$this->load->model('statmodel', 'showall');
			$this->load->helper('html');
			$this->load->view('stat');

 	}
public function index()
        {
         $tim = "2011-08-05-19";
         $timindex['time'] = $tim;
 //        $Data = $this->showall->get_where('user',$timindex);
  //       $res['data'] = $Data;
        }


}
?>
