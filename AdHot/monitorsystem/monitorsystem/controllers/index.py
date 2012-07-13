import logging

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from monitorsystem.lib.base import BaseController, render
from monitorsystem.lib.warning_data_manager import WarningDataManager
from monitorsystem.lib.get_chart_info import GetAllCatagory

log = logging.getLogger(__name__)

class IndexController(BaseController):

    def index(self):
      waring_data_manager = WarningDataManager(); 
      waring_data_manager.getRealTimeWarnings();
      c.warning_flag = waring_data_manager.getNewWarningFlag();
      c.last_night_warning_list = list(waring_data_manager.getLastNightWarnings())
      c.catagory_list = GetAllCatagory();
      log.debug("-------------------->Index ")	
      return render('/index.mako')
