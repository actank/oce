
function GetFormatedDate(unixtime) {
  var date = new Date(unixtime * 1000);
  var res = date.getFullYear();
  res += "-";

  var m = date.getMonth() + 1;
  if (m < 10) {
    res += "0";
  }
  res += m;
  res += "-";
  var d = date.getDate();
  if (d < 10) {
    res += "0";
  }
  res += d;
  return res;
}

function GetFormatedTime(unixtime) {
  var res = GetFormatedDate(unixtime);
  res += " ";
  
  var date = new Date(unixtime * 1000);

  var h = date.getHours();
  if (h < 10) {
    res += "0";
  }
  res += h;

  res += ":";

  var m = date.getMinutes();
  if (m < 10) {
    res += "0";
  }
  res += m;
  return res;
}

function update_subver_list()
{
  version = $("#select_version").val();
  cid = $("#select_client").val();
  $("#select_subver").html("");
  $.ajax({
    type: "get",
    url: "/event_check/subver_list", 
    data: "id=" + cid + "&version=" + version,
    success: function(data){
      json = eval("("+data+")");
      $("#select_subver").append("<option value='0'>全部</option>");
      for(i = 0 ;i < json.length;i++)
        $("#select_subver").append("<option value='"+ json[i] +"'>" + json[i] + "</option>");
    }, 
    error: function() {
      alert('获取子版本列表失败');
    }
  });
}
function monitor_update_subver_list()
{
   version = $("#select_version").val();
   cid = $("#select_client").val();
   $("#select_subver").html("");
   $.ajax({
      type: "get",
      url: "/event_check/monitor_subver_list", 
      data: "id=" + cid + "&version=" + version,
      success: function(data){
      json = eval("("+data+")");
      $("#select_subver").append("<option value='0'>全部</option>");
      for(i = 0 ;i < json.length;i++)
        $("#select_subver").append("<option value='"+ json[i] +"'>" + json[i] + "</option>");
       },  
      error: function() {
    alert('获取子版本列表失败');
    }   
  }); 
}


function update_version_list()
{
  $("#select_version").html("");
	$.ajax({
		type: "get",
		url: "/event_check/version_list", 
		data: "id=" + $('#select_client').val(),
		success: function(text){
      json = eval("("+text+")");
      $("#select_version").append("<option value='0'>全部</option>");
      for(i = 0 ;i < json.length;i++) {
        $("#select_version").append("<option value='"+ json[i] +"'>" + json[i] + "</option>");
      }
      update_subver_list();
    }, 
    error: function() {
      alert('获取版本列表失败');
    }
  });
}

function monitor_update_version_list()
{
    $("#select_version").html("");
    $.ajax({
    type: "get",
    url: "/event_check/monitor_version_list",        
    data: "id=" + $('#select_client').val(),
    success: function(text){
      json = eval("("+text+")");
      $("#select_version").append("<option value='0'>全部</option>");
    for(i = 0 ;i < json.length;i++) {
      $("#select_version").append("<option value='"+ json[i] +"'>" + json[i] + "</option>");
       }
     monitor_update_subver_list();
    },
    error: function() {
     alert('获取版本列表失败');
      }
   });
}


function update_startver_list()
{
    $("#start_version").html("");
    $.ajax({
     type: "get",
     url: "/event_check/startver_list", 
     data: "id=" + $('#select_update_client').val(),
     success: function(text){
       json = eval("("+text+")");
       $("#start_version").append("<option value='0'>全部</option>");
       for(i = 0 ;i < json.length;i++) {
        $("#start_version").append("<option value='"+ json[i] +"'>" + json[i] + "</option>");
       }   
//    update_subver_list();
    },  
    error: function() {
      alert('获取版本列表失败');
      }   
   }); 
}


function update_endver_list()
{
    $("#end_version").html("");
    $.ajax({
      type: "get",
      url: "/event_check/endver_list",
      data: "id=" + $('#select_update_client').val(),
      success: function(text){
      json = eval("("+text+")");
      $("#end_version").append("<option value='0'>全部</option>");
      for(i = 0 ;i < json.length;i++) {
      $("#end_version").append("<option value='"+ json[i] +"'>" + json[i] + "</option>");
      }
    },
    error: function() {
      alert('获取版本列表失败');
      }
    });
}


