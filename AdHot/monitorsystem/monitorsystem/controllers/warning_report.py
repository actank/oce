import logging

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from monitorsystem.lib.base import BaseController, render

from monitorsystem.lib.warning_data_manager import WarningDataManager

log = logging.getLogger(__name__)

class WarningReportController(BaseController):

    def index(self):
        # Return a rendered template
        #return render('/warning_report.mako')
        # or, return a string
        return 'Hello World'

    def graph(self):
      waring_data_manager = WarningDataManager(); 
      c.warning_list = list(waring_data_manager.getLastNightWarnings())
      
      return render('/warning_report.mako') 