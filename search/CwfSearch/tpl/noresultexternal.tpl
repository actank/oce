
<div id="single-column"><!--single start-->
<div id="list-results">
<ol>
	
</ol>
</div>
 
	<div class="wbg_m20">
	<h1 class="searchico">对不起，没有找到相关的人名或内容!</h1>
	<h1 style="padding-left: 35px;">建议您：</h1>
	<ul class="search-suggest">
		<li>名字输入是否正确</li>
		<li>名字、条件间是否空格</li>
		<li>名字是否为纯拼音</li>
	</ul>
	<p style="padding-left: 35px;">您可以选择&nbsp;<a href="http://browse.renren.com/search.do">重新搜索</a>&nbsp;或者&nbsp;<a href="http://invite.renren.com/Invite.do" target="_blank">邀请好友</a>
	</p>
	</div>
 
<div class="import-msn-email clearfix">
	<div class="list">
		<ul id="search-list">
				<h1>你也可以试试查找MSN和邮箱联系人</h1>
			<li class="noline open">
				<div class="choose-box">
					<form method="post" name="importMail" id="importEmail" action="http://invite.renren.com/dispatch/inviteDispatch">
						<table>
							<tbody>
								<tr>
									<td class="name">
										邮箱帐号:
									</td>
									<td>
										<input type="text" id="msn-account" name="uAccount" class="input-text">
									</td>
									<td>
										@
										<select name="importer" id="importer_webmail">
											<option value="">
												请选择邮箱
											</option>
											<option value="@163.com">
												163.com
											</option>
											<option value="@126.com">
												126.com
											</option>
											<option value="@sina.com">
												sina.com
											</option>
											<option value="@hotmail.com">
												hotmail.com
											</option>
											<option value="@yahoo.com.cn">
												yahoo.com.cn
											</option>
											<option value="@gmail.com">
												gmail.com
											</option>
											<option value="@live.cn">
												live.cn
											</option>
											<option value="@live.com">
												live.com
											</option>
											<option value="@msn.com">
												msn.com
											</option>
											<option value="@msn.cn">
												msn.cn
											</option>
											<option value="@xnmsn.cn">
												xnmsn.cn
											</option>
											<option value="@yahoo.cn">
												yahoo.cn
											</option>
											<option value="@yahoo.com">
												yahoo.com
											</option>
										</select>
									</td>
								</tr>
								<tr>
									<td class="name">
										邮箱密码:
									</td>
									<td>
										<input type="password" id="msn-pwd" name="pwd" class="input-text">
									</td>
									<td>
										<span style="">人人网不会记录你的密码，请放心使用。 </span>
									</td>
								</tr>
								<tr>
									<td></td>
									<td>
										<input type="submit" onclick="startRequest('','search_invite_0_email')" class="input-submit" value="查找好友">
									</td>
								</tr>
							</tbody>
						</table>
					<input type="hidden" value="" name="requestToken"></form>
				</div>
			</li>
			<li class="noline open">
				<div class="choose-box">
					<form method="post" name="importMsn" id="importMsn" action="http://invite.renren.com/dispatch/inviteDispatch">
						<table>
							<tbody>
								<tr>
									<td class="name">
										MSN帐号:
									</td>
									<td>
										<input type="text" id="msn-account" name="uAccount" class="input-text">
									</td>
								</tr>
								<tr>
									<td class="name">
										MSN密码:
									</td>
									<td>
										<input type="password" id="msn-pwd" name="pwd" class="input-text">
									</td>
									<td>
										<span style="">人人网不会记录你的密码，请放心使用。</span>
									</td>
								</tr>
								<tr>
									<td></td>
									<td>
										<input type="hidden" name="importer" value="@hotmail.com">
										<input type="submit" onclick="startRequest('','search_invite_0_msn')" class="input-submit" value="查找好友">
									</td>
								</tr>
							</tbody>
						</table>
					<input type="hidden" value="" name="requestToken"></form>
				</div>
			</li>
		</ul>
	</div>
</div><!--import-msn-email end-->
<script type="text/javascript">	
		XN.app.search.simpleImportMsn()
		XN.app.search.simpleImportEmail()
</script>

</div>
<input type="hidden" value="1" id="outer">
<input type="hidden" value="0" id="resultNum">
<input type="hidden" value="" id="userNum">
<input type="hidden" value="" id="pageNum">
<input type="hidden" value="" id="appNum">
<input type="hidden" value="" id="searchExS">

<input type="hidden" value="{{ANTISPAM_RST}}" id="antispamRst">


	<input type="hidden" value="true" id="noResult">


<div style="display: none;" id="searchExQ">{{SEARCH_EX_Q}}</div>
<div style="display: none;" id="searchExP">{{SEARCH_EX_P}}</div>

