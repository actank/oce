<div id="single-column">
  <div id="list-results">
    <ol>
      {{>PAGE_RESULT}}
      {{>CLUB_RESULT}}
      {{>APP_RESULT}}
      {{>USER_RESULT}}
    </ol>
  </div>
</div>
{{! 用于返回显示的数据}}
<input type="hidden" value="{{OUTER}}" id="outer">
<input type="hidden" value="{{RESULT_NUM}}" id="resultNum">
<input type="hidden" value="{{USER_NUM}}" id="userNum">
<input type="hidden" value="{{PAGE_NUM}}" id="pageNum">
<input type="hidden" value="{{APP_NUM}}" id="appNum">
<input type="hidden" value="{{SEARCH_EX_S}}" id="searchExS">

<input type="hidden" value="{{ANTISPAM_RST}}" id="antispamRst">



<div style="display: none;" id="searchExQ">{{SEARCH_EX_Q}}</div>
<div style="display: none;" id="searchExP">{{SEARCH_EX_P}}</div>
