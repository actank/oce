<li>
  <p class="avatar Userimg">
    <a href="http://www.renren.com/profile.do?id={{ID}}&amp;from={{INDEX}}&amp;ref=userSearchResult[{{INDEX}}]" target="_blank">
      <img src="{{HEAD_IMG}}" alt="{{HEAD_IMG}}" title="" onclick="statistic('{{ID}}','{{QUERY}}','{{FRIEND_TYPE}}', '{{COMMON_NUM}}','{{TOTAL_COUNT}}','{{INDEX}}','head');"/>
    </a>
  </p>
  <div class="info">
    <dl>
      <dt>姓名:</dt>
      <dd>
        <strong><a href="http://www.renren.com/profile.do?id={{ID}}&amp;from={{INDEX}}&amp;ref=userSearchResult[{{INDEX}}]" target="_blank" onclick="statistic('{{ID}}','{{QUERY}}','{{FRIEND_TYPE}}','{{COMMON_NUM}}','{{TOTAL_COUNT}}','{{INDEX}}','name');">{{NAME}}</a></strong>
        <span>
          {{>STAR}}  {{! just include tpl, no dictionary}}
        </span>
        <span>
          {{>VIP}}   {{! just include tpl, no dictionary}}
        </span>
      </dd>
      {{>USER_INFO}}
      {{>COMMON_FRIENDS}}
    </dl>
  </div>
  <ul class="actions">
    {{>ADD_SEND}}
    {{>SEND_MSG}}
    {{>WRITE_COMMENT}}
  </ul>
</li>
