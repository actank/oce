var F_PUSH_NEWS = 0x01;
var F_PUSH_MINI = 0x02;
var F_PUSH_IM = 0x04;
var F_PUSH_CLASS = 0x08;
var F_PUSH_MINI_GROUP = 0x10;

function GetStypeIdsHtml(default_value) {
  $.ajax('/get-stype-ids', {
    async : false,
    success : function(text){
      ids = $.parseJSON(text);
    },
    error : function(){
      alert('/get-stype-ids 出错');
    }
  });

  var html = '<option value="' + default_value + '">' + default_value + '</option>';
  for(var i = 0; i < ids.length; ++i) {
    html += '<option value="' + ids[i] + '">' + ids[i] + '</option>';
  }
  return html;
}

var FeedConfigView = function(stype) {
  this.stype_ = isNaN(stype) ? -1 : stype;
  this.dom_ = $('<table width="1000" border="0" class="t1">' +
          '<tr>' +
            '<th colspan="4">Feed配置信息 &nbsp; <a id="save-config" style="float:right;" href="#nogo">保存</a></th>' +
          '</tr>' +
          '<tr>' +
            '<td style="width:110px;">类型（TYPE）：</td>' +
            '<td><input id="type_id" size="4" disabled maxlength="3"/></td>' +
            '<td>小类型（STYPE）：</td>' +
            '<td><input id="stype_id" size="6" maxlength="5"/></td>' +
          '</tr>' +
          '<tr>' +
            '<td>业务类型描述：</td>' +
            '<td colspan="3"><input size="120" type="text" id="stype-desc"/>' +
          '</tr>' +
          '<tr>' +
            '<td>权重（Weight）：</td>' +
            '<td><select id="stype_weight">' +
                '<option>0</option>' +
                '<option>1</option>' +
                '<option>2</option>' +
                '<option>3</option>' +
                '<option>4</option>' +
                '<option>5</option>' +
                '<option>6</option>' +
              '</select>' +
            '</td>' +
            '<td>父小类型（PType）：</td>' +
            '<td><select id="ptype_id" style="width:70px;">' +
              '<option>0</option>' +
            '</select></td>' +
          '</tr>' +
          '<tr>' +
            '<td>持久化本体：</td>' +
            '<td>save:' +
            '<input name="SaveContent" type="radio" value="1" />&nbsp;&nbsp; not save:<input name="SaveContent" type="radio" value="0" checked="checked" /></td>' +
            '<td>持久化FeedDB：</td>' +
            '<td>save:' +
            '<input name="SaveFeedDB" type="radio" value="1" />&nbsp;&nbsp; not save:<input name="SaveFeedDB" type="radio" value="0" checked="checked" /></td>' +
          '</tr>' +
          '<tr>' +
            '<td>NewsFeed合并策略：</td>' +
            '<td>' +
               'append: <input name="NewsMergeType" type="radio" value="2" />' +
               '&nbsp; replace: <input name="NewsMergeType" type="radio" value="1" checked="checked" /></td>' +
            '<td>MiniFeed合并策略：</td>' +
            '<td>append:<input name="MiniMergeType" type="radio" value="2" />' +
              '&nbsp; replace: <input name="MiniMergeType" type="radio" value="1" checked="checked" />' +
            '</td>' +
          '</tr>' +
          '<tr>' +
            '<td>推送策略：</td>' +
            '<td colspan="3">NewsFeed:<input type="checkbox" id="PushPolicy_news"/>&nbsp;&nbsp; MiniFeed:<input id="PushPolicy_mini" type="checkbox"/>&nbsp;&nbsp; IM:<input id="PushPolicy_IM" type="checkbox"/>' +
              '&nbsp;&nbsp; Class:' +
              '<input id="PushPolicy_class" type="checkbox"/>&nbsp;&nbsp; MiniGroup:<input id="PushPolicy_mini_group" type="checkbox"/></td>' +
          '</tr>' +
          '<tr>' +
            '<td>分发表达式自定义：</td>' +
            '<td>自定义: <input name="CustomExpr" type="radio" value="1"/>' +
              '&nbsp;&nbsp;配置: <input name="CustomExpr" type="radio" value="0" checked="checked" /></td>' +
            '<td>合并后是否更新时间：</td>' +
            '<td>update:' +
              '<input name="UpdateTimeOnMerge" type="radio" value="1" />' +
              '&nbsp;&nbsp; not update:' +
            '<input name="UpdateTimeOnMerge" type="radio" value="0" checked="checked" /></td>' +
          '</tr>' +
          '<tr>' +
            '<td>保存时长：</td>' +
            '<td><input name="Lifetime" value="1" size="5" maxlength="5"/> 天（-1表示永久保存）</td>' +
            '<td>每天发送配额：</td>' +
            '<td><input name="DailyQuota" value="1000" size="9" maxlength="9"/> 条（-1表示不限制）</td>' +
          '</tr>' +
        '</table>');
  var T = this;
  $('#save-config', this.dom_).click(
    function() {
      T.Save();
    }
  );
  $('input#stype_id', this.dom_).change(
    function() {
      var stype_id = parseInt($(this).val());
      if (isNaN(stype_id) || stype_id <= 100) {
        alert('stype值必须是大于100的整数值');
        return;
      }
      console.log($(this).val());
      $('input#type_id', T.dom_).val(stype_id - stype_id % 100);
    }
  );
}

FeedConfigView.prototype = {
  "AppendToNode" : function(node) {
    node.append(this.dom_);
  },

  "Load" : function() {
    if(this.stype_ <= 0) {
      return;
    }
    $('input#type_id', this.dom_).val(this.stype_ - this.stype_ % 100).attr('disabled', true);
    $('input#stype_id', this.dom_).val(this.stype_).attr('disabled', true);
    var T = this;
    $.ajax('/get-feed-config?stype=' + this.stype_, {
      'success' : function(text){
         var o = $.parseJSON(text);
         $('#stype-desc', T.dom_).val(o.desc);
         $('#stype_weight', T.dom_).val(o.weight);
         $('#ptype_id', T.dom_).html(GetStypeIdsHtml('0')).val(o.ptype);

         $("input[name='SaveContent'][value='" + o.save_content + "']", T.dom_).attr("checked", true);
         $("input[name='SaveFeedDB'][value='" + o.save_feed_db + "']", T.dom_).attr("checked", true);
         $("input[name='NewsMergeType'][value='" + o.news_merge_type + "']", T.dom_).attr("checked", true);
         $("input[name='MiniMergeType'][value='" + o.mini_merge_type + "']", T.dom_).attr("checked", true);

         $("input:checkbox#PushPolicy_news", T.dom_).attr("checked", o.push_flags & F_PUSH_NEWS);
         $("input:checkbox#PushPolicy_mini", T.dom_).attr("checked", o.push_flags & F_PUSH_MINI);
         $("input:checkbox#PushPolicy_IM", T.dom_).attr("checked", o.push_flags & F_PUSH_IM);
         $("input:checkbox#PushPolicy_class", T.dom_).attr("checked", o.push_flags & F_PUSH_CLASS);
         $("input:checkbox#PushPolicy_mini_group", T.dom_).attr("checked", o.push_flags & F_PUSH_MINI_GROUP);

         $("input[name='CustomExpr']:checked", T.dom_).val(o.custom_expr);
         $("input[name='UpdateTimeOnMerge']:checked", T.dom_).val(o.update_time_on_merge);
         $("input[name='Lifetime']", T.dom_).val(o.lifetime);
         $("input[name='DailyQuota']", T.dom_).val(o.daily_quota);

         $("#MiniMergeBy", T.dom_).val(o.mini_merge_by);
         $("#NewsMergeBy", T.dom_).val(o.news_merge_by);
         $("#SourceBy", T.dom_).val(o.source_by);
         $("#PSourceBy", T.dom_).val(o.psource_by);
         $("#ActorBy", T.dom_).val(o.actor_by);
         $("#DispatchExpr", T.dom_).val(o.dispatch_expr);
      },
      'error' : function(){
        alert('加载新鲜事类型 ' + T.stype_ + ' 配置信息失败');
        $('#save-config', T.dom_).unbind('click').click(
          function() {
            alert('加载失败，禁止保存');
          }
        );
      }
    });
  }, 
  Save : function(jump_to_edit, apply_id) {
    var url='/save-feed-config?';
    var type_id = parseInt($('input#type_id', this.dom_).val());
    if (isNaN(type_id) || type_id <= 0) {
      alert('请指定正确的type值');
      return;
    }
    url += 'type=' + type_id;
    var stype_id = parseInt($('input#stype_id', this.dom_).val());
    if (isNaN(stype_id) || stype_id <= 0) {
      alert('请指定正确的stype值');
      return;
    }
    url += '&stype=' + stype_id;
    url += '&is_new=' + (this.stype_ != stype_id ? 1 : 0);
    if ((stype_id - stype_id % 100) != type_id) {
      alert('type ' + type_id + '和stype ' + stype_id + ' 值不匹配');
      return;
    }

    url += '&title=' + $('#stype-desc', this.dom_).val();
    url += '&weight=' + $('#stype_weight', this.dom_).val();
    url += '&ptype=' + $('#ptype_id', this.dom_).val();
    url += '&save_content=' + $("input[name='SaveContent']:checked", this.dom_).val();
    url += '&save_feed_db=' + $("input[name='SaveFeedDB']:checked", this.dom_).val();
    url += '&news_merge_type=' + $("input[name='NewsMergeType']:checked", this.dom_).val();
    url += '&mini_merge_type=' + $("input[name='MiniMergeType']:checked", this.dom_).val();

    var push_flags = 0;
    if ($("input:checkbox#PushPolicy_news", this.dom_).attr("checked")) {
      push_flags |= 0x01;
    }
    if ($("input:checkbox#PushPolicy_mini", this.dom_).attr("checked")) {
      push_flags |= 0x02;
    }
    if ($("input:checkbox#PushPolicy_IM", this.dom_).attr("checked")) {
      push_flags |= 0x04;
    }
    if ($("input:checkbox#PushPolicy_class", this.dom_).attr("checked")) {
      push_flags |= 0x08;
    }
    if ($("input:checkbox#PushPolicy_mini_group", this.dom_).attr("checked")) {
      push_flags |= 0x10;
    }
    url += '&push_flags=' + push_flags;
    url += '&custom_expr=' + $("input[name='CustomExpr']:checked", this.dom_).val();
    url += '&update_time_on_merge=' + $("input[name='UpdateTimeOnMerge']:checked", this.dom_).val();
    url += '&lifetime=' + $("input[name='Lifetime']", this.dom_).val();
    url += '&daily_quota=' + $("input[name='DailyQuota']", this.dom_).val();
    if (apply_id > 0) {
      url += '&apply_id=' + apply_id;
    }
    $.ajax(url, {
      'success' : function(text){
        var o = $.parseJSON(text);
        if (o.status == 0) {
          alert('保存成功');
          if (jump_to_edit) {
            window.location = '/feed-config-edit?stype=' + stype_id;
          }
        } else {
          alert(o.desc);
        }
      },
      'error' : function(){
        alert('保存失败');
      }
    });
  },
  HideSave : function() {
    $('#save-config', this.dom_).css('display', 'none');
  },
  "Disable" : function() {
    $('input, select', this.dom_).each(
      function() {
        $(this).attr('disabled', 'true');
      }
    );
    this.HideSave();
  }
}

