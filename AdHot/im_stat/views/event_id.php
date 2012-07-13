<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta name="keywords" content="校内,校内网,IM统计监控平台"/>
<link media="all" type="text/css" rel="stylesheet" href="/css/bolt.css"/>
<link media="all" type="text/css" rel="stylesheet" href="/css/imstat.css"/>
<link media="all" type="text/css" rel="stylesheet" href="/css/jqchart.css"/>
<script type="text/javascript" src="/js/jquery-1.6.2.min.js"></script>
<script type="text/javascript" src="/js/jquery.jqchart.js"></script>
<style type="text/css">
input[type=text] {
  border: 1px solid #BDC7D8;
}
</style>

<script type="text/javascript">
function eventid_check(data) {
    if(data.value  == ''){
      $("#check_id").html("id 不能为空").css('color', 'red');
      return false;
    }
    if(isNaN(data.value) == true){
      $("#check_id").html("id 必须为数字").css('color', 'red');
      return false;
    }
    var start = $("#switch_id").html();
    start = parseInt(start);
    var end = start + 99999;
    var mid = parseInt(data.value);
    if(mid < start || mid > end){
      $("#check_id").html("id 范围有误").css('color', 'red');
      return false;
    }
    $.ajax({
        type: "get",//使用get方法访问后台
       // dataType: "json",//返回json格式的数据
        url: "/event_check/check_id", //要访问的后台地址
        data: "id=" + data.value,//要发送的数据
        success: function(data){//msg为返回的数据，在这里做数据绑定
          if(data=='1'){
            $("#check_id").html("此id可用").css('color', 'green');
          }
          else{
            $("#check_id").html("此id已被占用").css('color', 'red');
          }
        }
     });
}

function idrange_show(data) {
    var start = data.value * 100000;
    var end = start + 99999;
    $("#switch_id").html(start);
    $("#switch_id2").html(" - "+ end); 

}

function GenerateRemoveCallback(ev, node) {
  return function() {
    var event_id = ev;
    var rm_td = node;
    if (!confirm('确定要删除该事件' + event_id + ' 吗？')) {
      return false;
    }
    $.ajax({
        type: "get",
        url: "/event/event_remove/" + event_id,
        success: function(text) {
          if (text == '1') {
            rm_td.html('<span style="color:green;">删除成功</span>');
          } else {
            rm_td.html('<span style="color:red;">删除失败</span>');
          }
        },
        error : function() {
          rm_td.html('<span style="color:red;">删除失败</span>');
        }
     });
    return false;
  };
}

function check_submit(){
  if($("#event_id").attr("value") == ""){
    alert("事件ID不能为空");
    return false;
  }
  if($("#describe").attr("value") == ""){
    alert("事件描述不能为空");
    return false;
  }
  if($("#manager").attr("value") == ""){
    alert("添加人不能为空");
    return false;
  }
  event_id=$("#event_id").attr("value");
  describe=$("#describe").attr("value");
  aggregate_type=$("#aggregate_type").attr("value");
  value_type=$("#value_type").attr("value");
  manager=$("#manager").attr("value");
  $.ajax({
    type: "get",
    url: "/event/eventid_insert",
    data: "event_id=" + event_id +"&describe=" + describe+"&aggregate_type="+aggregate_type+"&value_type="+value_type+"&manager="+manager,
    success: function() {
      return function(text)
      {
        var data = $.parseJSON(text);
        if (data) {
          alert('添加成功');

          var tr = $("<tr><td>" + data.event_id + "</td><td>" + data.describe + "</td><td>" 
              + (data.aggregate_type == "1" ? '<span style="color:#8899cc">按值累加</span>' : '<span style="color:#99cc88">按值计数</span>') + "</td><td>" 
              + (data.value_type == "0" ? "实时统计" : "按天统计") + "</td><td>"+ data.manager 
              + '</td><td><a class="rm_event" href="#">删除</a></td></tr>');
          $('.rm_event', tr).click(GenerateRemoveCallback(data.event_id, $('.rm_event', tr).parent()));
          $("#table1").append(tr);
        } else {
          alert('添加失败');
        }
      };
    }()
  });
}

