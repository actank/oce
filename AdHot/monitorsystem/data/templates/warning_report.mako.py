# -*- encoding:utf-8 -*-
from mako import runtime, filters, cache
UNDEFINED = runtime.UNDEFINED
__M_dict_builtin = dict
__M_locals_builtin = locals
_magic_number = 6
_modified_time = 1308282020.448724
_template_filename='/data/xce/pylons/monitorsystem/monitorsystem/templates/warning_report.mako'
_template_uri='/warning_report.mako'
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
        __M_writer(u'<html>\n\n<META http-equiv="Content-type" content="text/html; charset="utf-8">\n\n<title>\u670d\u52a1\u76d1\u63a7\u7cfb\u7edf</title>\n\n<script type="text/javascript">\nfunction getNewPage(v)\n{\n\turl = "http://10.3.19.120:5000/overview/"+v+"/60"\n\twindow.open(url);\n\n}\n</script> \n\n\n\n<select name="sel" onchange="getNewPage(this.value)" style="height:25px;width:140px">\n<option value="">--Choose a Source</option>\n<option value="FeedNews">FeedNews</option>\n<option value="FeedMemcProxy">FeedMemcProxy</option>\n<option value="FeedAssistantN">FeedAssistantN</option>\n<option value="FeedDispatcher">FeedDispatcher</option>\n<option value="FeedMini">FeedMini</option>\t\n\n</select>\n\n<a href="http://10.3.19.120:5000/"><p style="position:absolute; top:15; left:800; color:blue; font-size:110%">\u56de\u9996\u9875<p></a>\n\n<div id="linkweb" style="position: ralative; margin-left:950px; margin-top:-20px;font-size:110%;">\n</div>\n\n<script>\nwindow.onload=window.onload = function(){\n\tlinkweb.innerHTML=new Date().toLocaleString();\n}\nsetInterval("linkweb.innerHTML=new Date().toLocaleString();",1000);\n</script>\n\n<HR align=left width=100% color=#987cb9 SIZE=1>\n\n\n\n<h1 align=center style="color:red">\u6628\u665a\u5230\u4eca\u65e9\u6709\u70b9\u513f\u7ea2\u7684\u670d\u52a1</h1>\n\n\n')
        # SOURCE LINE 47


        base_top =120;
        i=0;
        
        for item in c.warning_list:
          top_value=base_top+i*30;        
          
          link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/60/0\"" +">"
          text ="<p style=\"position:absolute; top:" +str(top_value)+ "; left:500\">"+ str(item.localtime) +" "+ item.warninginfo+"</p></a>"
          
          warning_link = link+text; 
          context.write(warning_link);
          i=i+1;
        
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['i','text','top_value','item','warning_link','link','base_top'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 61
        __M_writer(u'\n\n \n\n</html>')
        return ''
    finally:
        context.caller_stack._pop_frame()