var FeedKeyView = function(stype, version, using_seq_id, test_seq_id, status, readonly) {
  this.stype_ = stype;
  this.top_key_list_ = []
  this.version_ = version;
  this.using_seq_id_ = using_seq_id;
  // alert(using_seq_id + ' - ' +  test_seq_id + ' - ' + status);
  this.dom_ = $('<div><div id="panel" style="margin-left:100px; width:998px; line-height:24px; border:1px solid #CAD9EA;border-top:0;">'
                 + '<span id="toggle-span" class="toggle-span folded"></span> <span id="version-span">版本:<span style="color:red;">' + version + '</span></span>'
                 + ' <span id="status-span">&nbsp; 状态: <select id="version-status" disabled="true"><option value="0">disabled</option>'
                 +    '<option value="1">test</option><option value="2">read only</option><option value="3">dispatching</option>' 
                 + '</select></span>' 
                 + ' <span id="seqs-span">&nbsp; 线上模板序号:<span id="using-seq-id" style="color:red;">' + using_seq_id + '</span>' 
                 + ' &nbsp; 测试模板序号:<span id="test-seq-id" style="color:red;">' + test_seq_id + '</span></span>' 
                 
                 + ' <span class="edit-keys-panel">&nbsp; <a href="#nogo" id="validate-keys">检查合法性</a> &nbsp; <a href="#nogo" id="save-keys">保存</a></span>'

                 + ' &nbsp; <a style="display:none;" href="/feed-keys-edit?stype=' + stype + '&version='+ version + '" id="all-seqs" target="_blank">全部模板序号列表</a>'
                 + ' &nbsp; <a id="generate-java-code" href="#nogo">生成java代码</a>'
               + '</div>' +
      '<div><table width="1000" id="mapping-table" border="0" class="t1">' +
          '<tbody><tr>' +
              '<th colspan="3">Feed必填字段设置</th>' +
          '</tr>' +
          '<tr>' +
            '<th>KEY</th>' +
            '<th>VALUE</th>' +
            '<th>说明</th>' +
          '</tr>' +
          '<tr>' +
            '<td>MiniMergeBy</td>' +
            '<td><select id="MiniMergeBy" class="key-mapping">' +
              '<option value=""></option>' +
            '</select></td>' +
            '<td>MiniFeed中将按此设置进行去重合并操作</td>' +
          '</tr>' +
          '<tr>' +
            '<td>NewsMergeBy</td>' +
            '<td><select id="NewsMergeBy" class="key-mapping">' +
              '<option value=""></option>' +
            '</select></td>' +
            '<td>NewsFeed中将按此设置进行去重合并操作</td>' +
          '</tr>' +
          '<tr>' +
            '<td>SourceBy</td>' +
            '<td><select id="SourceBy" class="key-mapping">' +
              '<option value=""></option>' +
            '</select></td>' +
            '<td>产生Feed的业务的流水号，如发日志新鲜事时Source为日志表中的自增ID，即Blogid</td>' +
          '</tr>' +
          '<tr>' +
            '<td>PSourceBy</td>' +
            '<td><select id="PSourceBy" class="key-mapping">' +
              '<option value=""></option>' +
            '</select></td>' +
            '<td>用于分享，照片上转等具有父子关系的新鲜事发送，如分享日志产生的Feed，PSource应该是日志业务的流水号; 上转照片产生的Feed的PSource为所在像册的ID，主要用于批量删除</td>' +
          '</tr>' +
          '<tr>' +
            '<td>ActorBy</td>' +
            '<td><select id="ActorBy" class="key-mapping">' +
              '<option value=""></option>' +
            '</select></td>' +
            '<td>触发Feed的人的UID</td>' +
          '</tr>' +
          '<tr>' +
            '<td>分发表达式</td>' +
            '<td><input id="DispatchExpr" value="" size="21"/></td>' +
            '<td>由分发表达式计算出新鲜事分发给哪些用户(仅当使用"配置"分发表达式时有效)</td>' +
          '</tr>' +
        '</tbody></table></div>' 
             + '<table id="key-list-header" width="1000" border="0" class="t1">'
               + '<tr>'
                 + '<th width="200px">KEY <input type="text" id="add-key" title="新建顶层key" class="add-child"> </th>'
                 + '<th width="260px">描述</th>'
                 + '<th width="120px">数据类型</th>'
                 + '<th width="120px">节点循环</th>'
                 + '<th>操作</th>'
               + '</tr>'
             + '</table>'
             + '<div class="key-list-container">'
               + '<ol id="key-list" style="list-style:none;padding:0;margin:0;"></ol>'
             + '</div></div>');

  var T = this;

  $('#version-status', this.dom_).val(status);
  $('#add-key', this.dom_).click(
    function() {
      T.KeyListAdd('', '', '');
    }
  );

  $('#save-keys', this.dom_).click(
    function() {
      if(!T.ValidateKeys()) {
        alert('数据格式不合法');
        return;
      }
      var url='/save-version-keys?'
        + 'stype=' + T.stype_
        + '&version=' + T.version_;

      url += '&mini_merge_by=' + $('#MiniMergeBy', T.dom_).val();
      url += '&news_merge_by=' + $('#NewsMergeBy', T.dom_).val();
      url += '&source_by=' + $('#SourceBy', T.dom_).val();
      url += '&psource_by=' + $('#PSourceBy', T.dom_).val();
      url += '&actor_by=' + $('#ActorBy', T.dom_).val();
      url += '&dispatch_expr=' + $('#DispatchExpr', T.dom_).val();

      var keys_xml = T.GetKeysXml();
      if(!T.ValidateXmlLoop(keys_xml)) {
        return;
      }
      alert(keys_xml);

      $.ajax(url, {
        'data' : 'kl=' + encodeURIComponent(T.GetKeysXml()),
        'type' : 'POST',
        'success' : function(text){
          alert(text);
        },
        'error' : function(){
          alert('保存出错');
        }
      });
    }
  );

  $('#validate-keys', this.dom_).click(
    function() {
      if(T.ValidateKeys()) {
        alert('Feed 数据验证通过');
      }
    }
  );
  
  $('#generate-java-code', this.dom_).click(
    function() {
      T.GenerateJavaCode();
    }
  );

  var T = this;
  $('#toggle-span', this.dom_).click(
    function() {
      if ($(this).hasClass('folded')) {
        T.ShowMapping(true);
        T.ShowKeys(true);
        $(this).removeClass('folded').addClass('expanded')
      } else {
        T.ShowMapping(false);
        T.ShowKeys(false);
        $(this).removeClass('expanded').addClass('folded')
      }
    }
  );

  $.ajax({
    'url': '/get-stype-version-info?stype=' + stype + '&version=' + version,
    'success': function(T) {
      var This = T;
      return function(text) {
        var o = $.parseJSON(text);
        This.XmlKeyListAdd($('<f>' + o.keys_xml + '</f>'));
        This.FillTopKeys(o);
        if (readonly) {
          This.Disable(); 
        }
      };
    }(this)
  });
}