function filter(){
  $('#table1').html('');
  fliter1 = $("#client_filter").attr("value");
  if(fliter1 == "0"){
    var start = 100000;
    var end  = 499999;
  }
  else{
  var start = fliter1 * 100000;
  var end = start + 99999;
  }
  $.ajax({
    type: "get",
    url: "/event/filter_select",
    data:"start="+ start + "&end=" + end,
    success: function(text) {
      $("#table1").append('<tr><th width="40">事件ID</th><th width="240">事件描述</th><th width="50">计数类型</th>' 
           + '<th width="60">统计频率</th><th width="90">添加人</th><th width="40"></th></tr>');
      var items = $.parseJSON(text);
      for(var i = 0; i < items.length; ++i) {
        var tr = $("<tr><td>"+ items[i]['event_id'] +  "</td><td>" + items[i]['describe'] 
            + "</td><td>" + (items[i]['aggregate_type'] == "1" ? '<span style="color:#223399">按值累加</span>' : '<span style="color:#339922">按值计数</span>') + "</td><td>" 
            + (items[i]['value_type'] == "0" ? '<span style="color:#223399">实时统计</span>' : '<span style="color:#339922">按天统计</span>') 
            + "</td><td>" + items[i]['manager'] + "</td><td><a class=\"rm_event\" href=\"#\">删除</a></td></tr>");
        $('.rm_event', tr).click(GenerateRemoveCallback(items[i]['event_id'], $('.rm_event', tr).parent()));
        $("#table1").append(tr);
      }
    }
  }
  )
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
                <li class="s">
                    <a href="/event/">统计项管理</a>
                </li>
                <li>
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
<div class="container" style="margin:0 auto; width:960px;font-size:14px;color:#636363;">

<style type="text/css">
.title h3 {
  font-size:14px;
}

</style>
<div style="margin-top:4px;border: 1px solid #D3D3D3;">
<div class="title" style="background-color: #EBEBEB; border-bottom: 1px solid #EBEBEB; padding: 7px;">
  <h3>添加统计/监控事件</h3>
</div>
<table style="font-size:12px;width:800px;line-height:18px;">
  <tr>
    <td class="fielddesc"> 业务类型：</td>
    <td>
    <select id="select_list" style='width:110px'  onchange="idrange_show(this);">
      <option value="1">人人桌面</option>
      <option value="2">人人游戏助手</option>
      <option value="3">极速上传</option>
      <option value="4">热门分享</option>
    </select>
    (ID范围 <span id="switch_id" style="color:red">100000</span><span id="switch_id2" style="color:red"> - 199999</span>)
    </td>
  </tr>
  <tr>
    <td class="fielddesc">事件ID：</td>
    <td><input type="text" name="event_id" id="event_id" onkeyup="eventid_check(this);"/> <span id="check_id"></span><br/></td>
  </tr>
  <tr>
    <td class="fielddesc">事件描述：</td>
    <td><input type="text" style="width:400px" name ="describe" id="describe" /></td>
  </tr>
  <tr>
    <td class="fielddesc">添加人：</td>
    <td><input type="text" style="width:200px" name ="manager" id="manager" /></td>
  </tr>
  <tr>
    <td class="fielddesc">数据汇总方式：</td>
    <td><select id="aggregate_type" style="width:90px">  
                  <option value="1">按值累加</option>
                  <option value="2">按值计数</option> 
                </select>
    </td>
  </tr>
  <tr>
    <td class="fielddesc">统计频率：</td>
    <td><select name="value_type" id="value_type" style='width:90px' >
                <option value="0">实时统计</option>
                <option value="1">按天统计</option>
                </select>
    </td>
  </tr>
  <tr>
    <td colspan="2"><input class="input-button" type ="submit" value="添加" style="margin:0 0 6px 60px;width:90px" onmousedown="check_submit();" /> </td>
  </tr>
</table>
</div>

  <div style="margin-top:14px;border: 1px solid #D3D3D3;min-height:420px;margin-bottom:16px;">
    <div class="title" style="background-color: #EBEBEB; border-bottom: 1px solid #EBEBEB; padding: 7px; margin-bottom:4px;">
      <h3>统计/监控事件列表
                  <select id="client_filter" style="margin-top:-2px;width:98px">
                  <option value="0">全部</option>
                   <option value="1">人人桌面</option>
                   <option value="2">人人游戏助手</option>
                   <option value="3">极速上传</option>
                   <option value="4">热门分享</option>
                  </select>
      </h3>
    </div>
    <table class="itemlist" id="table1"></table>
  </div>
</div>
<script type="text/javascript">

$("#client_filter").change(
  function() {
    filter();
  }
);
$(document).ready(
  function() {
    filter();
  }
);
</script>

</body>
</html>
