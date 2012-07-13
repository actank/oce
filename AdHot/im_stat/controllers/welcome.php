<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Welcome extends CI_Controller {

  function __construct()
      {
              parent::__construct();
                    print("111");
      }
	public function index()
	{
    print ("111");
		$this->load->view('welcome_message');
  }
  public function test()
  {
    echo phpinfo();
      }


}

/* End of file welcome.php */
/* Location: ./application/controllers/welcome.php */