FeedKeyView.prototype.GenerateJavaCode = function() {
  var keys_xml = this.GetKeysXml();
  if(!this.ValidateXmlLoop(keys_xml)) {
    return;
  }
  var parser = new SchemaParser();
  parser.ParseSchema(this.stype_, this.version_, keys_xml);
  var hdf = parser.GetHdfMap();

  $.ajax('/generate-code?lang=java&stype=' + this.stype_ + '&version=' + this.version_, {
    type: 'POST',
    data: 'hdf=' + encodeURIComponent(encodeURIComponent(hdf)), 
    context: document.body,
    success: function(text) {
      alert('生成代码成功，点确定后下载代码文件');
      window.open(text);
    },
    error: function(){
      alert('生成代码失败');
    }
  });
}

FeedKeyView.prototype.ValidateXmlLoop = function(keys_xml) {
  if (!keys_xml || keys_xml.length <= 0) {
    return false;
  }
  function HasLoopAncestor(node) {
    var p = node;
    console.log('parent = ' + p.parent().parent().length);
    // return false;
    p = p.parent()
    while(p.length > 0) {
      console.log('parent = ' + p);
      if (p.attr('is_loop') == "1") {
        return true;
      }
      p = p.parent();
    }
    return false;
  }

  function CheckChildren(dom) {
    var childs = dom.children();
    for(var i = 0; i < childs.length; ++i) {
      var n = $(childs[i]);
      if (n.attr('is_loop') == "1") {
        if (HasLoopAncestor(n)) {
          alert('循环节点 ' + n.attr('name') + ' 不能有可循环的上层节点');
          return false;
        }
      }
      if (n.children().length > 0) {
        if(!CheckChildren(n)) {
          return false;
        }
      }
    }

    return true;
  }

  return CheckChildren($('<f>' + keys_xml + '</f>'));
}

FeedKeyView.prototype.ValidateKeys = function() {
  var top_keys = this.GetTopKeys();
  function FindKey(key) {
    for(var i = 0; i < top_keys.length; ++i) {
      if (key == top_keys[i]) {
        return true;
      }
    }
    return false;
  }

  var nodes = $('select', $('#mapping-table', this.dom));

  for(var i = 0; i < nodes.length; ++i) {
    if (!FindKey($(nodes[i]).val())) {
      return false;
    }
  }
  return true;
}

FeedKeyView.prototype.ShowMapping = function(b) {
  $('#mapping-table', this.dom_).css('display', b ? '' : 'none');
}

FeedKeyView.prototype.ShowKeys = function(b) {
  $('#key-list-header', this.dom_).css('display', b ? '' : 'none');
  $('div.key-list-container', this.dom_).css('display', b ? '' : 'none');
}

FeedKeyView.prototype.UpdateTopKeys = function() {
  var keys = this.GetTopKeys();
  var html = '<option value=""></option>';
  for(var i = 0; i < keys.length; ++ i) {
    html += '<option value="' + keys[i] + '">' + keys[i] + '</option>';
  }
  $('select', $('#mapping-table', this.dom)).each(
    function() {
      var val = $(this).val();
      $(this).html(html).val(val);
    }
  );
}

FeedKeyView.prototype.FillTopKeys = function(ver_info) {
  var keys = this.GetTopKeys();
  for(var i = 0; i < keys.length; ++ i) {
    $('select.key-mapping', this.dom_).each(
        function() {
          $(this).append('<option value="' + keys[i] + '">' + keys[i] + '</option>');
        }
      );
  }
  var o = ver_info;
  $('#MiniMergeBy', this.dom_).val(o.mini_merge_by);
  $('#NewsMergeBy', this.dom_).val(o.news_merge_by);
  $('#SourceBy', this.dom_).val(o.source_by);
  $('#PSourceBy', this.dom_).val(o.psource_by);
  $('#ActorBy', this.dom_).val(o.actor_by);
  $('#DispatchExpr', this.dom_).val(o.DispatchExpr);
}

FeedKeyView.prototype.XmlKeyListAdd = function(xml_dom) {
  var child = xml_dom.children('key');
  var T = this;
  child.each(
    function() {
      var t = $(this);
      var root = T.KeyListAdd(t.attr('name'), t.attr('comment'), t.attr('type'), null, t.attr('is_loop'));

      var kids = t.children('key');
      if (kids && kids.length) {
        $('select[name=DataType]', root).append($('<option value="node">node</option>')).val('node').attr('disabled',true);
        T.XmlKeyListAddChild(root, kids);
      } else {
        if (t.attr('type') == 'number' && t.attr('is_loop') != '1') {
          T.top_key_list_[T.top_key_list_.length] = t.attr('name');
        }
      }
    }
  );
  this.top_key_ready_ = true;
}

FeedKeyView.prototype.XmlKeyListAddChild = function(root, kids) {
  // var child = xml_dom.children(':first-child').children('key');
  var T = this;
  kids.each(
    function() {
      var t = $(this);
      var li = T.KeyListAddChild(root, t.attr('name'), t.attr('comment'), 
          t.attr('type'), null, t.attr('is_loop'));

      var kids = t.children('key');
      if (kids && kids.length) {
        $('select[name=DataType]', li).append($('<option value="node">node</option>')).val('node').attr('disabled',true);
        T.XmlKeyListAddChild(li, kids);
      }
    }
  );
}

FeedKeyView.prototype.KeyListAddChild = function(root, key, note, data_type, kids, is_loop) {
  if (root.hasClass('multi')) {
    root.removeClass('plus');
  } else {
    root.addClass('multi');
    // $('span.node-loop', root).css('visibility', 'visible');
    root.click( function() {
        var t = $(this);
        if (t.hasClass('plus')) {
          t.removeClass('plus');
          t.addClass('minus');
          $('ol', t).show();
        } else {
          t.removeClass('minus');
          $('ol', t).hide();
          t.addClass('plus');
        }
      }
    );
  }
  root.addClass('minus');
  var ol = $($('ol', root).get(0));
  if (ol.length <= 0) {
    ol = $('<ol style="margin:3px 0 3px 1px; padding:0;list-style:none;"></ol>');
    root.append(ol);
  }
  var li = $('<li><input type="text" class="key" value="' + key + '"/><div style="float:right; width:759px;"><input type="text" class="note"/>' +
      '<select name="DataType"><option>string</option><option>number</option></select>' + 
      '<span class="node-loop"><input type="checkbox"/></span><input type="text" class="add-child"/><input type="text" class="remove-child"/></div></li>');
  $('input.note', li).val(decodeURIComponent(note));
  $('select[name=DataType]', li).val(data_type);
  $('input:checkbox', li).attr('checked', is_loop);

  li.children().click(
    function(e) {
      e.cancelBubble = true;
      e.stopPropagation && e.stopPropagation();
    }
  );

  li.click(
    function(e) {
      e.cancelBubble = true;
      e.stopPropagation && e.stopPropagation();
    }
  );
  var T = this;
  $('input.add-child', li).click(
    function(e) {
      T.KeyListAddChild(li, '', '', '');
    }
  );
  $('input.remove-child', li).click(
    function(e) {
      if(!confirm('确定删除该key吗?')) {
        return;
      }
      $(this).parent().parent().remove();
    }
  );

  if (kids && kids.length) {
    for(var i = 0; i < kids.length; ++i) {
      this.KeyListAddChild(li, kids[i].key, kids[i].note, kids[i].data_type, kids[i].kids);
    }
  }
  ol.append(li);
  ol.show();
  return li;
}
FeedKeyView.prototype.KeyListAdd = function(key, note, data_type, kids, is_loop) {
  var n = $('<li class="feed-key"><input type="text" class="key" value="' + key + '"/><div style="float:right; width:759px;"><input type="text" class="note"/>'
        + '<select name="DataType"><option>string</option><option>number</option></select>'
        + '<span class="node-loop"><input type="checkbox"/></span><input type="text" class="add-child"/>'
        + '<input type="text" class="remove-child" href="#nogo"/></div></li>');

  $('input.note', n).val(decodeURIComponent(note));
  $('select[name=DataType]', n).val(data_type);
  $('input:checkbox', n).attr('checked', is_loop);
  var T = this;
  $('input.add-child', n).click(
    function(e) {
      T.KeyListAddChild(n, '', '', '') ;
    }
  );
  $('input.remove-child', n).click(
    function(e) {
      if(!confirm('确定删除该key吗?')) {
        return;
      }
      $(this).parent().parent().remove();
    }
  );
  n.children().click(
    function(e) {
      e.cancelBubble = true;
      e.stopPropagation && e.stopPropagation();
    }
  );

  $('select[name=DataType]', n).change(
    function() {
      T.UpdateTopKeys();
    }
  );
  $('span.node-loop > input:checkbox', n).change(
    function() {
      // alert('');
      return false;
    }
  );

  if (kids && kids.length) {
    for(var i = 0; i < kids.length; ++i) {
      this.KeyListAddChild(n, kids[i].key, kids[i].note, kids[i].data_type, kids[i].kids);
    }
  }
  $('#key-list', this.dom_).append(n);
  return n;
}

