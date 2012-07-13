


<title>服务监控系统</title>

<style>
a:link {text-decoration: none;}
a:visited {text-decoration: none;}
a:active {text-decoration: none;}
a:hover {text-decoration: none;}
</style> 


<script type="text/javascript">
window.onload=window.onload = function(){
  linkweb.innerHTML=new Date().toLocaleString();
}
setInterval("linkweb.innerHTML=new Date().toLocaleString();",1000);
</script>


<h1 align="center" style="font-size:250%; color:blue">服务监控系统</h1>

<div id="linkweb" style="position: ralative; margin-left:1000px; margin-top:-20px;font-size:100%;">
</div>

<HR align=left width=100% color=#987cb9 SIZE=1>


<table style="position:absolute; top:90; left:400;" border="0" cellspacing="0" cellpadding="0">
<tr>
<td height=2000 style="border-right: 2px solid #af3f83; " >&nbsp;</td>
</tr>
</table>


<table style="position:absolute; top:90; left:900;" border="0" cellspacing="0" cellpadding="0">
<tr>
<td height=2000 style="border-right: 2px solid #af3f83; " >&nbsp;</td>
</tr>
</table>


<p  style="position:absolute; top:90; left:30; color:fuchsia; font-size:100%">昨晚到今早有点儿红的服务</p>

<%

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
%>


<p  style="position:absolute; top:80; left:560; color:red; font-size:175%">监控服务列表</p>

<%

base_top =130;	
i=0;

for cata in c.catagory_list:
  top_value=base_top+i*40;	  
  
  link = "<a href= \"http://10.3.19.120:5000/overview/" + str(cata) + "/60\"" +">"
  text ="<p style=\"position:absolute; top:" +str(top_value)+ "; left:580; font-size:140%; text-decoration: underline; \">"+ str(cata) +"</p></a>"
  
  warning_link = link+text; 
  context.write(warning_link);
  i=i+1;
%>

<!--
<center>
<a href="http://10.3.19.120:5000/overview/FeedNews/60"   style="font-size:150% ">FeedNews</a><br/><br/>
<a href="http://10.3.19.120:5000/overview/FeedDispatcher/60"   style="font-size:150% ">FeedDispatcher</a><br/><br/>
<a href="http://10.3.19.120:5000/overview/FeedMini/60"   style="font-size:150% ">FeedMini</a><br/><br/>
<a href="http://10.3.19.120:5000/overview/FeedMemcProxy/60"   style="font-size:150% ">FeedMemcProxy</a><br/><br/>
<a href="http://10.3.19.120:5000/overview/FeedDispatcher/60"   style="font-size:150% ">FeedDispatcher</a><br/><br/>
</center>
-->


<p style="position:absolute; top:90; left:990; color:blue; font-size:110% ">刚才出问题的服务</p>

<%

base_top =120;
i=0;

for item in g.warning_list:
  
  top_value=base_top+i*30;
  
  link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/180\"" +">"
  text ="<p style=\"position:absolute; top:" +str(top_value)+ "; font-size:90%; color:green; left:990\">"+ str(item.localtime) +" "+ item.warninginfo +"</p></a>"
  
  warning_link = link+text; 
  context.write(warning_link);
  i=i+1;	
%>
