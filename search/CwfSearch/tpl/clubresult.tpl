<li>
  <p class="avatar Starimg">
    <a href="http://org.renren.com/{{ID}}" target="_blank">
      <img src="{{HEAD_IMG}}" alt="{{HEAD_IMG}}" title="" onclick="statistic('{{ID}}','{{NAME}}','{{FRIEND_TYPE}}',             '{{COMMON_NUM}}','{{TOTAL_COUNT}}','{{INDEX}}','head');"/>
    </a>
  </p>
  <div class="info">
    <dl>
      <dt>俱乐部:</dt>
      <dd>
        <strong><a href="http://org.renren.com/{{ID}}" target="_blank" onclick="statistic('{{ID}}','{{QUERY}}','{{FRIEND_TYPE}}','{{COMMON_NUM}}','{{TOTAL_COUNT}}','{{INDEX}}','name');">{{NAME}}</a></strong>
        <span></span>
        <span></span>
      </dd>
      <dt>类型:</dt>
      <dd>
        {{PAGE_TYPE}}
      </dd>
      <dt>规模:</dt>
      <dd>
        {{FANS_COUNT}}个成员
      </dd>
    </dl>
  </div>
  <ul class="actions">
    {{>PAGE_HOME}}
    {{>JOIN_FANS}}
  </ul>
</li>

