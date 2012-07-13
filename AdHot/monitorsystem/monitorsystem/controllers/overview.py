import logging
import rrdtool

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from monitorsystem.lib.base import BaseController, render

from monitorsystem.lib.get_chart_info import ChartInfo, GetChartInfo, GetAllCatagory
from monitorsystem.lib.common import ImageInfo
from monitorsystem.lib.app_globals import Globals as g

from monitorsystem.lib.warning_data_manager import WarningDataManager

import codecs

log = logging.getLogger(__name__)

class OverviewController(BaseController):

    def index(self):
        return 'Hello World'

    def graph(self, catagory = None, points = 60):
      infolist = GetChartInfo(catagory);
      c.photoinfo_list = list();
      c.catalist = GetAllCatagory();
      c.catagory_name = catagory
      c.points = points;
      waring_data_manager = WarningDataManager(); 
      waring_data_manager.getRealTimeWarnings();
      c.warning_flag = waring_data_manager.getNewWarningFlag(); 
      
      log.debug("warning_flag: %s ", c.warning_flag)
      
      
      for info in infolist:
        chartname = info.chartname;
        num = info.num;
        description = info.description
        datasource = info.datasource
        limit = info.limit_value
        
#        log.debug("-------------------------------------->chartname: %s ", chartname)
        
        img_path = "/data/xce/pylons/monitorsystem/monitorsystem/public/img/" + str(chartname) + ".png"
        rrd_path = "/data/xce/monitor/data/" + str(chartname) + ".rrd";
        title = description;
        font = "TITLE:10:/data/xce/monitor/fonts/simhei.ttf" 
       

        
        passed_time = 60 * int(points);
        start = "now-" + str(passed_time) 

        rra1_points = 1200; 
        if(int(points) <= rra1_points): 
          resolution = "60"
        else:
          resolution = "3600"  
        
        ds_def_1 = "DEF:value1=" + rrd_path + ":" + datasource + ":AVERAGE"
        ds_def_2 = "DEF:value2=" + rrd_path + ":" + datasource + ":MAX"
       
        if(limit > 0):
          c_def_1 = "CDEF:value3=value1," + str(limit) + ",GT,value1,UNKN,IF"
          c_def_2 = "CDEF:value4=value2," + str(limit) + ",GT,value2,UNKN,IF"
        elif(limit < 0):
          c_def_1 = "CDEF:value3=value1," + str(-limit) + ",LT,value1,UNKN,IF"
          c_def_2 = "CDEF:value4=value2," + str(-limit) + ",LT,value2,UNKN,IF"
         
        graph_def_1 = "AREA:value1#00FF00:average" 
        graph_def_2 = "LINE1:value2#0000FF:max"
        graph_def_3 = "AREA:value3#FF0000:warning "
        graph_def_4 = "LINE1:value4#FF0000"
        
        width = "350"
        height = "200"
        
        comments = "COMMENT:Average--------------MAX--------------MIN------------ --"
        g_print_1 = "GPRINT:value1:AVERAGE:%1.2lf"
        g_print_2 = "GPRINT:value1:MAX:%18.2lf"
        g_print_3 = "GPRINT:value1:MIN:%15.2lf"
        
        if(limit == 0):
          if(int(points) <= rra1_points): 
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, ds_def_1, graph_def_1, comments, g_print_1, g_print_2, g_print_3)
          else:
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, "--vertical-label=", ds_def_1, ds_def_2, graph_def_1, graph_def_2)
        else:
          if(int(points) <= rra1_points): 
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, ds_def_1, c_def_1, graph_def_1, graph_def_3, comments, g_print_1, g_print_2, g_print_3)
          else:
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, "--vertical-label=", ds_def_1, ds_def_2, c_def_1, c_def_2, graph_def_1, graph_def_2, graph_def_3, graph_def_4)

        img_display_path = "/img/" + str(chartname) + ".png"
        imginfo = ImageInfo()
        imginfo.img_path = img_display_path;
        imginfo.chart_name = str(chartname);
#        imginfo.big_photo_url = "http://10.3.17.127:5000/zoom/" + str(chartname) + "/" + datasource + "/" + str(resolution) + "/" + description + "/" + str(points) + "/" + str(limit)
#        imginfo.big_photo_url = "http://10.3.17.127:5000/zoom/" + str(chartname) + "/" + datasource + "/" + str(resolution) + "/" + "" + "/" + str(points) + "/" + str(limit)
        utf8_description = unicode(info.description, "utf-8")
        imginfo.big_photo_url = "http://10.3.17.127:5000/zoom/" + str(chartname) + "/" + str(datasource) + "/" + str(resolution) + "/" + str(catagory) + "/" + str(points) + "/" + str(limit) + "/" + utf8_description
        
        c.photoinfo_list.append(imginfo)
      
        
      return render('/overview.mako')       
      
           
#      return
