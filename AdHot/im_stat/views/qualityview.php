<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta name="keywords" content="校内,校内网,IM统计监控平台"/>

<link media="all" type="text/css" rel="stylesheet" href="/css/bolt.css"/>
<link media="all" type="text/css" rel="stylesheet" href="/css/imstat.css"/>
<link media="all" type="text/css" rel="stylesheet" href="/css/jquery-ui.css"/>
<link media="all" type="text/css" rel="stylesheet" href="/css/jqchart.css"/>

<script type="text/javascript" src="/js/jquery.min.js"></script>
<script type="text/javascript" src="/js/jquery-ui.min.js"></script>
<script type="text/javascript" src="/js/jquery-ui-timepicker-addon.js"></script>
<script type="text/javascript" src="/js/highcharts.js"></script>
<script type="text/javascript" src="/js/imstat.js"></script>


<script type="text/javascript">
function monitor_list_select()
{

	client = $("#select_client").val();
  version = $("#select_version").val();
	subver = $("#select_subver").val();
	start = $("#start_time").val();
	end = $("#end_time").val();
  fliter1 = $("#select_client").attr("value");
  var eid_start = fliter1 * 100000;
  var eid_end = eid_start + 99999;

	if(client == '0' || subver=='' || start=='' || end=='' || client == ''){
		alert('不能为空');
		return false;
	}
  $('#charts_container').html('正在加载...');
	$.ajax({
		type: "get",//使用get方法访问后台
	   // dataType: "json",//返回json格式的数据
    url: "/event_check/monitor_select", //要访问的后台地址
//		data: "client=" + client +"&version=" + version +"&subver=" + subver + "&start=" + start + "&end=" + end,//要发送的数据
		data: "client=" + client +"&version=" + version +"&subver=" + subver + "&start=" + start + "&end=" + end + "&eid_start=" + eid_start + "&eid_end=" + eid_end,//要发送的数据
    success: function(data){
      var json = $.parseJSON(data);

      if (json.length <= 0) {
        $('#charts_container').html('没有数据');
        return;
      }
      $('#charts_container').html('');

      var length = json[0].time.length;
      for(var i = 0; i < json.length; ++i) {
        var e = json[i].events;
        var chart_id = "chart_container_" + e.id;
        var chart_title = e.desc;
        var chart_node = $('#' + chart_id);
        if (chart_node.length) {
          chart_node.html('');
        } else {
          $('#charts_container').append('<div id="' + chart_id + '" style="float:left;border: 0;padding:2px;margin:2px; width:450px; height:300px;"></div>');
        }
        draw_quality_map(chart_title, chart_id, json.time, e.series, e.aggregate_type == "2");
      }
    }
  });
}

function draw_quality_map(chart_title,chart_id, time_list, data_list, show_legend)
{
  var chart = new Highcharts.Chart({
    chart: {
      renderTo: chart_id,
      defaultSeriesType: 'line',
      marginRight: show_legend ? 60 : 2,
      marginBottom: 45
    },
    title: {
      // text: data_list[0].name,
      //text: '标题文本',
      text: chart_title,
      x: -20 //center
    },
    legend : {
      enabled : show_legend,
      layout: 'vertical',
      align: 'right',
      verticalAlign: 'top',
      x: -10,
      y: 100,
      borderWidth: 0
    },
    xAxis:{
      categories: time_list,
      title: {
        text: null
      }
    },
    plotOptions : {
      area: {
      	fillColor: {
      		linearGradient: [0, 0, 0, 300],
      		stops: [
      			[0, Highcharts.getOptions().colors[0]],
      			[1, 'rgba(2,0,0,0)']
      		]
      	},
      	lineWidth: 1,
      	marker: {
      		enabled: false,
      		states: {
      			hover: {
      				enabled: true,
      				radius: 5
      			}
      		}
      	},
      	shadow: false,
      	states: {
      		hover: {
      			lineWidth: 1						
      		}
      	}
      }
    },
    yAxis: {
      title: {
        text: null
      },
      plotLines: [{
        value: 0,
        width: 1,
        color: '#808080'
      }]
    },
    tooltip: {
      formatter: function() {
        return '<b>' + this.y + '(' + this.x + ')' + '</b>';
      }
    },
    series: data_list
  });

}

