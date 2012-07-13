<li class="{{SELECT_ALL}}" id="taball">
  <a href="#all" onclick="return XN.app.search.tabLink('all');">&nbsp;所有结果
    <span id="resultS" style="display: none;">{{COUNT}}</span>&nbsp;</a>
</li>

{{#TAB_LI}}
  <li id="{{TAB_ID}}" class="{{SELECT}}">
    <a href="#{{TAB_HREF}}" onclick="return XN.app.search.tabLink({{TAB_LINK}});">&nbsp;{{TAB_TYPE}}({{TAB_TYPE_COUNT}})&nbsp;</a>
  </li>
{{/TAB_LI}}
