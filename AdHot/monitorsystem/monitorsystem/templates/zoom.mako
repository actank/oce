<title>监控系统</title>

<script type="text/javascript">
function getNewPage(v)
{
	url = "http://10.3.19.120:5000/overview/"+v+"/60"
	window.open(url);

}
</script> 

<a href="http://10.3.19.120:5000/"><p style="position:absolute; top:5; left:1100; color:green; font-size:100%">回首页<p></a>

<select name="sel" onchange="getNewPage(this.value)" style="height:25px;width:140px">
<option value="">--Choose a Source</option>

<%
	for cata in c.catalist:
		option="<option value=" +str(cata) + ">" + str(cata) +"</option>"
		context.write(option);
%>


</select>

<br></br>


<p align="left" style="font-size:150%; color:blue ">${c.description}</p>
<img src="http://10.3.19.120:5000/${c.img_path}">