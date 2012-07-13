<html><head>
<style type="text/css">
body {
  margin:0;
  padding:0;
}
div.union_item {
border-right:1px solid #d9d9d9;float:left;clear:none;
}
</style>
<style>
.rr-union-link a{display:block;width:21px;height:17px;background:url(http://jebe.xnimg.cn/default/union/rrunion-icon.gif) 0 0 no-repeat;clear:both;position:absolute;bottom:0;right:0;}
.rr-union-link a:hover{display:block;width:87px;height:17px;background:url(http://jebe.xnimg.cn/default/union/rrunion-icon2.gif) 0 0 no-repeat;}
.rr-holder {display:block;border:1px solid #d6dfe4;height:90px;padding:10px 0 0 0;position:relative;font-size:12px;}
.rr-holder a{color:#005eac;text-decoration:none;}
.rr-holder a:hover{text-decoration:underline;}
span.wkey{color:#172;font-weight:bold;}
</style>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"></head>
<body>
<div  class="rr-holder">
  {{#AD_ZONES}}
  <div style="clear:both; border:red 1px solid;height:150px;margin-right:2px;">
  ad zone id {{AD_ZONE_ID}}</br>
  {{#AD_ITEMS}}
  <div class="union_item">
    <a target="_blank" style="display:block;margin-left:2px;font-weight:700;line-height:18px;" href="http://ebp.renren.com/ebp/report.html?aid={{AD_ID}}&url={{AD_DEST_URL}}&mc={{AD_MATCHER_COOKIE}}">{{AD_TITLE}}</a>
    <p style="margin:0 0 0 2px;"><a target="_blank" href="http://ebp.renren.com/ebp/report.html?aid={{AD_ID}}&url={{AD_DEST_URL}}&mc={{AD_MATCHER_COOKIE}}" style="display:block;color:#808080;line-height:18px;"><span class="wkey">widget </span>{{AD_WIDGET}} <span class="wkey">widget id:</span>{{AD_WIDGET_ID}} <span class="wkey">widget version:</span>{{AD_WIDGET_VERSION}}</a></p>
    <p style="margin:0 0 0 2px;"><span class="wkey">creative id :</span>{{AD_ID}} </a></p>
  </div>
  {{/AD_ITEMS}}
  </div>
  {{/AD_ZONES}}
  <div class="rr-union-link">
    <a target="_blank" href="http://www.renren.com/xn.do?ss=17043&rt=1&g=union"></a>
  </div>
</div>
<script type="text/javascript">//<!--

function getElementsByClassName(classname, node)  {
  if(!node) node = document.getElementsByTagName("body")[0];
  var a = [];
  var re = new RegExp('\\b' + classname + '\\b');
  var els = node.getElementsByTagName("*");
  for(var i=0,j=els.length; i<j; i++)
    if(re.test(els[i].className))a.push(els[i]);
  return a;
}
function ClientHeight () {
  var b = document.body;
  var p = b.parentNode;
  var bcHeight = b.clientHeight;
  var pcHeight = p.clientHeight;
  if (env.ie) {
    return (pcHeight == 0) ? bcHeight : pcHeight;
  } else {
    return (pcHeight == p.offsetHeight
        && pcHeight == p.scrollHeight) ? bcHeight : pcHeight;
  }
  return bcHeight;
}
var ua = navigator.userAgent.toLowerCase();
var env = {
  ff: /gecko/.test(ua) && /rv/.test(ua),
  opera : /opera/.test(ua),
  chrome: /chrome/.test(ua),
  webkit : /webkit/.test(ua) && !/opera/.test(ua) && !/chrome/.test(ua),
  ie : /msie/.test(ua) && !/opera/.test(ua)
};

function ClientWidth() 
{
  var b = document.body;
  var p = b.parentNode;
  var bcWidth = b.clientWidth;
  var pcWidth = p.clientWidth;
  if (env.ie) { // && !OS.isOpera
    return (pcWidth == 0) ? bcWidth : pcWidth;
  } else if (env.ff) {
    return (pcWidth == p.offsetWidth
        && pcWidth == p.scrollWidth) ? bcWidth : pcWidth;
  }
  return bcWidth;
}
var divs = getElementsByClassName('union_item');
var width = ClientWidth() / divs.length - 8;
for (var i = 0; i < divs.length; ++i)
{
  divs[i].style.width = width;
  if (i == divs.length - 1) {
    divs[i].style.border = '0';
  }
}

var divs = getElementsByClassName('rr-holder');
var h = ClientHeight() - 2;
if (!env.ie) {
  h -= 10;
}

with(divs[0].style) {
  height = h + 'px';
}
//-->
</script>
</body></html>
