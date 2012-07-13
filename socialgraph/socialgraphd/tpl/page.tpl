<li class="clearfix" id="commonFriend_{{ID}}" onmouseover="Sizzle('a.shut-new',this)[0].style.display='block'" onmouseout="Sizzle('a.shut-new',this)[0].style.display='none'">
	<a class="shut-new x-to-hide" href="javascript:;" title="不再推荐此公共主页" onclick="XN.app.pymk.remove('{{ID}}');logRcd({action:'frtPage_recommend_P_forbid', guest_id:{{ID}},type:'{{TYPE_VALUE}}'});return false;"></a>
	<span class="headpichold">
		<a href="http://page.renren.com/{{ID}}" onclick="logRcd({action:'frtPage_recommend_P_imgToPrf',guest_id:{{ID}}})">
			<img src="{{TINY_URL}}" width="50" height="50" onload="clipImage(this)" />
		</a>
	</span>
	<span>
		<a class="name" href="http://page.renren.com/{{ID}}" onclick="logRcd({action:'frtPage_recommend_P_nameToPrf',guest_id:{{ID}}})">{{NAME}}</a>
		<p>
			<a class="addfriend_action" href="#nogo" onclick="requestPageFriend('{{ID}}',XN.app.pymk.removeFromAdd);logRcd({action:'frtPage_recommend_P_addFriend',guest_id:{{ID}},type:'{{TYPE_VALUE}}'});return false;">
				<img src="http://a.xnimg.cn/imgpro/icons/follow-add.gif" /> 加为好友
			</a>
		</p>
		<p>
			{{#FIRST_FIELDS}}
				<a href="http://www.renren.com/profile.do?id={{FIRST_FIELD_ID}}">{{FIRST_FIELD_NAME}}</a>&nbsp;
			{{/FIRST_FIELDS}}
			等<a href="#nogo" onclick="XN.app.pymk.showPageProfile('{{ID}}');logRcd({action:'frtPage_recommend_P_info',guest_id:{{ID}}});return false;">{{FANS}}人关注</a>
		</p>
	</span>
	<div style="display:none;" class="comefrom" id="more-mayknow-{{ID}}">
		<div id="more-person-data-{{ID}}">
			<p><span class="profile-label">简介：</span>{{DESCRIPTION}}</p>
		</div>
		<p>
			<span class="profile-label">共同好友：</span>
			{{#FIELDS}}
				<a href="http://www.renren.com/profile.do?id={{FIELD_ID}}">{{FIELD_NAME}}</a>&nbsp;
			{{/FIELDS}}
			等{{COMMONS}}人
		</p>
	</div>
	<input type="hidden" id="type_value_{{ID}}" value="{{TYPE_VALUE}}" />
</li>


