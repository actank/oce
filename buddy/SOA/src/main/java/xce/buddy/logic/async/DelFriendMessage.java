package xce.buddy.logic.async;

import com.xiaonei.platform.component.feed.focus.DeleteFocusManager;
import com.xiaonei.platform.service.AppFriendsServiceHome;

public class DelFriendMessage implements Message {

	private int host;

	private int guest;

	public DelFriendMessage(int host, int guest) {
		this.host = host;
		this.guest = guest;
	}

	@Override
	public void run() {
		// app需要的通知(志才的需求 2010-11-01加入),在成功解除好友的时候
		try {
			AppFriendsServiceHome.getInstance().removeFriend(host, guest);
		} catch (Exception e) {
			e.printStackTrace();
		}

		// 删除关注, 康伟的需求
		try {
			DeleteFocusManager.delete(host, guest);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
