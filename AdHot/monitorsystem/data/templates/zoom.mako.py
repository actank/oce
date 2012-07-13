# -*- encoding:utf-8 -*-
from mako import runtime, filters, cache
UNDEFINED = runtime.UNDEFINED
__M_dict_builtin = dict
__M_locals_builtin = locals
_magic_number = 6
_modified_time = 1308030260.176861
_template_filename='/data/xce/pylons/monitorsystem/monitorsystem/templates/zoom.mako'
_template_uri='/zoom.mako'
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
        __M_writer(u'<title>\u76d1\u63a7\u7cfb\u7edf</title>\n\n<script type="text/javascript">\nfunction getNewPage(v)\n{\n\turl = "http://10.3.19.120:5000/overview/"+v+"/60"\n\twindow.open(url);\n\n}\n</script> \n\n<a href="http://10.3.19.120:5000/"><p style="position:absolute; top:5; left:1100; color:green; font-size:100%">\u56de\u9996\u9875<p></a>\n\n<select name="sel" onchange="getNewPage(this.value)" style="height:25px;width:140px">\n<option value="">--Choose a Source</option>\n\n')
        # SOURCE LINE 17

        for cata in c.catalist:
                option="<option value=" +str(cata) + ">" + str(cata) +"</option>"
                context.write(option);
        
        
        __M_locals_builtin_stored = __M_locals_builtin()
        __M_locals.update(__M_dict_builtin([(__M_key, __M_locals_builtin_stored[__M_key]) for __M_key in ['cata','option'] if __M_key in __M_locals_builtin_stored]))
        # SOURCE LINE 21
        __M_writer(u'\n\n\n</select>\n\n<br></br>\n\n\n<p align="left" style="font-size:150%; color:blue ">')
        # SOURCE LINE 29
        __M_writer(escape(c.description))
        __M_writer(u'</p>\n<img src="http://10.3.19.120:5000/')
        # SOURCE LINE 30
        __M_writer(escape(c.img_path))
        __M_writer(u'">')
        return ''
    finally:
        context.caller_stack._pop_frame()