FeedKeyView.prototype.AppendToBody = function() {
  $(document.body).append(this.dom_);
}
FeedKeyView.prototype.AppendToNode = function(node) {
  node.append(this.dom_);
}

FeedKeyView.prototype.Disable = function() {
  $('span.edit-keys-panel',this.dom_).css('display', 'none');
  $('#add-key', this.dom_).css('display', 'none');
  $('input, select', this.dom_).each(
     function() {
       $(this).attr('disabled', true);
     }
  );
  $('#key-list input.add-child, #key-list input.remove-child', this.dom_).each(
     function() {
       $(this).css('display', 'none');
     }
  );

  $('#all-seqs', this.dom_).css('display', 'inline');
}

FeedKeyView.prototype.HideToggle= function() {
  $('#toggle-span', this.dom_).css('display', 'none');
}

FeedKeyView.prototype.HideVersion = function() {
  $('#version-span', this.dom_).css('display', 'none');
}
FeedKeyView.prototype.HideStatus = function() {
  $('#status-span', this.dom_).css('display', 'none');
}
FeedKeyView.prototype.HideSeqs = function() {
  $('#seqs-span', this.dom_).css('display', 'none');
}
FeedKeyView.prototype.HideAllPanel = function() {
  $('#panel', this.dom_).css('display', 'none');
}

// 返回顶层的数据，且数据类型必须是number
FeedKeyView.prototype.GetTopKeys = function(root) {
  var top_keys = [];
  $('#key-list > li', this.dom_).each(
    function() {
      if ($('> div > select', $(this)).val() == 'number'
        && !$('> div input:checkbox', $(this)).attr('checked')) {
        console.log($('> input.key', $(this)).val());
        top_keys[top_keys.length] = $('> input.key', $(this)).val();
      }
    }
  );
  return top_keys;
}

FeedKeyView.prototype.GetKeysXml = function(root) {
  if(!root) {
    root = $("#key-list", this.dom_);
  }
  var T = this;
  var nodes = $("li", root);
  var i = 0;
  var xml = '';
  
  var keys = $('input.key', root);
  for(var i = 0; i < keys.length; ++i) {
    var key = $(keys[i]).val();
    if (!key || key.length < 0) {
      alert('字段名不能为空');
      return '';
    }
    if (!/^[a-zA-Z][a-zA-Z0-9]*$/.test(key)) {
      alert('字段' + key + '命名不合法, 字段名必须由字母或数字组成, 且以字母开头');
      return '';
    }
  }
      
  var notes = $('input.note', root);
  for(var i = 0; i < notes.length; ++i) {
    var note = $(notes[i]).val();
    if (!note || note.length < 0) {
      alert('字段' + key + ' 描述不能为空');
      return '';
    }
  }

  nodes.each(
    function() {
      var n = $(this);
      if (!(n.parent().get(0) === root.get(0))) {
        return;
      }

      var key = $('input.key', n).val();
      var note = $('input.note', n).val();
      if ($('> div input:checkbox', n).attr('checked')) {
        xml += '<key is_loop="1"';
      } else {
        xml += '<key';
      }
      xml += ' name="' + key + '"';
      xml += ' comment="' + encodeURIComponent(note) + '"';
      xml += ' type="' + $('select[name=DataType]', n).val() + '">';

      if (n.hasClass('multi')) {
        var kids = T.GetKeysXml($('ol', n));
        if (kids.length > 0) {
          xml += kids;
        }
      }
      xml += '</key>';
      ++i;
    }
  );
  return xml;
}

var TplListItem = function(view, view_desc, tpl_content) {
  // var view_desc = $('#add-template-view option[value="' + view + '"]').text();
  this.view_ = view;
  this.dom_ = $('<tr height="2"><td><span id="toggle-span" class="toggle-span folded"></span> View类型:<span class="view-desc">' + view_desc + '</span> </td><td> <a href="#nogo" id="validate-template">检查ClearSilver语法</a> <a href="#nogo" id="save-template">保存</a> <a href="#nogo" id="remove-template">删除</a> </td></tr>');
  this.tpl_ = $('<tr class="expanded-row" style="display:none;"><td colspan="2"><textarea width="80%" rows="20">' + decodeURIComponent(tpl_content) + '</textarea><ul style="margin-left:80%;padding-left:2px;"><li>字段校验信息</li></ul></td></tr>');

  var T = this;
  $('#toggle-span', this.dom_).click(
    function() {
      if ($(this).hasClass('folded')) {
        T.tpl_.css('display', '');
        $(this).removeClass('folded').addClass('expanded')
      } else {
        T.tpl_.css('display', 'none');
        $(this).removeClass('expanded').addClass('folded')
      }
    }
  );
  $('#validate-template', this.dom_).click(
    function() {
      var view_type = view;
      var textarea = $('textarea', this.tpl_);
      return function() {
        var stype_id = $('#edit-tpl span.stype_id').text();
        var version = $('#edit-tpl span.stype_version').text();
        var tpl = textarea.val();
        var url = '/validate-tpl?stype=' + stype_id + '&version=' + version;
        $.ajax(url, {
          type: 'POST',
          data: 'tpl=' + encodeURIComponent(encodeURIComponent(tpl)), 
          context: document.body,
          success: function(msg){
            alert(decodeURIComponent(msg));
          },
          error: function(){
            alert('验证出错');
          }
        });
      };
    }()
  );
  $('#save-template', this.dom_).click(
    function() {
      var view_type = view;
      var textarea = $('textarea', T.tpl_);
      return function() {
        var stype_id = $('#edit-tpl span.stype_id').text();
        var version = $('#edit-tpl span.stype_version').text();
        var tpl = textarea.val();
        var url = '/save-tpl?tpl_id=' + g_tpl_id
            + '&view=' + view_type;
        alert(textarea.val());
        $.ajax(url, {
          type: 'POST',
          data: 'tpl=' + encodeURIComponent(encodeURIComponent(tpl)), 
          context: document.body,
          success: function(msg){
            alert(msg);
          },
          error: function(){
            alert('保存出错');
          }
        });
      };
    }()
  );

  $('#remove-template', this.dom_).click(
    function() {
      var view_type = view;
      return function() {
        if (!confirm('确定删除该模板吗?')) {
          return;
        }
        var stype_id = $('#edit-tpl span.stype_id').text();
        var version = $('#edit-tpl span.stype_version').text();
        var url = '/remove-tpl?tpl_id=' + g_tpl_id + '&view=' + view_type;
        $.ajax(url, {
          context: $(this).parent().parent(),
          success: function(msg){
            alert(msg);
            $(this).remove();
            var option = $('<option value="' + view_type + '">' + $('span.view-desc', $(this)).text() + '</option>');
            $("#add-template-view").append(option);
          },
          error: function(){
            alert('删除模板失败');
          }
        });
      };
    }()
  );
}

