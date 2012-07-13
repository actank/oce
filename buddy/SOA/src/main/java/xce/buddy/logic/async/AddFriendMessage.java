package xce.buddy.logic.async;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.sql.SQLException;
import java.util.concurrent.TimeUnit;

import xce.buddy.FriendApply;

import com.renren.wap.friendaccept.listener.FriendAcceptFactory;
import com.xiaonei.email.service.control.ImmediateEmail;
import com.xiaonei.email.service.model.EmailConstants;
import com.xiaonei.notify.logic.NotifyLogic;
import com.xiaonei.platform.component.notify.view.Notify;
import com.xiaonei.platform.core.model.User;
import com.xiaonei.platform.core.opt.permission.define.Stage;
import com.xiaonei.platform.service.AppFriendsServiceHome;

/**
 * 在成功成为好友关系后,做一些后序的工作 <br/>
 * 发email,发Webpager等等
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class AddFriendMessage implements Message {

    private User applicantUser;

    private User accepterUser;

    private FriendApply friendApply;

    public AddFriendMessage(User applicantUser, User accepterUser, FriendApply friendApply) {
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
        }

        String siteName = "人人网";
        String domain = "renren.com";
        StringBuffer sb = new StringBuffer();
        sb.append(applicantUser.getName() + "你好： <br/>");
        try {
            sb.append(accepterUser.getName()
                    + "在"
                    + siteName
                    + "通过了你的好友申请，请用此链接查看："
                    + ImmediateEmail.getCountingUrl("http://" + domain + "/PLogin.do?domain="
                            + domain + "&origURL="
                            + URLEncoder.encode("http://" + domain + "/profile.do?id=", "UTF-8")
                            + accepterUser.getId(), mailDisc) + " <br/>");
        } catch (UnsupportedEncodingException e) {
            // TODO: Logger 一下
        }

        sb.append("感谢你对" + siteName + "的支持！<br/>");
        sb.append(siteName + " " + ImmediateEmail.getCountingUrl("http://www." + domain, mailDisc)
                + " <br/>");
        sb.append("---<br/>");
        sb
                .append("若你不想收到此类邮件通知，可登录"
                        + siteName
                        + "进行设置："
                        + ImmediateEmail.getCountingUrl("http://safe." + domain + "/setEmail.do",
                                mailDisc));

        String title = accepterUser.getName() + "在" + siteName + "上通过了你的好友申请";
        try {
            ImmediateEmail
                    .send(accepterUser, applicantUser, sb.toString(), title, mailDisc, domain);
        } catch (SQLException e) {
            // TODO: Logger 一下
        }
    }

    @Override
    public void run() {
        //先sleep一段时间,让Buddy服务把addFriend操作干完
        try {
            TimeUnit.SECONDS.sleep(1);
        } catch (InterruptedException e) {
            // TODO: Logger 一下
        }
        //发通知
        try {
            StringBuffer notifyContentBuffer = new StringBuffer();
            notifyContentBuffer.append("<a href=\"http://www.renren.com/profile.do?id=");
            notifyContentBuffer.append(accepterUser.getId()).append("\">");
            notifyContentBuffer.append(accepterUser.getName()).append("</a> 接受了你的好友请求");

            NotifyLogic.notify(applicantUser.getId(), Notify.TYPE_FRIEND, notifyContentBuffer
                    .toString());
        } catch (Exception e) {
            // TODO: Logger 一下
        }
        //发email
        try {
            SendEmail();
        } catch (Exception e) {
            // TODO: Logger 一下
        }
        try {
            FriendAcceptFactory.createFriendAcceptService().accept(accepterUser.getId(),
                    applicantUser.getId(), System.currentTimeMillis());
        } catch (Exception e) {

        }
        //app需要的通知(志才的需求 2010-11-01加入),在成功成为好友的时候 
        try {
            AppFriendsServiceHome.getInstance().becomeFriends(applicantUser.getId(),
                    accepterUser.getId());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
