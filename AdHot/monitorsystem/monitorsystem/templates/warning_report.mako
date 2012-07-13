<html>

<META http-equiv="Content-type" content="text/html; charset="utf-8">

<title>服务监控系统</title>

<script type="text/javascript">
function getNewPage(v)
{
	url = "http://10.3.19.120:5000/overview/"+v+"/60"
	window.open(url);

}
</script> 



<select name="sel" onchange="getNewPage(this.value)" style="height:25px;width:140px">
<option value="">--Choose a Source</option>
<option value="FeedNews">FeedNews</option>
<option value="FeedMemcProxy">FeedMemcProxy</option>
<option value="FeedAssistantN">FeedAssistantN</option>
<option value="FeedDispatcher">FeedDispatcher</option>
<option value="FeedMini">FeedMini</option>	

</select>

<a href="http://10.3.19.120:5000/"><p style="position:absolute; top:15; left:800; color:blue; font-size:110%">回首页<p></a>

<div id="linkweb" style="position: ralative; margin-left:950px; margin-top:-20px;font-size:110%;">
</div>

<script>
window.onload=window.onload = function(){
	linkweb.innerHTML=new Date().toLocaleString();
}
setInterval("linkweb.innerHTML=new Date().toLocaleString();",1000);
</script>

<HR align=left width=100% color=#987cb9 SIZE=1>



<h1 align=center style="color:red">昨晚到今早有点儿红的服务</h1>


<%

base_top =120;
i=0;

for item in c.warning_list:
  top_value=base_top+i*30;	  
  
  link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/60/0\"" +">"
  text ="<p style=\"position:absolute; top:" +str(top_value)+ "; left:500\">"+ str(item.localtime) +" "+ item.warninginfo+"</p></a>"
  
  warning_link = link+text; 
  context.write(warning_link);
  i=i+1;
%>

 

</html>