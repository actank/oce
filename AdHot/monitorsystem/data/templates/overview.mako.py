# -*- encoding:utf-8 -*-
from mako import runtime, filters, cache
UNDEFINED = runtime.UNDEFINED
__M_dict_builtin = dict
__M_locals_builtin = locals
_magic_number = 6
_modified_time = 1308029653.8115439
_template_filename='/data/xce/pylons/monitorsystem/monitorsystem/templates/overview.mako'
_template_uri='/overview.mako'
_template_cache=cache.Cache(__name__, _modified_time)
_source_encoding='utf-8'
from webhelpers.html import escape
_exports = []


def render_body(context,**pageargs):
    context.caller_stack._push_frame()
    try:
        __M_locals = __M_dict_builtin(pageargs=pageargs)
        c = context.get('c', UNDEFINED)
        str = context.get('str', UNDEFINED)
        __M_writer = context.writer()
        # SOURCE LINE 1
        __M_writer(u'<html>\t\n\n<title>\u670d\u52a1\u76d1\u63a7\u7cfb\u7edf</title>\n\n<META http-equiv="Content-type" content="text/html; charset="utf-8">\n\n<script type="text/javascript">\nfunction getNewPage(v)\n{\n\turl = "http://10.3.19.120:5000/overview/"+v+"/60"\n\twindow.open(url);\n\n}\n</script> \n\n\n<style type="text/css">\n\n<script>\nwindow.onload=window.onload = function(){\n\t//linkweb.innerHTML=new Date().toLocaleString();\n\tgetTime();\n}\n\nfunction getTime()\n{\n\ttime = new Date().toLocaleString();\n\tarr=time.split("(");\n  \n\t//linkweb.innerHTML ="<p style=\\"font-size:80%; position:absolute; top:30; left:1030;\\">"+ arr[0]+"\\<p>";\n\tlinkweb.innerHTML= arr[0];\n}\n\nfunction reloadThisPage()\n{\n\tlocation.reload()\n}\n\nsetTimeout(\'reloadThisPage()\',60000);\n\nsetInterval("getTime()",1000);\n</script>\n\n\n<style type="text/css">\n\n\t#container {\n\t\tmargin: 0px;\n\t\tpadding: 0px 175px 0px 110px;\n\t\tposition: relative;\n\t}\n\t\n\t#pageHeader {\n\t\theight: 87px;\n\t\tpadding-top: 20px;\n\t}\n\t\n\t#imgRegion {\n\t\tmargin-bottom: 40px;\n\t\tpadding-left: 10px;\n\t\twidth: 80%;\n\t}\n\t\n\t#linkList {\n\t\tmargin-left: 600px;\n\t\tposition: absolute;\n\t\tright: 0px;\n\t\ttop: 100px;\n\t}\n\t\n\t#linkList2 {\n\t\tmargin-top: 20px;\n\t\tpadding: 10px;\n\t\twidth: 280px;\n\t}\n\t\n\t#graph {\n\t\tmargin-top: 15px;\n\t\tmargin-left: 15px;\n\t}\n\t\n\t\n\t#zoom {\n\t\tposition:relative; \n\t\ttop:-300; \n\t\tleft:-20\uff1b\n\t}\n\t\n    #back_home{\n    \tmargin-top: 15px;\n\t\tmargin-left: 500px;\n\t\tcolor:green; \n\t\tfont-size:100%;\n    }\n\t\n\t#right_conner{\n\t    padding-left: 0px;\n\t\tmargin-left: 600px;\n\t\tposition: absolute;\n\t\tright: 0px;\n\t\ttop: 0px;\n\t}\n\t\n\t#last_night{\n\t\t\n\t}\n\t\n</style>\n\n\n<div id="container">\n\n\t<div id="pageHeader">\n\n\t\t<select name="sel" onchange="getNewPage(this.value)" style="height:25px;width:140px">\n\t\t\t<option value="">--Choose a Source</option>\n\t\t\t<option value="">--Choose a Source</option>\n\n\t\t\t')
        # SOURCE LINE 119

        for cata in c.catalist:
                option="<option value=" +str(cata) + ">" + str(cata) +"</option>"
                context.write(option);
                                
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['cata','option'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 123
        __M_writer(u'\n\t\n\t\t</select>\n\n\t\t<table style="margin-top:10px">\n\t\t\t<tr>\n\t\t\t\t\t<th>\u65f6\u95f4\u6bb5:<th>\n    \t\t\t\t<th><a href="http://10.3.19.120:5000/overview/')
        # SOURCE LINE 130
        __M_writer(escape(c.catagory_name))
        __M_writer(u'/60">1H</a></th>\n    \t\t\t\t<th><a href="http://10.3.19.120:5000/overview/')
        # SOURCE LINE 131
        __M_writer(escape(c.catagory_name))
        __M_writer(u'/180">3H</a></th>\n    \t\t\t\t<th><a href="http://10.3.19.120:5000/overview/')
        # SOURCE LINE 132
        __M_writer(escape(c.catagory_name))
        __M_writer(u'/360">6H</a></th>\n    \t\t\t\t<th><a href="http://10.3.19.120:5000/overview/')
        # SOURCE LINE 133
        __M_writer(escape(c.catagory_name))
        __M_writer(u'/720">12H</a></th>\n    \t\t\t\t<th><a href="http://10.3.19.120:5000/overview/')
        # SOURCE LINE 134
        __M_writer(escape(c.catagory_name))
        __M_writer(u'/1440">1D</a></th>\n    \t\t\t\t<th><a href="http://10.3.19.120:5000/overview/')
        # SOURCE LINE 135
        __M_writer(escape(c.catagory_name))
        __M_writer(u'/10080">1W</a></th>\n\t\t\t</tr>\n\t\t</table>\n\n\n\t\t<div id="right_conner">\n\t\t\t<a href="http://10.3.19.120:5000/"><p>\u56de\u9996\u9875<p></a>\n\n\t\t\t<a href="http://10.3.19.120:5000/warning_report"><p id="last_night">\u6628\u665a\u5230\u4eca\u65e9\u7684\u72b6\u51b5<p></a>\n\n\t\t\t<div id="linkweb">\n\t\t\t</div>\n\t   </div>\n\t\t\n\t\t<HR style="position:absolute; top:80;" align=left width=100% color=#987cb9 SIZE=1>\n\n\t</div>\n\n\t<div id="imgRegion">\n\t\t')
        # SOURCE LINE 154

        from monitorsystem.controllers.overview import ImageInfo
        from monitorsystem.lib.app_globals import Globals as g
        
        base_top =120;
        i=0;
        
        for imginfo in c.photoinfo_list:
                link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(imginfo.chart_name) + "/" + str(c.points)+ "\"" +">"
                origin_imglink= "<img id=\"graph\" src=" +str(imginfo.img_path) + "></a>" 
                content = link+origin_imglink
                context.write(content);
                
                firstpart_link = "<a title=\"Zoom Out\"  href=" + "\""+ imginfo.big_photo_url+ "\"" +">"
                big_img_src = "<img src=\"http://10.3.19.120:5000/graph_zoom.gif\" id=\"zoom\">"
                big_imglink=firstpart_link+big_img_src+"</a>"
                context.write(big_imglink);
                        
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['ImageInfo','g','i','big_imglink','content','link','firstpart_link','origin_imglink','big_img_src','base_top','imginfo'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 171
        __M_writer(u'\t\n\t</div>\n\n\n\n\n\t<div id="linkList">\n\t\t<div id="linkList2">\n\t\t\t<p >\u4e0b\u9762\u7684\u670d\u52a1\u51fa\u95ee\u9898\u5566!!!</p>\n\t\t\t')
        # SOURCE LINE 180

        for item in g.warning_list:
                link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/180\"" +">"
                text ="<p>"+ str(item.localtime) +" "+item.warninginfo +"</p></a>"
                warning_link = link+text; 
                context.write(warning_link);
                                
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['item','warning_link','link','text'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 186
        __M_writer(u'\n\t\t</div>\t\n\t\n\t</div>\n\n\n</div>\n\n</html>\n')
        return ''
    finally:
        context.caller_stack._pop_frame()