TplListItem.prototype.AddToList = function() {
  $("#add-template-view option[value='" + this.view_ + "']").remove();

  if($('#tpl-list').length <= 0) {
    var n = $('<br/><table width="1000" id="tpl-list"></table>');
    $(document.body).append(n);
  }
  $('#tpl-list').append(this.dom_);
  $('#tpl-list').append(this.tpl_);
}

var StypeApplyView = function(apply_id) {
  this.dom_ = $('<div><table id="apply-table" width="1200" border="1" class="t1">' +
          '<tr>' +
            '<th colspan="2" style="font-size:20px;">创建新鲜事申请流程单</th>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">产品：</td>' +
            '<td width="30"><input id="pm-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">产品Email：</td>' +
            '<td width="30">' +
                '<input id="pm-emails" size="50" type="text"/><span class="flag no-mark"></span>' +
                '例：xxx@renren-inc.com,ooo@renren-inc.com(只接受renren-inc.com域名)' +
            '</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">开发：</td>' +
            '<td width="30"><input id="dev-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">开发Email：：</td>' +
            '<td width="30">' +
              '<input id="dev-emails" size="50" type="text"/><span class="flag no-mark"></span>' +
              '例：xxx@renren-inc.com,ooo@renren-inc.com(只接受renren-inc.com域名)' +
            '</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">新鲜事描述：</td>' +
            '<td width="30"><input id="stype-desc" type="text"/> 例：用户发日志时产生的日志新鲜事</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">产品文案：</td>' +
            '<td width="30"><textarea id="stype-text" rows="5" cols="60"></textarea></td>' +
          '</tr>' +
          '<tr>' +
            '<td width="30">是否发NewsFeed？</td>' +
            '<td width="30"><input type="checkbox" id="PushPolicy_news"/></td>' +
          '</tr>' +
           '<tr>' +
            '<td width="30">是否发MiniFeed？</td>' +
            '<td width="30"><input id="PushPolicy_mini" type="checkbox"/></td>' +
          '</tr>' +
           '<tr>' +
             '<td width="30">NewsFeed如何合并或去重？</td>' +
             '<td width="30"><input id="news-merge-desc" size="100" type="text"/></td>' +
          '</tr>' +
           '<tr>' +
             '<td width="30">MiniFeed如何合并或去重？</td>' +
             '<td width="30"><input id="mini-merge-desc" size="100" type="text"/></td>' +
          '</tr>' +
           '<tr>' +
            '<td width="30">该新鲜事在MiniFeed中保存的天数(-1永久保存)</td>' +
            '<td width="30"><input id="lifetime" value="1" size="5" maxlength="5"/>天</td>' +
          '</tr>' +
           '<tr>' +
            '<td colspan="2" align="right"><input id="apply-submit" type="button" value="提交申请"/></td>' +
          '</tr>' +
        '</table></div>');
  var email_pat = /^([_a-z0-9.]+@renren-inc[.]com,)*[_a-z0-9.]+@renren-inc\.com$/;
  function ValidateMails(node) {
    var text = node.val();
    console.log(text);
    if(email_pat.test(text)) {
      node.next().addClass('yes-mark').removeClass('no-mark');
    } else {
      node.next().addClass('no-mark').removeClass('yes-mark');
    }
  }

  $('#pm-emails, #dev-emails', this.dom_).keyup(
    function() {
      ValidateMails($(this));
    }
  );

  $('#pm-emails, #dev-emails', this.dom_).each(
    function() {
      ValidateMails($(this));
    }
  );
  var T = this;
  if (!isNaN(apply_id) && apply_id > 0) {
    $.ajax('/get-apply-feed-item?apply_id=' + apply_id, {
      'type' : 'GET',
      'success' : function(text) {
        var o = $.parseJSON(text);
        if (!o.apply_id) {
          alert('该申请id不存在');
          return;
        }
        var d = T.dom_;
        $('#pm-names', d).val(o.pm_names);
        $('#pm-emails', d).val(o.pm_emails);
        $('#dev-names', d).val(o.dev_names);
        $('#dev-emails', d).val(o.dev_emails);
        $('#stype-desc', d).val(o.stype_desc);
        $('#stype-text', d).val(o.stype_text);
        $("input:checkbox#PushPolicy_news", d).attr("checked", o.push_news);
        $("input:checkbox#PushPolicy_mini", d).attr("checked", o.push_mini);
        $('#news-merge-desc', d).val(o.news_merge_desc);
        $('#mini-merge-desc', d).val(o.mini_merge_desc);
        $('#lifetime', d).val(o.lifetime);
      },
      'error' : function() {
        alert('加载申请信息出错');
      }
    });
  }

  $('#apply-submit', this.dom_).click(
    function() {
      if ($('#pm-names', this.dom_).val().length <= 0) {
        alert('产品名字字段不能为空');
        return;
      }
      var data = 'pm_names=' + encodeURIComponent($('#pm-names', this.dom_).val());

      var pm_emails = $('#pm-emails', this.dom_).val();
      if (pm_emails.length <=0 || !email_pat.test(pm_emails)) {
        alert('产品email字段不合法');
        return;
      }
      data += '&pm_emails=' + pm_emails;

      if ($('#dev-names', this.dom_).val().length <= 0) {
        alert('开发名字字段不能为空');
        return;
      }
      data += '&dev_names=' + encodeURIComponent($('#dev-names', this.dom_).val());

      var dev_emails = $('#dev-emails', this.dom_).val();
      if (dev_emails.length <=0 || !email_pat.test(dev_emails)) {
        alert('开发email字段不合法');
        return;
      }
      data += '&dev_emails=' + dev_emails;

      var stype_desc = $('#stype-desc', this.dom_).val();
      if (stype_desc.length <= 0) {
        alert('新鲜事描述字段不能为空');
        return;
      }
      data += '&stype_desc=' + encodeURIComponent(stype_desc);

      var stype_text = $('#stype-text', this.dom_).val();
      if (stype_text.length <= 0) {
        alert('新鲜事产品文案不能为空');
        return;
      }
      data += '&stype_text=' + encodeURIComponent(stype_text);

      data += '&push_news=' + ($("input:checkbox#PushPolicy_news", this.dom_).attr("checked") ? 1 : 0);
      data += '&push_mini=' + ($("input:checkbox#PushPolicy_mini", this.dom_).attr("checked") ? 1 : 0);

      var news_merge_desc = $('#news-merge-desc', this.dom_).val();
      if (news_merge_desc.length <= 0) {
        alert('NewsFeed合并策略描述不能为空');
        return;
      }
      data += '&news_merge_desc=' + encodeURIComponent(news_merge_desc);

      var mini_merge_desc = $('#mini-merge-desc', this.dom_).val();
      if (mini_merge_desc.length <= 0) {
        alert('MiniFeed合并策略描述不能为空');
        return;
      }
      data += '&mini_merge_desc=' + encodeURIComponent(mini_merge_desc);

      var lifetime = parseInt($('#lifetime', this.dom_).val());
      if (isNaN(lifetime)) {
        alert('MiniFeed中保存天数值不合法');
        return;
      }
      data += '&lifetime=' + lifetime;
      alert(data);
      $.ajax('/apply-new-feed-submit', {
        'type' : 'POST',
        'data' : data,
        'success' : function(text) {
          alert(text);
        },
        'error' : function() {
          alert('提交出错');
        }
      });
    }
  );
}

StypeApplyView.prototype.AppendToNode = function(node) {
  node.append(this.dom_);
}

StypeApplyView.prototype.Disable = function() {
  var t = $('#apply-table', this.com_);
  $('input', t).attr('readonly', 'readonly');
  $('textarea', t).attr('readonly', 'readonly');
  $('submit', t).attr('readonly', 'readonly');
 
  $('#apply-submit', t).parent().css('display', 'none');
}

