import logging
import time
import rrdtool
import traceback

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from monitorsystem.lib.base import BaseController, render

from monitorsystem.lib.app_globals import Globals as g

log = logging.getLogger(__name__)


def  GetServicePrefix(servicename):
  i = len(servicename)
  num = 0;
  while(i >= 0):
    pos = i - 1;
    if(servicename[pos].isdigit()):
      num -= 1;
      i -= 1;
    else:
      break;
          
  service_prefix = servicename[0:num]
  return  service_prefix;
 

class DataLoadController(BaseController):
    def __init__(self):
      self.sum = 0 

    def index(self):
        return 'Hello World'
    
    
    def _update(self, rrd_path, v):
      try:
        res = rrdtool.fetch(rrd_path, "AVERAGE", "--start", "now-60", "--end", "now");
#        log.debug("-------------------->update")
      except:  
         traceback.print_exc()
         return 0
          
      if(res):
          start_time = res[0][0];
          end_time = res[0][1];  
          step = res[0][2];
          
          try:
#            if(res[2][0][0] == None):
#              rrdtool.update(rrd_path, str(start_time) + ":" + str(v));
#            else:
#              rrdtool.update(rrd_path, str(end_time) + ":" + str(v)); 
             rrdtool.update(rrd_path, str(end_time) + ":" + str(v)); 
          except:
             t = end_time + 60;
             try:
               rrdtool.update(rrd_path, str(t) + ":" + str(v));
             except:
#               log.debug("Update Error!! datasource_name: %s ", rrd_path);
               traceback.print_exc()
                 
            #log.debug("Update Error!! datasource_name: %s servicename: %s time: %d value: %d", dbname, servicename, t, v);
               return 0
             
      return 1;       
          
         
      
    def update(self, dbname = None, servicename = None, value = None):
      
      str_v = str(value)  
      v = int(float(str_v))
      t = int(time.time())
      t = (t / 10) * 10;
      t = t - t % 60
      db_path = "/data/xce/monitor/data/" + str(dbname) + "_" + str(servicename) + ".rrd";
      
      res = self._update(db_path, v)
      if(res == 0):
        log.debug("Update Error!! datasource_name: %s servicename: %s time: %d value: %d", dbname, servicename, t, v);
        return;
      
      
      
#      try:
#        rrdtool.update(db_path, str(t) + ":" + `v`);
#      except:
#        log.debug("Update Error!! datasource_name: %s servicename: %s time: %d value: %d", dbname, servicename, t, v);
#        traceback.print_exc()
#      
      log.debug("Update Sucess!! datasource_name: %s servicename: %s time: %d value: %d ", dbname, servicename, t, v);
      service_prefix = GetServicePrefix(servicename); 
      
          
      db_path = "/data/xce/monitor/data/" + str(dbname) + "_" + str(service_prefix) + ".rrd";
      ret = rrdtool.update(db_path, "N:" + `v`);

      return "Viewing " + str(dbname) + " " + str(servicename) + " " + str(value)

