<?php
class Statmodel extends CI_Model{

	
    	function __construct()
    	{
        	parent::__construct();
    	}
		 public function get_where($array)
		 {
		    $all_data = $this->mongo_db->get('user',$array);
		    return $all_data;
		 }
		 public function version_selelct($array)
		 {
			$res = $this->mongo_db->select("user",$array);
		 }
}
?>