var VersionApplyView = function(apply_id) {
  this.dom_ = $('<div><table id="stype-list-table" width="1200" border="1" class="t1">' +
          '<tr>' +
            '<th colspan="2" style="font-size:20px;">新鲜事数据字段修改申请流程单</th>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品：</td>' +
            '<td width="80"><input id="pm-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品Email：</td>' +
            '<td width="80"><input id="pm-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">新鲜事STYPE（小类型）：</td>' +
            '<td width="80"><input id="stype-id" type="text"/>例：502 发状态新鲜事</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">希望在哪个版本上进行修改（选填项，请尽量填写）：</td>' +
            '<td width="80"><input id="from-version-id" type="text"/>例：在3号版本上修改，系统将把线上3号版本的所有数据帮你预先填充好，方便你的修改</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发：</td>' +
            '<td width="80"><input id="dev-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发Email：：</td>' +
            '<td width="80"><input id="dev-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">修改原因及变化：</td>' +
            '<td width="80"><input id="version-desc" type="text"/> 例：需要在新鲜事中显示用户的VIP级别，需要加入VIP级别这样一个值</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品文案或需求描述：</td>' +
            '<td width="80"><textarea id="version-text" rows="5" cols="60">' +
            '</textarea></td>' +
          '</tr>' +
           '<tr>' +
            '<td id="apply-submit" colspan="2" align="right"><input type="button" value="提交申请"/></td>' +
          '</tr>' +
        '</table></div>');
  var email_pat = /^([_a-z0-9.]+@renren-inc[.]com,)*[_a-z0-9.]+@renren-inc\.com$/;
  function ValidateMails(node) {
    var text = node.val();
    console.log(text);
    if(email_pat.test(text)) {
      node.next().addClass('yes-mark').removeClass('no-mark');
    } else {
      node.next().addClass('no-mark').removeClass('yes-mark');
    }
  }

  $('#pm-emails, #dev-emails', this.dom_).keyup(
    function() {
      ValidateMails($(this));
    }
  );

  $('#pm-emails, #dev-emails', this.dom_).each(
    function() {
      ValidateMails($(this));
    }
  );
  var T = this;
  if (!isNaN(apply_id) && apply_id > 0) {
    $.ajax('/get-version-apply-item?apply_id=' + apply_id, {
      'type' : 'GET',
      'success' : function(text) {
        var o = $.parseJSON(text);
        if (!o.apply_id) {
          alert('该申请id不存在');
          return;
        }
        var d = T.dom_;
        $('#stype-id', d).val(o.stype_text);
        $('#from-version-id', d).val(o.stype_text);
        $('#pm-names', d).val(o.pm_names);
        $('#pm-emails', d).val(o.pm_emails);
        $('#dev-names', d).val(o.dev_names);
        $('#dev-emails', d).val(o.dev_emails);
        $('#version-desc', d).val(o.version_desc);
        $('#version-text', d).val(o.version_text);
      },
      'error' : function() {
        alert('加载申请信息出错');
      }
    });
  }

  $('#apply-submit', this.dom_).click(
    function() {
      if ($('#pm-names', this.dom_).val().length <= 0) {
        alert('产品名字字段不能为空');
        return;
      }
      var data = 'pm_names=' + encodeURIComponent($('#pm-names', this.dom_).val());

      var pm_emails = $('#pm-emails', this.dom_).val();
      if (pm_emails.length <=0 || !email_pat.test(pm_emails)) {
        alert('产品email字段不合法');
        return;
      }
      data += '&pm_emails=' + pm_emails;

      var stype_id= $('#stype-id', this.dom_).val();
      if (stype_id.length <= 0) {
        alert('stype不能为空');
        return;
      }
      data += '&stype_id=' + stype_id;

      var from_version_id= $('#from-version-id', this.dom_).val();
      data += '&from_version_id=' + from_version_id;

      if ($('#dev-names', this.dom_).val().length <= 0) {
        alert('开发名字字段不能为空');
        return;
      }
      data += '&dev_names=' + encodeURIComponent($('#dev-names', this.dom_).val());

      var dev_emails = $('#dev-emails', this.dom_).val();
      if (dev_emails.length <=0 || !email_pat.test(dev_emails)) {
        alert('开发email字段不合法');
        return;
      }
      data += '&dev_emails=' + dev_emails;

      var version_desc = $('#version-desc', this.dom_).val();
      if (version_desc.length <= 0) {
        alert('新版本产品文案或需求描述不能为空');
        return;
      }
      data += '&version_desc=' + encodeURIComponent(version_desc);

      var version_text = $('#version-text', this.dom_).val();
      if (version_text.length <= 0) {
        alert('新版本产品文案或需求描述不能为空');
        return;
      }
      data += '&version_text=' + encodeURIComponent(version_text);

      alert(data);
      $.ajax('/apply-new-version-submit', {
        'type' : 'POST',
        'data' : data,
        'success' : function(text) {
          alert(text);
        },
        'error' : function() {
          alert('提交出错');
        }
      });
    }
  );
}

VersionApplyView.prototype.AppendToNode = function(node) {
  node.append(this.dom_);
}

VersionApplyView.prototype.Disable = function() {
  var t = $('#apply-table', this.com_);
  $('input', t).attr('readonly', 'readonly');
  $('textarea', t).attr('readonly', 'readonly');
  $('submit', t).attr('readonly', 'readonly');
 
  $('#apply-submit', t).parent().css('display', 'none');
}

var SeqApplyView = function(apply_id) {
  this.dom_ = $('<div><table id="stype-list-table" width="1200" border="1" class="t1">' +
          '<tr>' +
            '<th colspan="2" style="font-size:20px;">新鲜事模板修改申请流程单</th>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品：</td>' +
            '<td width="80"><input id="pm-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品Email：</td>' +
            '<td width="80"><input id="pm-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">新鲜事STYPE（小类型）：</td>' +
            '<td width="80"><input id="stype-id" type="text"/>例：502 发状态新鲜事</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">stype数据版本id：</td>' +
            '<td width="80"><input id="version-id" type="text"/></td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">希望在哪个模板序号上进行修改（选填项，请尽量填写）：</td>' +
            '<td width="80"><input id="from-seq-id" type="text"/>例：在3号版本上修改，系统将把线上3号版本的所有数据帮你预先填充好，方便你的修改</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发：</td>' +
            '<td width="80"><input id="dev-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发Email：：</td>' +
            '<td width="80"><input id="dev-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">修改原因及变化：</td>' +
            '<td width="80"><input id="seq-desc" type="text"/> 例：需要在新鲜事中显示用户的VIP级别，需要加入VIP级别这样一个值</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品文案或需求描述：</td>' +
            '<td width="80"><textarea id="seq-text" rows="5" cols="60">' +
            '</textarea></td>' +
          '</tr>' +
           '<tr>' +
            '<td id="apply-submit" colspan="2" align="right"><input type="button" value="提交申请"/></td>' +
          '</tr>' +
        '</table></div>');
  var email_pat = /^([_a-z0-9.]+@renren-inc[.]com,)*[_a-z0-9.]+@renren-inc\.com$/;
  function ValidateMails(node) {
    var text = node.val();
    console.log(text);
    if(email_pat.test(text)) {
      node.next().addClass('yes-mark').removeClass('no-mark');
    } else {
      node.next().addClass('no-mark').removeClass('yes-mark');
    }
  }

  $('#pm-emails, #dev-emails', this.dom_).keyup(
    function() {
      ValidateMails($(this));
    }
  );

  $('#pm-emails, #dev-emails', this.dom_).each(
    function() {
      ValidateMails($(this));
    }
  );
  var T = this;
  if (!isNaN(apply_id) && apply_id > 0) {
    $.ajax('/get-seq-apply-item?apply_id=' + apply_id, {
      'type' : 'GET',
      'success' : function(text) {
        var o = $.parseJSON(text);
        if (!o.apply_id) {
          alert('该申请id不存在');
          return;
        }
        var d = T.dom_;
        $('#stype-id', d).val(o.stype_id);
        $('#version-id', d).val(o.version);
        $('#from-seq-id', d).val(o.from_seq_id);
        $('#pm-names', d).val(o.pm_names);
        $('#pm-emails', d).val(o.pm_emails);
        $('#dev-names', d).val(o.dev_names);
        $('#dev-emails', d).val(o.dev_emails);
        $('#seq-desc', d).val(o.seq_desc);
        $('#seq-text', d).val(o.seq_text);
      },
      'error' : function() {
        alert('加载申请信息出错');
      }
    });
  }

  $('#apply-submit', this.dom_).click(
    function() {
      if ($('#pm-names', this.dom_).val().length <= 0) {
        alert('产品名字字段不能为空');
        return;
      }
      var data = 'pm_names=' + encodeURIComponent($('#pm-names', this.dom_).val());

      var pm_emails = $('#pm-emails', this.dom_).val();
      if (pm_emails.length <=0 || !email_pat.test(pm_emails)) {
        alert('产品email字段不合法');
        return;
      }
      data += '&pm_emails=' + pm_emails;

      var stype_id= $('#stype-id', this.dom_).val();
      if (stype_id.length <= 0) {
        alert('stype不能为空');
        return;
      }
      data += '&stype_id=' + stype_id;

      var version = $('#version-id', this.dom_).val();
      if (version.length <= 0) {
        alert('version不能为空');
        return;
      }
      data += '&version=' + version;

      var from_seq_id= $('#from-seq-id', this.dom_).val();
      data += '&from_seq_id=' + from_seq_id;

      if ($('#dev-names', this.dom_).val().length <= 0) {
        alert('开发名字字段不能为空');
        return;
      }
      data += '&dev_names=' + encodeURIComponent($('#dev-names', this.dom_).val());

      var dev_emails = $('#dev-emails', this.dom_).val();
      if (dev_emails.length <=0 || !email_pat.test(dev_emails)) {
        alert('开发email字段不合法');
        return;
      }
      data += '&dev_emails=' + dev_emails;

      var seq_desc = $('#seq-desc', this.dom_).val();
      if (seq_desc.length <= 0) {
        alert('新版本产品文案或需求描述不能为空');
        return;
      }
      data += '&seq_desc=' + encodeURIComponent(seq_desc);

      var seq_text = $('#seq-text', this.dom_).val();
      if (seq_text.length <= 0) {
        alert('新版本产品文案或需求描述不能为空');
        return;
      }
      data += '&seq_text=' + encodeURIComponent(seq_text);

      alert(data);
      $.ajax('/apply-new-seq-submit', {
        'type' : 'POST',
        'data' : data,
        'success' : function(text) {
          alert(text);
        },
        'error' : function() {
          alert('提交出错');
        }
      });
    }
  );
}

