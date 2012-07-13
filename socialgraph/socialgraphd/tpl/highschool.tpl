<li class="clearfix" id="commonFriend_{{ID}}" onmouseover="Sizzle('a.shut-new',this)[0].style.display='block'" onmouseout="Sizzle('a.shut-new',this)[0].style.display='none'">
	<a class="shut-new x-to-hide" href="javascript:;" title="不再推荐此人" onclick="XN.app.pymk.remove('{{ID}}');logRcd({action:'frtPage_recommend_P_forbid', guest_id:{{ID}},type:'{{TYPE_VALUE}}'});return false;"></a>
	<span class="headpichold">
		<a href="http://www.renren.com/profile.do?id={{ID}}" onclick="logRcd({action:'frtPage_recommend_P_imgToPrf',guest_id:{{ID}}})">
			<img src="{{TINY_URL}}" width="50" height="50" onload="clipImage(this)" />
		</a>
	</span>
	<span>
		<a class="name" href="http://www.renren.com/profile.do?id={{ID}}" onclick="logRcd({action:'frtPage_recommend_P_nameToPrf',guest_id:{{ID}}})">{{NAME}}</a>
		<p>
			<a class="addfriend_action" href="javascript:;" onclick="showRequestFriendDialog('{{ID}}','{{NAME}}','{{TINY_URL}}', '', '', XN.app.pymk.removeFromAdd);logRcd({action:'frtPage_recommend_P_addFriend',guest_id:{{ID}},type:'{{TYPE_VALUE}}'});return false;"><img src="http://a.xnimg.cn/imgpro/icons/plus-green.png" /> 加为好友</a>
		</p>
		<p><a href="javascript:;" onclick="XN.app.pymk.showProfile('{{ID}}');logRcd({action:'frtPage_recommend_P_info',guest_id:{{ID}}});">查看资料</a><span class="pymk-type">(高中校友)</span></p>	
	</span>	
	<div style="display:none;" class="comefrom" id="more-mayknow-{{ID}}">
		<div id="more-person-data-{{ID}}"></div>
	</div>
	<input type="hidden" id="type_value_{{ID}}" value="{{TYPE_VALUE}}"/>
</li>

