package com.xiaonei.xce.buddy.friend.impl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.xiaonei.xce.buddy.friend.FriendContext;
import com.xiaonei.xce.buddyadapter.buddybyaddtimecache.BuddyByAddTimeCacheAdapter;
import com.xiaonei.xce.buddyadapter.buddybyidcache.BuddyByIdCacheAdapter;
import com.xiaonei.xce.buddyadapter.buddybynamecache.BuddyByNameCacheAdapter;
import com.xiaonei.xce.buddyadapter.buddybyonlinetimecache.BuddyByOnlineTimeCacheAdapter;
import com.xiaonei.xce.friendrank.FriendRankReplicaAdapter;
import com.xiaonei.xce.friendrank.Rank;
import com.xiaonei.xce.friendrank.RankFactory;
import com.xiaonei.xce.userapicache.UserApiCache;
import com.xiaonei.xce.userapicache.UserApiCacheAdapter;
import com.xiaonei.xce.usercache.UserCache;
import com.xiaonei.xce.usercache.UserCacheAdapter;

/**
 * @author yuyang
 *
 */
public class FriendContextImpl implements FriendContext {

	@Override
	public List<Integer> getFriendList(int userId, int begin, int limit,
			SortType sortType) {
		// TODO Auto-generated method stub

		switch (sortType) {
		case BY_ID: {
			return BuddyByIdCacheAdapter.getInstance().getFriendListAsc(userId,
					begin, limit);
		}
		case BY_NAME: {
			return BuddyByNameCacheAdapter.getInstance().getFriendList(userId, begin,
					limit);
		}
		case BY_ON_LINE_TIME: {
			return BuddyByOnlineTimeCacheAdapter.getInstance().getFriendList(userId,
					begin, limit);
		}
		case BY_ADD_TIME: {
			return BuddyByAddTimeCacheAdapter.getInstance().getFriendList(userId,
					begin, limit);
		}
		case BY_RANK: {
			FriendRankReplicaAdapter<Rank> adpater = new FriendRankReplicaAdapter<Rank>(
					new MyRankFactory());
			List<Integer> rankIds = adpater.getFriendIdSortByScore(userId);
			List<Integer> friends = BuddyByIdCacheAdapter.getInstance()
					.getFriendListAsc(userId, -1);
			rankIds.retainAll(friends);
			friends.removeAll(rankIds);
			rankIds.addAll(friends);
			if (begin >= rankIds.size() || begin < 0 || limit == 0 || limit < -1) {
				return new ArrayList<Integer>();
			}
			int toIndex = ((limit == -1) || (begin + limit > rankIds.size())) ? rankIds
					.size()
					: (begin + limit);
			return rankIds.subList(begin, toIndex);
		}
		default:
			break;
		}

		return new ArrayList<Integer>();
	}

	@Override
	public <T extends UserCache> Map<Integer, T> getFriendList(int userId,
			int begin, int limit, UserCacheAdapter<T> adapter, SortType sortType) {

		switch (sortType) {
		case BY_ID: {
			List<Integer> list = BuddyByIdCacheAdapter.getInstance()
					.getFriendListAsc(userId, begin, limit);
			return adapter.getUserCacheMap(list);
		}
		case BY_NAME: {
			List<Integer> list = BuddyByNameCacheAdapter.getInstance().getFriendList(
					userId, begin, limit);
			return adapter.getUserCacheMap(list);
		}
		case BY_ON_LINE_TIME: {
			List<Integer> list = BuddyByOnlineTimeCacheAdapter.getInstance()
					.getFriendList(userId, begin, limit);
			return adapter.getUserCacheMap(list);
		}
		case BY_ADD_TIME: {
			List<Integer> list = BuddyByAddTimeCacheAdapter.getInstance()
					.getFriendList(userId, begin, limit);
			return adapter.getUserCacheMap(list);
		}
		case BY_RANK: {
			return adapter.getUserCacheMap(getFriendList(userId, begin, limit,
					SortType.BY_RANK));
		}
		default:
			break;
		}
		return null;
	}

	@Override
	public Map<Integer, UserApiCache> getFriendList(int userId, int begin,
			int limit, UserApiCacheAdapter adapter, SortType sortType) {

		switch (sortType) {
		case BY_ID: {
			List<Integer> list = BuddyByIdCacheAdapter.getInstance()
					.getFriendListAsc(userId, begin, limit);
			return adapter.getUserApiCacheList(list);
		}
		case BY_NAME: {
			List<Integer> list = BuddyByNameCacheAdapter.getInstance().getFriendList(
					userId, begin, limit);
			return adapter.getUserApiCacheList(list);
		}
		case BY_ON_LINE_TIME: {
			List<Integer> list = BuddyByOnlineTimeCacheAdapter.getInstance()
					.getFriendList(userId, begin, limit);
			return adapter.getUserApiCacheList(list);
		}
		case BY_ADD_TIME: {
			List<Integer> list = BuddyByAddTimeCacheAdapter.getInstance()
					.getFriendList(userId, begin, limit);
			return adapter.getUserApiCacheList(list);
		}
		case BY_RANK: {
			return adapter.getUserApiCacheList(getFriendList(userId, begin, limit,
					SortType.BY_RANK));
		}
		default:
			break;
		}
		return null;
	}

	@Override
	public List<Integer> getFriendList(int userId, List<Integer> ids) {
		// TODO Auto-generated method stub
		List<Integer> idsAsc = BuddyByIdCacheAdapter.getInstance()
		.getFriendListAsc(userId, 0, -1);
		idsAsc.retainAll(ids);
		return idsAsc;
	}

}

class MyRank extends Rank {
}

class MyRankFactory implements RankFactory {
	public Rank create() {
		return new MyRank();
	}
}