SeqApplyView.prototype.AppendToNode = function(node) {
  node.append(this.dom_);
}

SeqApplyView.prototype.Disable = function() {
  var t = $('#apply-table', this.com_);
  $('input', t).attr('readonly', 'readonly');
  $('textarea', t).attr('readonly', 'readonly');
  $('submit', t).attr('readonly', 'readonly');
 
  $('#apply-submit', t).parent().css('display', 'none');
}

var RollbackApplyView = function(apply_id) {
  this.dom_ = $('<div><table id="stype-list-table" width="1200" border="1" class="t1">' +
          '<tr>' +
            '<th colspan="2" style="font-size:20px;">新鲜事版本回退申请流程单</th>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品：</td>' +
            '<td width="80"><input id="pm-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品Email：</td>' +
            '<td width="80"><input id="pm-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">新鲜事STYPE（小类型）：</td>' +
            '<td width="80"><input id="stype-id" type="text"/>例：502 发状态新鲜事</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">stype数据版本id：</td>' +
            '<td width="80"><input id="version-id" type="text"/></td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">模板序号：</td>' +
            '<td width="80"><input id="seq-id" type="text"/></td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发：</td>' +
            '<td width="80"><input id="dev-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发Email：：</td>' +
            '<td width="80"><input id="dev-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">回退原因：</td>' +
            '<td width="80"><input id="rollback-desc" type="text"/> 例：脚本有语法错误</td>' +
          '</tr>' +
          '<tr>' +
            '<td id="apply-submit" colspan="2" align="right"><input type="button" value="提交申请"/></td>' +
          '</tr>' +
        '</table></div>');
  var email_pat = /^([_a-z0-9.]+@renren-inc[.]com,)*[_a-z0-9.]+@renren-inc\.com$/;
  function ValidateMails(node) {
    var text = node.val();
    console.log(text);
    if(email_pat.test(text)) {
      node.next().addClass('yes-mark').removeClass('no-mark');
    } else {
      node.next().addClass('no-mark').removeClass('yes-mark');
    }
  }

  $('#pm-emails, #dev-emails', this.dom_).keyup(
    function() {
      ValidateMails($(this));
    }
  );

  $('#pm-emails, #dev-emails', this.dom_).each(
    function() {
      ValidateMails($(this));
    }
  );
  var T = this;
  if (!isNaN(apply_id) && apply_id > 0) {
    $.ajax('/get-rollback-item?apply_id=' + apply_id, {
      'type' : 'GET',
      'success' : function(text) {
        var o = $.parseJSON(text);
        if (!o.apply_id) {
          alert('该申请id不存在');
          return;
        }
        var d = T.dom_;
        $('#stype-id', d).val(o.stype_id);
        $('#version-id', d).val(o.version);
        $('#seq-id', d).val(o.seq_id);
        $('#pm-names', d).val(o.pm_names);
        $('#pm-emails', d).val(o.pm_emails);
        $('#dev-names', d).val(o.dev_names);
        $('#dev-emails', d).val(o.dev_emails);
        $('#rollback-desc', d).val(o.rollback_desc);
      },
      'error' : function() {
        alert('加载申请信息出错');
      }
    });
  }

  $('#apply-submit', this.dom_).click(
    function() {
      if ($('#pm-names', this.dom_).val().length <= 0) {
        alert('产品名字字段不能为空');
        return;
      }
      var data = 'pm_names=' + encodeURIComponent($('#pm-names', this.dom_).val());

      var pm_emails = $('#pm-emails', this.dom_).val();
      if (pm_emails.length <=0 || !email_pat.test(pm_emails)) {
        alert('产品email字段不合法');
        return;
      }
      data += '&pm_emails=' + pm_emails;

      var stype_id= $('#stype-id', this.dom_).val();
      if (stype_id.length <= 0) {
        alert('stype不能为空');
        return;
      }
      data += '&stype_id=' + stype_id;

      var version = $('#version-id', this.dom_).val();
      if (version.length <= 0) {
        alert('version不能为空');
        return;
      }
      data += '&version=' + version;

      var seq_id= $('#seq-id', this.dom_).val();
      data += '&seq_id=' + seq_id;

      if ($('#dev-names', this.dom_).val().length <= 0) {
        alert('开发名字字段不能为空');
        return;
      }
      data += '&dev_names=' + encodeURIComponent($('#dev-names', this.dom_).val());

      var dev_emails = $('#dev-emails', this.dom_).val();
      if (dev_emails.length <=0 || !email_pat.test(dev_emails)) {
        alert('开发email字段不合法');
        return;
      }
      data += '&dev_emails=' + dev_emails;

      var rollback_desc = $('#rollback-desc', this.dom_).val();
      if (rollback_desc.length <= 0) {
        alert('新版本产品文案或需求描述不能为空');
        return;
      }
      data += '&rollback_desc=' + encodeURIComponent(rollback_desc);

      alert(data);
      $.ajax('/apply-rollback-submit', {
        'type' : 'POST',
        'data' : data,
        'success' : function(text) {
          alert(text);
        },
        'error' : function() {
          alert('提交出错');
        }
      });
    }
  );
}

RollbackApplyView.prototype.AppendToNode = function(node) {
  node.append(this.dom_);
}

RollbackApplyView.prototype.Disable = function() {
  var t = $('#apply-table', this.com_);
  $('input', t).attr('readonly', 'readonly');
  $('textarea', t).attr('readonly', 'readonly');
  $('submit', t).attr('readonly', 'readonly');
 
  $('#apply-submit', t).parent().css('display', 'none');
}

