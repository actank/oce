<!DOCTYPE HTML PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html id="designdetector-com" xml:lang="en" xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link href="/css/new-style.css" rel="stylesheet" type="text/css"></link>
<script src="/jquery.js" type="text/javascript"></script>
<title>字段编辑 - 新鲜事配置管理</title>
<script type="text/javascript" src="/feed-admin.js"></script>
<script type="text/javascript">
var g_stype = ${stype};
var g_version = ${version};
var g_from_tpl_id = ${from_tpl_id};
var g_apply_id = ${apply_id};
var g_is_user = ${is_user};
</script>
</head>
<body>
<table width="1000" border="0" class="t1">
  <tbody><tr>
      <th colspan="5">Feed数据字段定义(stype:<span>${stype}</span>, 版本号: ${version}) 
        &nbsp; 状态 <a id="version-status-edit" href="#nogo">修改</a> <select id="version-status" disabled="true"><option value="0">disabled</option><option value="1">test</option><option value="2">read only</option><option value="3">dispatching</option></select>
        | &nbsp;  线上模板序号 <a id="using-version-id-edit" href="#nogo">修改</a> <select id="using-version-id" value="" disabled="true"><option></option></select> | &nbsp;测试模板序号 <a id="test-version-id-edit" href="#nogo">修改</a> <select id="test-version-id" value=""  disabled="true"><option></option></select>&nbsp; <a id="create_seq" href="#nogo">新建模板序号</a></th>
  </tr>
</tbody></table>
<div id="feed-key-container"></div>
<div class="seq-list-container">模板序号列表(点击编辑)：</div>
<br/>
</body>
<script type="text/javascript">
$('#create_seq').click(
  function() {
      $.ajax('/add-version-seq?stype=' + g_stype + '&version=' + g_version, {
          success : function(text) {
            alert('成功新建模板序号 : ' + text);
            var seq_list = $('div.seq-list-container');
            seq_list.append($('<a style="margin-left:10px;" target="_blank" href="/feed-tpl-edit?stype=' + g_stype 
                + '&version=' + g_version + '&tpl_id=' + text + '">模板序号' + text + '</a>'));
          },
          error : function(){
          }
        });
  }
);

if (g_apply_id > 0) {
  $('#create_seq').click(
    function() {
      $.ajax('/seq-apply-handled?apply_id=' + g_apply_id, {
          success : function(text) {
          },
          error : function(){
          }
        });
    }
  );
}

$('#using-version-id-edit').click(
  function() {
    if (!confirm("该操作会影响到线上新鲜事。确定要" + $(this).html() + "吗?")) {
      return;
    }
    if ($(this).html() == '保存') {
      $(this).html('修改');
      $('#using-version-id').attr('disabled', true);
      $.ajax('/update-stype-version-using-id?stype=' + g_stype + '&version=' + g_version + '&tpl_id=' + $('#using-version-id').val(), {
        'success' : function(text){
          alert('保存成功');
        },
        'error' : function() {
          alert('保存失败');
        }
      });
    } else {
      $(this).html('保存');
      $('#using-version-id').attr('disabled', false);
    }
  }
);

$('#version-status-edit').click(
  function() {
    if (!confirm("该操作会影响到线上新鲜事。确定要" + $(this).html() + "吗?")) {
      return;
    }
    if ($(this).html() == '保存') {
      $(this).html('修改');
      $('#version-status').attr('disabled', true);
      $.ajax('/update-stype-version-status?stype=' + g_stype + '&version=' + g_version + '&status=' + $('#version-status').val(), {
        'success' : function(text){
          alert('保存成功');
        },
        'error' : function() {
          alert('保存失败');
        }
      });
    } else {
      $(this).html('保存');
      $('#version-status').attr('disabled', false);
    }
  }
);

$('#test-version-id-edit').click(
  function() {
    if ($(this).html() == '保存') {
      $(this).html('修改');
      $('#test-version-id').attr('disabled', true);
      $.ajax('/update-stype-version-test-id?stype=' + g_stype + '&version=' + g_version + '&tpl_id=' + $('#test-version-id').val(), {
        'success' : function(text){
          alert('保存成功');
        },
        'error' : function() {
          alert('保存失败');
        }
      });
    } else {
      $(this).html('保存');
      $('#test-version-id').attr('disabled', false);
    }
  }
);

$(document).ready(
  function() {
    if(g_stype <= 0) {
      return;
    }
    $.ajax('/get-stype-version-seqs?stype=' + g_stype + '&version=' + g_version, {
      'success' : function(text){
        var seqs = $.parseJSON(text); 
        for(var i = 0; i < seqs.length; ++i) {
          // var view = new FeedKeyView(g_stype, g_version, seqs[i]);
          // view.AppendToBody();
          var seq_list = $('div.seq-list-container');
          if (seq_list.length <= 0) {
            seq_list = $('<div class="seq-list-container"> 模板序号列表(点击编辑模板): </div>');
            $(document.body).append(seq_list);
          }
          seq_list.append($('<a style="margin-left:10px;" target="_blank" href="/feed-tpl-edit?stype=' + g_stype 
              + '&version=' + g_version + '&tpl_id=' + seqs[i] + '">模板序号' + seqs[i] + '</a>'));
          
          $('#test-version-id').append($('<option value=' + seqs[i] + '>' + seqs[i] + '</option>'));
          $('#using-version-id').append($('<option value=' + seqs[i] + '>' + seqs[i] + '</option>'));
        }
        $.ajax('/get-stype-version-info?stype=' + g_stype + '&version=' + g_version, {
           'success' : function(text) {
             var o = $.parseJSON(text);
             $('#using-version-id').val(o.using_id);
             $('#test-version-id').val(o.test_id);
             $('#version-status').val(o.status);

             var view = new FeedKeyView(g_stype, g_version, o.using_id, o.test_id, o.status, false);
             view.AppendToNode($('#feed-key-container'));
             view.HideVersion();
             view.HideStatus();
             view.HideSeqs();
             view.HideToggle();
           },
           'error' : function() {
             alert('加载id出错');
           }
        });
      },
      'error' : function(text){
      }
    });
  }
);
</script>
</html>

