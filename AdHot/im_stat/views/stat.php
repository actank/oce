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
<script type="text/javascript" src="/js/highcharts.js"></script>
<script type="text/javascript" src="/js/imstat.js"></script>

<script type="text/javascript">
json_table  = new Array();

function render_table()
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

	$.ajax({
		type: "get",
    url: "/event_check/total_select",
    // url: "/stat/get_stat_data",
		data: "client=" + client +"&version=" + version +"&subver=" + subver + "&start=" + start + "&end=" + end + "&eid_start=" + eid_start + "&eid_end=" + eid_end,//要发送的数据
		success: function(data) { //msg为返回的数据，在这里做数据绑定
      var json = $.parseJSON(data);
      if (json.length <= 0) {
        $('#show_table').html('没有数据');
        return;
      }

      var length = json[0]['time'].length;
      var header = '<tr><th width="70"></th>';
      /*      for(var i = 0; i < length; ++i) {
        header += '<th>' + json[0]['time'][i] + '</th>';
    }*/
      for(var i = 0; i < json.length; ++i) {
        var e = json[i].events;
         header += '<th>' + e.desc + '</th>';
//        header += '<th>' + json[0]['time'][i] + '</th>';
      } 
      header += '</tr>';
      $('#show_table').html('');
      $('#show_table').append(header);
      for(var i = 0; i < length; ++i) {
        var tr = '<tr><td width="70">' + json[0]['time'][i] + '</td>';
        for(var j = 0; j < json.length; ++j) {
          tr += '<td width="40">';
          var e = json[j].events;
          if (e.aggregate_type == "1") {
            var li = '';
            var total = 0;
            for(var k = 0; k < e.series.length; ++k) {
              li += '<li style="list-style:none;">' + e.series[k].name + ' ' + e.series[k].data[i] + '</li>';
              total += e.series[k].data[i];
            }
            tr += '<ul class="plus">total:' + total + li + '</ul>';
          } else {
            tr += e.series[0].data[i];
          }
          tr += '</td>';
        }
        tr += '</tr>';
        var tr_node = $(tr);
        $('ul', tr_node).click(function(){
            var n = $(this);
            if (n.hasClass('plus')) {
              n.removeClass('plus').addClass('minus');
            } else {
              n.removeClass('minus').addClass('plus');
            }
          }
        );
        $('#show_table').append(tr_node);
      }
    }, 
    error : function() {
      alert('error');
    }
	 });
	 return false;
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
                <li class="s">
                    <a href="/stat/">业务统计数据</a>
                </li>
                <li>
                    <a href="/quality_monitor/">客户端服务质量监控</a>
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
        <h3>查看统计数据报表</h3>
      </div>
      <table style="font-size:12px;width:800px;line-height:18px;">
        <tr>
                <td class="fielddesc">客户端</td>
                <td>
					<select name="select_client" id="select_client" style="width:90px" onchange="update_version_list();">
					   <option value="1">人人桌面</option> 
					   <option value="2">人人游戏助手</option>
					   <option value="2">人人极速相册</option>
					   <option value="2">人人热门分享</option>
				</td>
                <td id='check_id' style="color:red"></td>
        </tr>
        <tr>
                <td class="fielddesc">大版本号</td>
                <td>
					          <select name="select_version" id="select_version" style="width:90px" onchange="update_subver_list();">
					          </select>
				        </td>
        </tr>
        <tr>
                <td class="fielddesc">小版本号</td>
                <td>
                    <select name="select_subver" id="select_subver" style='width:90px'  >
                    </select>
                </td>
        </tr>
        <tr>
                <td class="fielddesc">起止日期</td>
                <td>
                  <input type="text" class="date_picker" name="start_time" id="start_time" /> - 
                  <input type="text" class="date_picker" name="end_time" id="end_time" />
                </td>
        </tr>
        <tr>
          <td colspan="2">
             <input id="rendertable" class="input-button" type="button" value="生成报表" style="width:90px;margin:4px 3px 4px 56px;"/>
             <!--
             <input id="drawchart" class="input-button" type="button" value = "折线图" style="width:90px;margin:4px 3px;"/>
             -->
          </td>
        </tr>
		<tr>

</table>
</div>

  <div style="margin-top:14px;border: 1px solid #D3D3D3;min-height:420px;margin-bottom:16px;">
    <div class="title" style="background-color: #EBEBEB; border-bottom: 1px solid #EBEBEB; padding: 7px; margin-bottom:4px;">
      <h3>统计数据报表</h3>
    </div>
    <table class="itemlist" id="show_table"></table>
  </div>
<div id="data"></div>

</div>

<div id="chart_container" style="display:none;border:1px solid red;margin:0 auto; width:960px; height:400px; color:#636363;"></div>

</body>
<script type="text/javascript">
$(document).ready(
  function(){
    update_version_list();

    $( "#start_time" ).datepicker();
    $( "#start_time" ).datepicker("option", "dateFormat", 'yy-mm-dd');
    $( "#end_time" ).datepicker();
    $( "#end_time" ).datepicker("option", "dateFormat", 'yy-mm-dd');

    $("#start_time").val(GetFormatedDate(new Date().getTime() / 1000 - 7 * 3600 * 24));
    $("#end_time").val(GetFormatedDate(new Date().getTime() / 1000 - 1 * 3600 * 24));
  }
);

$('#rendertable').click(
  function() {
    $('#show_table').html('<tr><td>正在生成报表....</td></tr>');
    render_table();
    return false;
  }
);
</script>
</html>
