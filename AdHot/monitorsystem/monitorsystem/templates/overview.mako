<html>	

<title>服务监控系统</title>

<META http-equiv="Content-type" content="text/html; charset="utf-8">

<script type="text/javascript">
function getNewPage(v)
{
	url = "http://10.3.19.120:5000/overview/"+v+"/60"
	window.open(url);

}
</script> 


<style type="text/css">

<script>
window.onload=window.onload = function(){
	//linkweb.innerHTML=new Date().toLocaleString();
	getTime();
}

function getTime()
{
	time = new Date().toLocaleString();
	arr=time.split("(");
  
	//linkweb.innerHTML ="<p style=\"font-size:80%; position:absolute; top:30; left:1030;\">"+ arr[0]+"\<p>";
	linkweb.innerHTML= arr[0];
}

function reloadThisPage()
{
	location.reload()
}

setTimeout('reloadThisPage()',60000);

setInterval("getTime()",1000);
</script>


<style type="text/css">

	#container {
		margin: 0px;
		padding: 0px 175px 0px 110px;
		position: relative;
	}
	
	#pageHeader {
		height: 87px;
		padding-top: 20px;
	}
	
	#imgRegion {
		margin-bottom: 40px;
		padding-left: 10px;
		width: 80%;
	}
	
	#linkList {
		margin-left: 600px;
		position: absolute;
		right: 0px;
		top: 100px;
	}
	
	#linkList2 {
		margin-top: 20px;
		padding: 10px;
		width: 280px;
	}
	
	#graph {
		margin-top: 15px;
		margin-left: 15px;
	}
	
	
	#zoom {
		position:relative; 
		top:-300; 
		left:-20；
	}
	
    #back_home{
    	margin-top: 15px;
		margin-left: 500px;
		color:green; 
		font-size:100%;
    }
	
	#right_conner{
	    padding-left: 0px;
		margin-left: 600px;
		position: absolute;
		right: 0px;
		top: 0px;
	}
	
	#last_night{
		
	}
	
</style>


<div id="container">

	<div id="pageHeader">

		<select name="sel" onchange="getNewPage(this.value)" style="height:25px;width:140px">
			<option value="">--Choose a Source</option>
			<option value="">--Choose a Source</option>

			<%
				for cata in c.catalist:
					option="<option value=" +str(cata) + ">" + str(cata) +"</option>"
					context.write(option);
			%>
	
		</select>

		<table style="margin-top:10px">
			<tr>
					<th>时间段:<th>
    				<th><a href="http://10.3.19.120:5000/overview/${c.catagory_name}/60">1H</a></th>
    				<th><a href="http://10.3.19.120:5000/overview/${c.catagory_name}/180">3H</a></th>
    				<th><a href="http://10.3.19.120:5000/overview/${c.catagory_name}/360">6H</a></th>
    				<th><a href="http://10.3.19.120:5000/overview/${c.catagory_name}/720">12H</a></th>
    				<th><a href="http://10.3.19.120:5000/overview/${c.catagory_name}/1440">1D</a></th>
    				<th><a href="http://10.3.19.120:5000/overview/${c.catagory_name}/10080">1W</a></th>
			</tr>
		</table>


		<div id="right_conner">
			<a href="http://10.3.19.120:5000/"><p>回首页<p></a>

			<a href="http://10.3.19.120:5000/warning_report"><p id="last_night">昨晚到今早的状况<p></a>

			<div id="linkweb">
			</div>
	   </div>
		
		<HR style="position:absolute; top:80;" align=left width=100% color=#987cb9 SIZE=1>

	</div>

	<div id="imgRegion">
		<%
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
		%>	
	</div>




	<div id="linkList">
		<div id="linkList2">
			<p >下面的服务出问题啦!!!</p>
			<%
				for item in g.warning_list:
					link = "<a href= \"http://10.3.19.120:5000/monitor/" + str(item.chartname) + "/180\"" +">"
					text ="<p>"+ str(item.localtime) +" "+item.warninginfo +"</p></a>"
					warning_link = link+text; 
  					context.write(warning_link);
			%>
		</div>	
	
	</div>


</div>

</html>