</script>


</head>
<body>

<div class="top clearfix">
  <div class="logo">
    <a style="font-size:32px;text-decoration:none;" href="#" title="人人IM统计监控系统">
      <img style="margin-top:2px;float:left;" src="../img/rr-logo.png"/><div style="float:left;" >IM统计监控</div>
    </a>
  </div>

  <div class="userinfo">
    <div class="user-line1">
      <span class="name"><a target="_blank" href="http://www.renren.com/BasicInfo.do">穆玉伟, 您好</a></span>
      <span><a onclick="XN.Connect.logout(function(){window.location='http://bolt.jebe.renren.com/bolt/home/logout.htm'});return false;" href="#">退出</a></span>
    </div>
    <div class="user-line2"> yuwei.mu@renren-inc.com </div>
  </div>
</div>

<div class="tabs-widthchild">
    <div class="ad3-tab clearfix">
        <ul class="clearfix">
            <ul class="clearfix">
                <li>
                    <a href="/event/">统计项管理</a>
                </li>
                <li>
                    <a href="/stat/">业务统计数据</a>
                </li>
                <li class="s">
                    <a href="/quality_monitor">客户端服务质量监控</a>
                </li>
                <li>
                    <a href="http://skynet.jebe.d.xiaonei.com/ping/service.s">服务性能监控</a>
                </li>
            </ul>
        </ul>
    </div>
</div>
<div class="container" style="margin:0 auto; width:960px; height:400px; color:#636363;">
<div style="margin-top:4px;border: 1px solid #D3D3D3;">
      <div class="title" style="background-color: #EBEBEB; border-bottom: 1px solid #EBEBEB; padding: 7px;">
        <h3>查看实时监控数据</h3>
      </div>
      <table style="font-size:12px;width:800px;line-height:18px;">
        <tr>
          <td class="fielddesc">客户端</td>
          <td>
            <select name="select_client" id="select_client" style="width:90px" onchange="monitor_update_version_list();">
              <option value="1">人人桌面</option> 
              <option value="2">人人游戏助手</option>
              <option value="2">人人极速相册</option>
              <option value="2">人人热门分享</option>
            </select>
          </td>
          <td id="check_id" style="color:red"></td>
        </tr>
        <tr>
          <td class="fielddesc">大版本号</td>
          <td>
					  <select name="select_version" id="select_version" style="width:90px" onchange="monitor_update_subver_list();"></select>
				  </td>
        </tr>
        <tr>
          <td class="fielddesc">小版本号</td>
          <td>
            <select name="select_subver" id="select_subver" style="width:90px"></select>
          </td>
        </tr>
        <tr>
          <td class="fielddesc">起止时间</td>
          <td>
             <input type="text" class="time_picker" name="start_time" id="start_time" /> - 
             <input type="text" class="time_picker" name="end_time" id="end_time" />
          </td>
        </tr>
        <tr>
          <td colspan="2">
             <input id="drawchart" class="input-button" type="button" value = "生成图表" style="width:90px;margin:4px 3px 4px 50px;"/>
          </td>
        </tr>
      </table>
</div>

<table border="1" id = "show_table">

</table>
<div id="data"></div>
<div id="charts_container" style="margin-top:4px;"></div>
</div>

</body>
<script type="text/javascript">
$(document).ready(
  function(){
    monitor_update_version_list();

    $( "#start_time" ).datetimepicker();
    $( "#start_time" ).datetimepicker("option", "dateFormat", 'yy-mm-dd');
    $( "#end_time" ).datetimepicker();
    $( "#end_time" ).datetimepicker("option", "dateFormat", 'yy-mm-dd');

    $("#start_time").val(GetFormatedTime(new Date().getTime() / 1000 - 1 * 3600));
    $("#end_time").val(GetFormatedTime(new Date().getTime() / 1000 - 0 * 3600));
  }
);

$('#drawchart').click(
  function() {
    monitor_list_select();
    return false;
  }
);
</script>
</html>
