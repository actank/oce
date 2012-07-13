<li class="clearfix" id="commonFriend_{{ID}}" onmouseover="Sizzle('a.shut-new',this)[0].style.display='block'" onmouseout="Sizzle('a.shut-new',this)[0].style.display='none'">
	<!-- {{ID}}{{TINY_URL}}
	{{NAME}}
	{{MESSAGE}}
    {{TYPE_VALUE}} -->
    
    <a onclick="XN.app.pymk.remove({{ID}});logRcd({action:'frtPage_recommend_R_forbid', guest_id:{{ID}},type:'{{TYPE_VALUE}}'});return false;" title="不给TA留言" href="javascript:;" class="shut-new x-to-hide" style="display: none;"></a>
    <span class="headpichold">
        <a onclick="logRcd({action:'frtPage_recommend_R_imgToPrf',guest_id:{{ID}}})" href="http://www.renren.com/profile.do?id={{ID}}">
           <img width="50" height="50" onload="clipImage(this)" src="{{TINY_URL}}" />
        </a>
    </span>
    <span>
       <a onclick="logRcd({action:'frtPage_recommend_R_nameToPrf',guest_id:{{ID}}})" href="http://www.renren.com/profile.do?id={{ID}}" class="name">{{NAME}}</a>
       <p class="description">有段时间没联系了</p>
       <p>
           <a href="javascript:;" class="reconnect" onclick="XN.app.pymk.showCmt({{ID}});logRcd({action:'frtPage_recommend_R_leaveMsgIcon',guest_id:{{ID}}});">给TA留言吧</a>
       </p>
    </span>
    <div id="reconnect_{{ID}}" class="reconnect-gossip" style="display: none;"><textarea></textarea><div style="margin-top:3px;" class="clearfix"><label><input type="checkbox" id="whisper_{{ID}}" />悄悄话</label><input type="button" class="input-button" value="留言" onclick="XN.app.pymk.gossip({{ID}});logRcd({action:'frtPage_recommend_R_leaveMsgBtn',guest_id:{{ID}}});" /></div></div>
    <input type="hidden" value="{{TYPE_VALUE}}" id="type_value_{{ID}}" />
    
</li>

