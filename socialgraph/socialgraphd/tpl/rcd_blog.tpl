<li class="clearfix" id="commonFriend_{{RCD_BLOG_ID}}" onmouseover="Sizzle('a.shut-new',this)[0].style.display='block'"
onmouseout="Sizzle('a.shut-new',this)[0].style.display='none'">
	<a class="shut-new x-to-hide" href="javascript:;" title="不再推荐" onclick="XN.app.pymk.remove('{{RCD_BLOG_ID}}');logRcd({action:'frtPage_recommend_P_forbid', guest_id:{{RCD_BLOG_ID}},type:'{{TYPE_VALUE}}'});return false;">
	</a>
	<span class="ugc-type"><img src="http://a.xnimg.cn/imgpro/icons/rcd_blog.png" class="icon" />日志</span>
	<span class="ugc-title">
		<a onclick="logRcd({action:'frtPage_recommend_C_click', guest_id:{{RCD_BLOG_ID}},type:'{{TYPE_VALUE}}'});" href="{{RCD_BLOG_URL}}">{{RCD_BLOG_TITLE}}</a>
	</span>
	<input type="hidden" id="type_value_{{RCD_BLOG_ID}}" value="{{TYPE_VALUE}}" />
</li>
