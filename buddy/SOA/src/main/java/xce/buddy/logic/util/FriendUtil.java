package xce.buddy.logic.util;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import mop.hi.oce.domain.buddy.BuddyRelation;

import com.xiaonei.platform.core.model.User;
import com.xiaonei.sns.platform.core.opt.ice.impl.SnsAdapterFactory;
import com.xiaonei.xce.buddyrelationcache.BuddyRelationCacheAdapter;

/**
 * 好友小工具
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class FriendUtil {

	public static boolean isFriend(int host, int guest) {
		try {
			BuddyRelation buddyRelation = BuddyRelationCacheAdapter
					.getInstance().getRelation(host, guest);
			return buddyRelation.isFriend();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	public static Map<Integer, Boolean> isFriend(int host, List<Integer> guests) {
		Map<Integer, Boolean> isFriendMap = new HashMap<Integer, Boolean>();
		for (Integer guest : guests) {
			try {
				BuddyRelation buddyRelation = BuddyRelationCacheAdapter
						.getInstance().getRelation(host, guest);
				if (buddyRelation.isFriend()) {
					isFriendMap.put(guest, true);
				} else {
					isFriendMap.put(guest, false);
				}
			} catch (Exception e) {
				continue;
			}
		}
		return isFriendMap;
	}

	private static final int STAR_USER_MAX_HOT_FRIEND_COUNT = 10;
	private static final int NOT_STAR_USER_MAX_HOT_FRIEND_COUNT = 6;

	public static int maxHotFriendCount(User user) {
		return user.isStarUser() ? STAR_USER_MAX_HOT_FRIEND_COUNT
				: NOT_STAR_USER_MAX_HOT_FRIEND_COUNT;
	}

}
