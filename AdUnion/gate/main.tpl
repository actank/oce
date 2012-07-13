<html><head>
<style type="text/css">
body{margin:0;padding:10px 0 0 0;border:1px solid #d6dfe4;font-size:12px;}
a{color:#005eac;text-decoration:none;}
a:hover{text-decoration:underline;}
div.union_item {border-right:1px solid #d9d9d9;float:left;clear:none;}
</style>
<style>
.rr-union-link a{display:block;width:21px;height:17px;background:url(http://jebe.xnimg.cn/default/union/rrunion-icon.gif) 0 0 no-repeat;clear:both;position:absolute;bottom:0;right:0;}
.rr-union-link a:hover{display:block;width:87px;height:17px;background:url(http://jebe.xnimg.cn/default/union/rrunion-icon2.gif) 0 0 no-repeat;}
</style>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"></head>
<body>
  {{#AD_ITEMS}}
  <div class="union_item">
  <a target="_blank" href="http://ad.renren.com/union/redirect.html?aid={{AD_ID}}&url={{AD_DEST_URL}}{{AD_DEST_APPENDIX}}"><img style="border:1px;margin:0 5px 0 5px;float:left;" src="{{AD_IMG_URL}}" /></a>
    <a target="_blank" style="display:block;margin-left:85px;font-weight:700;line-height:18px;" href="http://ad.renren.com/union/redirect.html?aid={{AD_ID}}&url={{AD_DEST_URL}}{{AD_DEST_APPENDIX}}">{{AD_TITLE}}</a>
    <p style="margin:0 0 0 85px;"><a target="_blank" href="http://ad.renren.com/union/redirect.html?aid={{AD_ID}}&url={{AD_DEST_URL}}{{AD_DEST_APPENDIX}}" style="display:block;color:#808080;line-height:18px;">{{AD_CONTENT}}</a></p>
  </div>
  {{/AD_ITEMS}}
  <div class="rr-union-link">
    <a target="_blank" href="http://www.renren.com/xn.do?ss=17043&rt=1&g=union"></a>
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
var divs = getElementsByClassName('union_item');
var width = ClientWidth() / divs.length - 2;
for (var i = 0; i < divs.length; ++i)
{
  divs[i].style.width = width;
  if (i == divs.length - 1) {
    divs[i].style.border = '0';
  }
}
if (env.ff) {
  document.body.style.height = (ClientHeight() - 12) + 'px';
}
//-->
</script>
</body></html>
