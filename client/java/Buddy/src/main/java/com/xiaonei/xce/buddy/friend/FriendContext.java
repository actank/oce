package com.xiaonei.xce.buddy.friend;

import java.util.List;
import java.util.Map;

import com.xiaonei.xce.buddy.BuddyContext;
import com.xiaonei.xce.userapicache.UserApiCache;
import com.xiaonei.xce.userapicache.UserApiCacheAdapter;
import com.xiaonei.xce.usercache.UserCache;
import com.xiaonei.xce.usercache.UserCacheAdapter;

/**
 * @author yuyang
 *
 */
public interface FriendContext extends BuddyContext {
	
	static enum SortType {
		BY_ID, BY_NAME, BY_ADD_TIME, BY_ON_LINE_TIME, BY_RANK;
	}
	
	/**
	 * 获取好友ID序列。
	 * 
	 * @param user
	 * @param begin
	 * @param limit
	 * @param sortType 排序方式
	 * @return
	 */
	public List<Integer> getFriendList(int userId, int begin, int limit,
			SortType sortType);
	
	/**
	 * 获取指定ID序列中的好友ID序列。
	 * 
	 * @param user
	 * @param ids
	 * @return
	 */
	public List<Integer> getFriendList(int userId, List<Integer> ids);
	

	/**
	 * 获取好友UserCache序列。
	 * 
	 * @param user
	 * @param begin
	 * @param limit
	 * @param sortType
	 * @return
	 */
	public <T extends UserCache> Map<Integer, T> getFriendList(int userId, int begin, int limit, UserCacheAdapter<T> adapter,
			SortType sortType);
	
	/**
	 * 获取好友UserCache序列。
	 * 
	 * @param user
	 * @param begin
	 * @param limit
	 * @param sortType
	 * @return
	 */
	
	public Map<Integer, UserApiCache> getFriendList(int userId, int begin, int limit, UserApiCacheAdapter adapter,
			SortType sortType);
}
