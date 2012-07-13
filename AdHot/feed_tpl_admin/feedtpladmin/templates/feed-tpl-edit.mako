<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>新鲜事模板view编辑</title>
<link href="/css/new-style.css" rel="stylesheet" type="text/css" />
<script src="/jquery.js" type="text/javascript"></script>
<script type="text/javascript" src="/feed-admin.js"></script>
<script type="text/javascript">
var g_stype = ${stype};
var g_version = ${version};
var g_tpl_id = ${tpl_id};
</script>

</head>

<body>
<table width="1000" border="0" class="t1">
  <tbody><tr>
      <th colspan="5">stype <span style="color:red;font-weight:bold;">${stype}</span> 
        verion <span style="color:red;font-weight:bold;">${version}</span> 
        序号 <span style="color:red;font-weight:bold;">${tpl_id}</span> 
        Feed 模板列表编辑. <span style="float:right;"><a href="#nogo" id="add-template">新建模板</a>(类型=<select id="add-template-view"><option></option>
        <option value="0">网站的Home&amp;Profile</option>
        <option value="1">IM</option>
        <option value="2">Wap</option>
        <option value="3">Phone</option>
        <option value="4">实时化</option>
        <option value="5">开放平台</option>
    </select>)</span></th>
  </tr>
</tbody></table>
<div style="margin: 0 15%;">
</div>
</body>

<script type="text/javascript">
$('#add-template').click(
  function() {
    var view = $('#add-template-view').val();
    if(view.length <= 0) {
      alert('请指定模板view类型');
      return;
    }
    var view_desc = $('option[value=' + view + ']', $('#add-template-view')).text();
    var tpl = new TplListItem(view, view_desc, 'empty');
    tpl.AddToList();
  }
);
$(document).ready(
  function() {
    var view = new FeedKeyView(g_stype, g_version, g_tpl_id, -1, 0, true);
    view.AppendToBody();
    view.HideAllPanel();

    $.ajax('/get-tpls?tpl_id=' + g_tpl_id, {
      success : function(text) {
        var tpl_map = eval('(' + text + ')');
        for(var view in tpl_map) {
          var view_desc = $('option[value=' + view + ']', $('#add-template-view')).text();
          var tpl = new TplListItem(view, view_desc, tpl_map[view]);
          tpl.AddToList();
        }
      },
      error : function() {
        alert('加载模板失败, 请刷新页面');
      }
    });
  }
);
</script>
</html>