var PublishApplyView = function(apply_id) {
  this.dom_ = $('<div><table id="stype-list-table" width="1200" border="1" class="t1">' +
          '<tr>' +
            '<th colspan="2" style="font-size:20px;">新鲜事版本上线申请流程单</th>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品：</td>' +
            '<td width="80"><input id="pm-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">产品Email：</td>' +
            '<td width="80"><input id="pm-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">新鲜事STYPE（小类型）：</td>' +
            '<td width="80"><select id="stype-id" style="width:70px;"></select>例：502 发状态新鲜事</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">stype数据版本id：</td>' +
            '<td width="80"><select id="version-id" style="width:70px;"></select></td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">模板序号：</td>' +
            '<td width="80"><select id="seq-id" style="width:70px;"></select></td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发：</td>' +
            '<td width="80"><input id="dev-names" type="text"/>例：张三,李四,王五</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">开发Email：：</td>' +
            '<td width="80"><input id="dev-emails" size="50" type="text"/><span class="flag no-mark"></span>例：xxx@renren-inc.com,ooo@renren-inc.com</td>' +
          '</tr>' +
          '<tr>' +
            '<td width="20">版本描述：</td>' +
            '<td width="80"><input id="publish-desc" type="text"/> 例：增加了分享按钮</td>' +
          '</tr>' +
          '<tr>' +
            '<td id="apply-submit" colspan="2" align="right"><input type="button" value="提交申请"/></td>' +
          '</tr>' +
        '</table></div>');
  var T = this; 
  $('#stype-id', this.dom_).html(GetStypeIdsHtml('')).change(
    function() {
      $.ajax('/get-stype-versions?stype=' + $(this).val(), {
        async : false,
        success : function(text){
          var ids = $.parseJSON(text);
          $('#version-id', T.dom_).html('');
          for(var i = 0; i < ids.length; ++i) {
            $('#version-id', T.dom_).append($('<option value="' + ids[i].version + '">' + ids[i].version + '</option>'));
          }
        },
        error : function(){
          alert('/get-stype-versions 出错');
        }
      });
    }
  );

  $('#version-id', this.dom_).change(
    function() {
      $.ajax('/get-stype-version-seqs?stype=' + $('#stype-id', T.dom_).val()
        + '&version=' + $('#version-id', T.dom_).val(), {
        async : false,
        success : function(text){
          var ids = $.parseJSON(text);
          $('#seq-id', T.dom_).html('');
          for(var i = 0; i < ids.length; ++i) {
            $('#seq-id', T.dom_).append($('<option value="' + ids[i] + '">' + ids[i] + '</option>'));
          }
        },
        error : function(){
          alert('/get-stype-version-seqs 出错');
        }
      });
    }
  );


  var email_pat = /^([_a-z0-9.]+@renren-inc[.]com,)*[_a-z0-9.]+@renren-inc\.com$/;
  function ValidateMails(node) {
    var text = node.val();
    console.log(text);
    if(email_pat.test(text)) {
      node.next().addClass('yes-mark').removeClass('no-mark');
    } else {
      node.next().addClass('no-mark').removeClass('yes-mark');
    }
  }

  $('#pm-emails, #dev-emails', this.dom_).keyup(
    function() {
      ValidateMails($(this));
    }
  );

  $('#pm-emails, #dev-emails', this.dom_).each(
    function() {
      ValidateMails($(this));
    }
  );
  var T = this;

  if (!isNaN(apply_id) && apply_id > 0) {
    $.ajax('/get-publish-item?apply_id=' + apply_id, {
      'type' : 'GET',
      'success' : function(text) {
        var o = $.parseJSON(text);
        if (!o.apply_id) {
          alert('该申请id不存在');
          return;
        }
        var d = T.dom_;
        $('#stype-id', d).val(o.stype_id);
        $('#version-id', d).val(o.version);
        $('#seq-id', d).val(o.seq_id);
        $('#pm-names', d).val(o.pm_names);
        $('#pm-emails', d).val(o.pm_emails);
        $('#dev-names', d).val(o.dev_names);
        $('#dev-emails', d).val(o.dev_emails);
        $('#publish-desc', d).val(o.publish_desc);
      },
      'error' : function() {
        alert('加载申请信息出错');
      }
    });
  }

  $('#apply-submit', this.dom_).click(
    function() {
      if ($('#pm-names', this.dom_).val().length <= 0) {
        alert('产品名字字段不能为空');
        return;
      }
      var data = 'pm_names=' + encodeURIComponent($('#pm-names', this.dom_).val());

      var pm_emails = $('#pm-emails', this.dom_).val();
      if (pm_emails.length <=0 || !email_pat.test(pm_emails)) {
        alert('产品email字段不合法');
        return;
      }
      data += '&pm_emails=' + pm_emails;

      var stype_id= $('#stype-id', this.dom_).val();
      if (!stype_id || stype_id.length <= 0) {
        alert('stype不能为空');
        return;
      }
      data += '&stype_id=' + stype_id;

      var version = $('#version-id', this.dom_).val();
      if (!version || version.length <= 0) {
        alert('version不能为空');
        return;
      }
      data += '&version=' + version;

      var seq_id = $('#seq-id', T.dom_).val();
      if (!seq_id || seq_id.length <= 0) {
        alert('模板序号不能为空');
        return;
      }
      data += '&seq_id=' + seq_id;

      if ($('#dev-names', this.dom_).val().length <= 0) {
        alert('开发名字字段不能为空');
        return;
      }
      data += '&dev_names=' + encodeURIComponent($('#dev-names', this.dom_).val());

      var dev_emails = $('#dev-emails', this.dom_).val();
      if (dev_emails.length <=0 || !email_pat.test(dev_emails)) {
        alert('开发email字段不合法');
        return;
      }
      data += '&dev_emails=' + dev_emails;

      var publish_desc = $('#publish-desc', this.dom_).val();
      if (publish_desc.length <= 0) {
        alert('新版本产品文案或需求描述不能为空');
        return;
      }
      data += '&publish_desc=' + encodeURIComponent(publish_desc);

      alert(data);
      $.ajax('/apply-publish-submit', {
        'type' : 'POST',
        'data' : data,
        'success' : function(text) {
          alert(text);
        },
        'error' : function() {
          alert('提交出错');
        }
      });
    }
  );
}

PublishApplyView.prototype.AppendToNode = function(node) {
  node.append(this.dom_);
}

PublishApplyView.prototype.Disable = function() {
  var t = $('#apply-table', this.com_);
  $('input', t).attr('readonly', 'readonly');
  $('textarea', t).attr('readonly', 'readonly');
  $('submit', t).attr('readonly', 'readonly');
 
  $('#apply-submit', t).parent().css('display', 'none');
}

var SchemaParser = function() {
  var non_loop_nodes = [], loop_nodes = [];
  function GetNodePath(nodes, sep) {
    var ret = '';
    for(var i = 0; i < nodes.length; ++i) {
      if (i > 0) {
        ret += sep;
      }
      ret += nodes[i];
    }

    return ret;
  }

  function GetPathValue() {
    var value;
    if (loop_nodes.length > 0) {
      value = GetNodePath(loop_nodes, '.');
    } else {
      value = GetNodePath(non_loop_nodes, '.');
    }
    return value;
  }
  function GetFullKey(key, firstnode) {
    var prefix;
    if (loop_nodes.length > 0) {
      prefix = 'loop.' + GetNodePath(non_loop_nodes, '_') + '.' + GetNodePath(non_loop_nodes, '_') + '_' + GetNodePath(loop_nodes, '_');
    } else {
      prefix = (firstnode ? firstnode : 'keys.') + GetNodePath(non_loop_nodes, '_');
    }

    return prefix + '.' + key;
  }
  var hdf_map_ = '';
  this.GetHdfMap = function() {
    return hdf_map_;
  }

  function AddHdfMap(item) {
    if (hdf_map_.length > 0) {
      hdf_map_ += ';';
    }
    hdf_map_ += item;
  }

  function DoParse(node, looping) {
    var childs = node.children();
    if (looping) {
      loop_nodes.push(node.attr("name"));
    } else {
      if (node.attr("name") != "f") {
        non_loop_nodes.push(node.attr("name"));
      }
    }
    // console.log("looping = " + looping);

    var current_looping = (node.attr("is_loop") == "1");
    if (childs.length > 0) {
      if (current_looping) {
        AddHdfMap("FeedStruct." + GetFullKey("name", "loop.") + "=" + GetPathValue());
        AddHdfMap("FeedStruct." + GetFullKey("type", "loop.") + "=node");
        AddHdfMap("FeedStruct." + GetFullKey("comment", "loop.") + "=" + node.attr("comment"));

        console.log("FeedStruct." + GetFullKey("name", "loop.") + "=" + GetPathValue());
        console.log("FeedStruct." + GetFullKey("type", "loop.") + "=" + "node");
        console.log('');
      }

      for(var i = 0; i < childs.length; ++i) {
        DoParse($(childs[i]), current_looping || looping);
      }
    } else {
      AddHdfMap("FeedStruct." + GetFullKey("name") + "=" + GetPathValue());
      AddHdfMap("FeedStruct." + GetFullKey("type") + "=" + node.attr("type"));
      AddHdfMap("FeedStruct." + GetFullKey("comment") + '=' + node.attr("comment"));

      console.log("FeedStruct." + GetFullKey("name") + "=" + GetPathValue());
      console.log("FeedStruct." + GetFullKey("type") + "=" + node.attr("type"));
      console.log("FeedStruct." + GetFullKey("comment") + '="' + decodeURIComponent(node.attr("comment")) + '"');
      console.log('');
    }

    if (looping) {
      loop_nodes.pop();
    } else {
      if (node.attr("name") != "f") {
        non_loop_nodes.pop();
      }
    }
  }

  this.ParseSchema = function(stype, version, xml) {
    hdf_map_ = "FeedStruct.stype=" + stype + ";FeedStruct.version=" + version + ";";
    DoParse($('<key name="f" comment="feedroot" type="node">' + xml + '</key>'));
  }
}


