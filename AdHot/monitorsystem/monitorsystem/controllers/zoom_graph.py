import logging
import rrdtool

from pylons import request, response, session, tmpl_context as c, url
from pylons.controllers.util import abort, redirect

from monitorsystem.lib.base import BaseController, render

from monitorsystem.lib.app_globals import Globals as g

from monitorsystem.lib.get_chart_info import GetAllCatagory

log = logging.getLogger(__name__)

class ZoomGraphController(BaseController):

    def index(self):
        # Return a rendered template
        #return render('/zoom_graph.mako')
        # or, return a string
        return 'Hello World'
     
    def zoomin(self, dbname, datasource, resolution, title, points, limit, description):
        limit = int(limit)
        img_path = "/data/xce/pylons/monitorsystem/monitorsystem/public/img/" + str(dbname) + "_big.png"
        rrd_path = "/data/xce/monitor/data/" + str(dbname) + ".rrd";
        title = str(title);
        font = "TITLE:10:/data/xce/monitor/fonts/simhei.ttf" 
        passed_time = 60 * int(points);
        start = "now-" + str(passed_time)   
        
        datasource = str(datasource)
        resolution = str(resolution)
        rra1_points = 1200; 
        
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

        width = "500"
        height = "400"
        
        comments = "COMMENT:Average--------------MAX--------------MIN--------------      "
        g_print_1 = "GPRINT:value1:AVERAGE:%1.2lf"
        g_print_2 = "GPRINT:value1:MAX:%18.2lf"
        g_print_3 = "GPRINT:value1:MIN:%15.2lf"
        
        if(limit == 0):
          if(int(points) <= rra1_points): 
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, ds_def_1, graph_def_1, comments, g_print_1, g_print_2, g_print_3)
          else:
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, "--vertical-label=", ds_def_1, ds_def_2, graph_def_1, graph_def_2, comments, g_print_1, g_print_2, g_print_3)
        else:
          if(int(points) <= rra1_points): 
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, ds_def_1, c_def_1, graph_def_1, graph_def_3, comments, g_print_1, g_print_2, g_print_3)
          else:
            ret = rrdtool.graph(img_path, "-w", width, "-h", height, "-n", font, "-t", title, "-S", resolution, "--start", start, "--vertical-label=", ds_def_1, ds_def_2, c_def_1, c_def_2, graph_def_1, graph_def_2, graph_def_3, graph_def_4)

        c.img_path = "img/" + str(dbname) + "_big.png"
        c.description = description
        c.catalist = GetAllCatagory();
        return render('/zoom.mako')
#        return "Viewing " + str(dbname) + " " + str(resolution) + " " + str(points) + " " + str(limit) 
        