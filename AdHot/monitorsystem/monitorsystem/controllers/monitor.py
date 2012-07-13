import logging

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from monitorsystem.lib.base import BaseController, render

from monitorsystem.lib.get_chart_info import ChartInfo, GetChartInfo, GetChartInfoByChartName, GetAllCatagory

from monitorsystem.lib.common import ImageInfo
from monitorsystem.lib.app_globals import Globals as g

import rrdtool
import time;

log = logging.getLogger(__name__)


class MonitorController(BaseController):

    def index(self):
        return 'Hello World'

    def view(self, chart = None, type = None, points = None):
      if(type == "minute"):
          ret = rrdtool.graph("./monitorsystem/public/feed.png", "--start", "now-36000", "--vertical-label=",
                "DEF:value=/tmp/feed.rrd:timeout_ratio:MAX",
                         "LINE1:value#0000FF:timeout_ratio\\r")
      
      elif(type == "hour"):
          ret = rrdtool.graph("./monitorsystem/public/feed.png", "--start", "now-10h", "-S", "3600", "--vertical-label=",
                "DEF:value=/tmp/feed.rrd:timeout_ratio:AVERAGE",
                         "LINE1:value#0000FF:timeout_ratio\\r")
      
      #return "<img src=http://10.3.17.127:5000/feed.png>"
      return "Viewing " + str(chart) + " " + str(type) + " " + str(points)

    def graph(self, chart = None, points = None, endtime = None):
      
        chartInfo = GetChartInfoByChartName(chart);
         
        end = chartInfo.num;
        limit = chartInfo.limit_value;
        
        service_name = chartInfo.catagory;
        
        c.chartname = chart
        c.photo_list = list();
        c.catalist = GetAllCatagory();
         
        for i in range(0, end):

          img_path = "/data/xce/pylons/monitorsystem/monitorsystem/public/img/" + str(chart) + str(i) + ".png"
          rrd_path = "/data/xce/monitor/data/" + str(chart) + str(i) + ".rrd";
          title = service_name + str(i); 
         
          
          if(endtime == None):
            passed_time = 60 * int(points);
            start = "now-" + str(passed_time)
            end = "now"
          else:
            now_time = int(time.time());
            lt = time.localtime(now_time);
            now_hour = int(lt.tm_hour);
            if(now_hour > 9):
              hour = 9;
              end = "now-" + str((now_hour - hour) * 3600);
              passed_time = (4 + now_hour) * 3600
              start = "now-" + str(passed_time)   

#        log.debug("-------------------------------------->start: %s ", start)

          rra1_points = 1200;
         
          if(int(points) <= rra1_points): 
            resolution = "60"
          else:
            resolution = "3600"  
          c.description = chartInfo.description; 
         # comments = "COMMENT:" + str(chartInfo.description)

          ds_def_1 = "DEF:value1=" + rrd_path + ":" + str(chartInfo.datasource) + ":AVERAGE"
          ds_def_2 = "DEF:value2=" + rrd_path + ":" + str(chartInfo.datasource) + ":MAX"
          
          if(limit > 0):
            c_def_1 = "CDEF:value3=value1," + str(limit) + ",GT,value1,UNKN,IF"
            c_def_2 = "CDEF:value4=value2," + str(limit) + ",GT,value2,UNKN,IF"
          elif(limit < 0):
            c_def_1 = "CDEF:value3=value1," + str(-limit) + ",LT,value1,UNKN,IF"
            c_def_2 = "CDEF:value4=value2," + str(-limit) + ",LT,value2,UNKN,IF"
          elif(limit == 0):
#            c_def_3 = "CDEF:value5=value1,0,EQ,100,UNKN,IF"
            c_def_3 = "CDEF:value5=value1,0,100,UN,IF"
            

          graph_def_1 = "AREA:value1#00FF00:average" 
          graph_def_2 = "LINE1:value2#0000FF:max" 
          graph_def_3 = "AREA:value3#FF0000:warning " 
          graph_def_4 = "LINE1:value4#FF0000"
          graph_def_5 = "AREA:value5#FF0000"
                 
          comments = "COMMENT:Average--------------MAX--------------MIN--------------    "
#          comments = "COMMENT:Average                 MAX                 MIN                     "
          g_print_1 = "GPRINT:value1:AVERAGE:%1.2lf"
          g_print_2 = "GPRINT:value1:MAX:%18.2lf"
          g_print_3 = "GPRINT:value1:MIN:%15.2lf"
        
          width = "370"
          height = "100"
          
          if(limit == 0):
            if(resolution == "60"):
              ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-t", title, "-S", resolution, "--start", start, "--end", end, ds_def_1, c_def_3, graph_def_1, graph_def_5, comments, g_print_1, g_print_2, g_print_3)
            else:
              ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-t", title, "-S", resolution, "--start", start, "--end", end, "--vertical-label=", ds_def_1, ds_def_2, c_def_3, graph_def_1, graph_def_2, graph_def_5)
          else:
            if(resolution == "60"):
              ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-t", title, "-S", resolution, "--start", start, "--end", end, ds_def_1, c_def_1, graph_def_1, graph_def_3, comments, g_print_1, g_print_2, g_print_3)
            else:
              ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-t", title, "-S", resolution, "--start", start, "--end", end, "--vertical-label=", ds_def_1, ds_def_2, c_def_1, c_def_2, graph_def_1, graph_def_2, graph_def_3, graph_def_4)
        
          img_display_path = "/img/" + str(chart) + str(i) + ".png"
          
          imginfo = ImageInfo()
          imginfo.img_path = img_display_path;
          imginfo.chart_name = str(chart);
          imginfo.big_photo_url = "http://10.3.19.120:5000/zoom/" + str(chart) + str(i) + "/" + str(chartInfo.datasource) + "/" + str(resolution) + "/" + title + "/" + str(points) + "/" + str(limit) + "/" + chartInfo.description 
            
#          c.photo_list.append(img_display_path)
          c.photo_list.append(imginfo)
        
         
        return render('/monitor.mako') 
#        return "Graph Viewing " + str(chart) + " " + str(type) + " " + str(points)
