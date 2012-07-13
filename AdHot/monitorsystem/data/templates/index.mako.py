# -*- encoding:utf-8 -*-
from mako import runtime, filters, cache
UNDEFINED = runtime.UNDEFINED
__M_dict_builtin = dict
__M_locals_builtin = locals
_magic_number = 6
_modified_time = 1308030237.6492779
_template_filename='/data/xce/pylons/monitorsystem/monitorsystem/templates/index.mako'
_template_uri='/index.mako'
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
        __M_writer(u'\n\n\n<title>\u670d\u52a1\u76d1\u63a7\u7cfb\u7edf</title>\n\n<style>\na:link {text-decoration: none;}\na:visited {text-decoration: none;}\na:active {text-decoration: none;}\na:hover {text-decoration: none;}\n</style> \n\n\n<script type="text/javascript">\nwindow.onload=window.onload = function(){\n  linkweb.innerHTML=new Date().toLocaleString();\n}\nsetInterval("linkweb.innerHTML=new Date().toLocaleString();",1000);\n</script>\n\n\n<h1 align="center" style="font-size:250%; color:blue">\u670d\u52a1\u76d1\u63a7\u7cfb\u7edf</h1>\n\n<div id="linkweb" style="position: ralative; margin-left:1000px; margin-top:-20px;font-size:100%;">\n</div>\n\n<HR align=left width=100% color=#987cb9 SIZE=1>\n\n\n<table style="position:absolute; top:90; left:400;" border="0" cellspacing="0" cellpadding="0">\n<tr>\n<td height=2000 style="border-right: 2px solid #af3f83; " >&nbsp;</td>\n</tr>\n</table>\n\n\n<table style="position:absolute; top:90; left:900;" border="0" cellspacing="0" cellpadding="0">\n<tr>\n<td height=2000 style="border-right: 2px solid #af3f83; " >&nbsp;</td>\n</tr>\n</table>\n\n\n<p  style="position:absolute; top:90; left:30; color:fuchsia; font-size:100%">\u6628\u665a\u5230\u4eca\u65e9\u6709\u70b9\u513f\u7ea2\u7684\u670d\u52a1</p>\n\n')
        # SOURCE LINE 46


        from monitorsystem.controllers.overview import ImageInfo
        from monitorsystem.lib.app_globals import Globals as g
        
        
        base_top =120;  
        i=0;
        
        for item in c.last_night_warning_list:
          top_value=base_top+i*30;        
          
          link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/60/0\"" +">"
          text ="<p style=\"position:absolute; top:" +str(top_value)+ "; font-size:90%; color:green; left:30\">"+ str(item.localtime) +" "+ item.warninginfo+"</p></a>"
          
          warning_link = link+text; 
          context.write(warning_link);
          i=i+1;
        
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['ImageInfo','g','i','text','top_value','item','warning_link','link','base_top'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 64
        __M_writer(u'\n\n\n<p  style="position:absolute; top:80; left:560; color:red; font-size:175%">\u76d1\u63a7\u670d\u52a1\u5217\u8868</p>\n\n')
        # SOURCE LINE 69


        base_top =130;  
        i=0;
        
        for cata in c.catagory_list:
          top_value=base_top+i*40;        
          
          link = "<a href= \"http://10.3.19.120:5000/overview/" + str(cata) + "/60\"" +">"
          text ="<p style=\"position:absolute; top:" +str(top_value)+ "; left:580; font-size:140%; text-decoration: underline; \">"+ str(cata) +"</p></a>"
          
          warning_link = link+text; 
          context.write(warning_link);
          i=i+1;
        
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['cata','i','text','top_value','warning_link','link','base_top'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 83
        __M_writer(u'\n\n<!--\n<center>\n<a href="http://10.3.19.120:5000/overview/FeedNews/60"   style="font-size:150% ">FeedNews</a><br/><br/>\n<a href="http://10.3.19.120:5000/overview/FeedDispatcher/60"   style="font-size:150% ">FeedDispatcher</a><br/><br/>\n<a href="http://10.3.19.120:5000/overview/FeedMini/60"   style="font-size:150% ">FeedMini</a><br/><br/>\n<a href="http://10.3.19.120:5000/overview/FeedMemcProxy/60"   style="font-size:150% ">FeedMemcProxy</a><br/><br/>\n<a href="http://10.3.19.120:5000/overview/FeedDispatcher/60"   style="font-size:150% ">FeedDispatcher</a><br/><br/>\n</center>\n-->\n\n\n<p style="position:absolute; top:90; left:990; color:blue; font-size:110% ">\u521a\u624d\u51fa\u95ee\u9898\u7684\u670d\u52a1</p>\n\n')
        # SOURCE LINE 98


        base_top =120;
        i=0;
        
        for item in g.warning_list:
          
          top_value=base_top+i*30;
          
          link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/180\"" +">"
          text ="<p style=\"position:absolute; top:" +str(top_value)+ "; font-size:90%; color:green; left:990\">"+ str(item.localtime) +" "+ item.warninginfo +"</p></a>"
          
          warning_link = link+text; 
          context.write(warning_link);
          i=i+1;        
        
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['i','text','top_value','item','warning_link','link','base_top'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 113
        __M_writer(u'\n')
        return ''
    finally:
        context.caller_stack._pop_frame()


