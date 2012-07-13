package xce.buddy.logic.async;

import java.sql.SQLException;
import java.util.concurrent.TimeUnit;

import xce.buddy.FriendApply;

import com.xiaonei.email.service.control.ImmediateEmail;
import com.xiaonei.email.service.model.EmailConstants;
import com.xiaonei.platform.component.webpager.event.WebpagerEventNotify;
import com.xiaonei.platform.core.model.User;
import com.xiaonei.platform.core.opt.permission.define.Stage;
import com.xiaonei.xce.notify.NotifyAdapter;
import com.xiaonei.xce.notify.NotifyBody;

/**
 * 在成功发送好友申请后,做一些后序的工作 <br/>
 * 发email,发Webpager等等
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class AddFriendApplyMessage implements Message {

	private User applicantUser;

	private User accepterUser;

	private FriendApply friendApply;

	public AddFriendApplyMessage(User applicantUser, User accepterUser,
			FriendApply friendApply) {
		this.applicantUser = applicantUser;
		this.accepterUser = accepterUser;
		this.friendApply = friendApply;
	}

	private void SendEmail() {
		int mailDisc = 0;
		switch (accepterUser.getStage()) {
		case Stage.STAGE_HEIGHSCHOOL:
			mailDisc = EmailConstants.HIGHSCHOOL_REQUEST_FRIEND;
			break;
		case Stage.STAGE_UNIV:
			mailDisc = EmailConstants.UNIVERSITY_REQUEST_FRIEND;
			break;
		case Stage.STAGE_MAJOR:
			mailDisc = EmailConstants.WHITE_COLLAR_REQUEST_FRIEND;
			break;
		default:
			mailDisc = EmailConstants.WHITE_COLLAR_REQUEST_FRIEND;
			break;
		}

		String siteName = "人人网";
		String domain = "renren.com";
		StringBuffer sb = new StringBuffer();
		sb.append(accepterUser.getName() + "你好： <br/>");
		sb.append(applicantUser.getName()
				+ "想在"
				+ siteName
				+ "上加你为好友，请用此链接查看："
				+ ImmediateEmail.getCountingUrl("http://req." + domain
						+ "/request/requestList.do#nonapp_position_601",
						mailDisc) + " <br/>");
		sb.append("感谢你对" + siteName + "的支持！<br/>");
		sb.append(siteName
				+ " "
				+ ImmediateEmail.getCountingUrl("http://www." + domain,
						mailDisc) + " <br/>");
		sb.append("---<br/>");
		sb.append("若你不想收到此类邮件通知，可登录"
				+ siteName
				+ "进行设置："
				+ ImmediateEmail.getCountingUrl("http://safe." + domain
						+ "/setEmail.do", mailDisc));

		String title = applicantUser.getName() + "想在" + siteName + "上加你为好友";
		try {
			ImmediateEmail.send(applicantUser, accepterUser, sb.toString(),
					title, mailDisc, domain);
		} catch (SQLException e) {
			// TODO: Logger 一下
		}
	}

	@Override
	public void run() {

		// 先sleep一段时间,让Buddy服务把addApply操作干完
		try {
			TimeUnit.SECONDS.sleep(1);
		} catch (InterruptedException e) {
			// TODO: Logger 一下
		}

		// 发Email
		SendEmail();

		// 发Webpager
		String body = "<a href='http://www.renren.com/profile.do?id="
				+ applicantUser.getId()
				+ "'  target='_blank'>"
				+ applicantUser.getName()
				+ " </a>想与你<a href='http://req.renren.com"
				+ "/request/requestList.do#nonapp_position_601'  target='_blank'>加为好友</a>";
		WebpagerEventNotify.notify(accepterUser.getId(),
				WebpagerEventNotify.TYPE_FRIEND, "收到加好友请求", body);

		try {
			NotifyBody notify = new NotifyBody();
			notify.setSchemaId(36);
			notify.setFromId(applicantUser.getId());
			notify.setType(36);
			notify.setOwner(accepterUser.getId());
			notify.addToid(accepterUser.getId());
			notify.setValue("anchor", "601");
			notify.setSource(accepterUser.getId());
			notify.setValue("from_name", applicantUser.getName());
			notify.setTime(System.currentTimeMillis());
			NotifyAdapter.getInstance().dispatch(notify);
		} catch (Exception e) {
			// TODO: Logger 一下
		}
	}

}
