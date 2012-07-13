package com.renren.xce.socialgraph.strategy;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;

import xce.socialgraph.util.Clogging;

import com.renren.xce.socialgraph.common.Category;
import com.renren.xce.socialgraph.common.Strategy;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.friendcountcache.FriendCountCacheAdapter;
import com.xiaonei.xce.scorecache.DefaultScoreCache;
import com.xiaonei.xce.scorecache.DefaultScoreCacheFactory;
import com.xiaonei.xce.scorecache.ScoreCacheAdapter;
import com.xiaonei.xce.socialgraph.userapply.UserApplyStatsAdapter;

/**
 * Get Stategy according to user's apply action
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class UserApplyStrategy implements Strategy{
	
	public List<Category> getStrategy(int userId) {
		List<Category> categoryList = new ArrayList<Category>();
		
		BitSet applyStats = new BitSet(31);
		int friendsCount = 0;
		int loginCount = 0;
		
		try {
			applyStats = getApplyStats(userId);		//user's status of applying
			friendsCount = getFriendsCount(userId);		//number of user's friends
			loginCount = getLoginCount(userId);		//number of user's login
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (applyStats.cardinality() == 0 && loginCount > 15		//no applying in a month, login > 15, friendsCount > 100
				&& friendsCount > 100) {
			Clogging.Debug("UserApplyStrategy use no applying strategy userId:" + userId);
			CommonCategory sameinfoFriends = new CommonCategory(CreateRecommendDataThread.INFO, 4, 10);
			categoryList.add(sameinfoFriends);
			
			CommonCategory friendCluster = new CommonCategory(CreateRecommendDataThread.FREINDS_CLUSTER, 4, 10);	
			categoryList.add(friendCluster);
			
			CommonCategory commonFreinds = new CommonCategory(CreateRecommendDataThread.COMMON_FRIENDS, 6, 10);
			categoryList.add(commonFreinds);
			
			CommonCategory page = new CommonCategory(CreateRecommendDataThread.PAGE, 4, 10);
			categoryList.add(page);
			
			CommonCategory video = new CommonCategory(CreateRecommendDataThread.RVIDEO, 9, 10);
			categoryList.add(video);
			
			CommonCategory blog = new CommonCategory(CreateRecommendDataThread.RBLOG, 9, 10);
			categoryList.add(blog);
		} else {
			Clogging.Debug("UserApplyStrategy use common strategy userId:" + userId);
			CommonCategory sameinfoFriends = new CommonCategory(CreateRecommendDataThread.INFO, 4, 10);
			categoryList.add(sameinfoFriends);
			
			CommonCategory friendCluster = new CommonCategory(CreateRecommendDataThread.FREINDS_CLUSTER, 12, 10);	
			categoryList.add(friendCluster);
			
			CommonCategory commonFreinds = new CommonCategory(CreateRecommendDataThread.COMMON_FRIENDS, 12, 10);
			categoryList.add(commonFreinds);
			
			CommonCategory page = new CommonCategory(CreateRecommendDataThread.PAGE, 2, 10);
			categoryList.add(page);
			
			CommonCategory video = new CommonCategory(CreateRecommendDataThread.RVIDEO, 3, 10);
			categoryList.add(video);
			
			CommonCategory blog = new CommonCategory(CreateRecommendDataThread.RBLOG, 3, 10);
			categoryList.add(blog);
		}
		
		return categoryList;
	}
	
	/**
	 * Get apply status of user
	 * @param userId
	 * @return
	 */
	private BitSet getApplyStats(int userId) {
		return UserApplyStatsAdapter.getInstance().getUserApplyStats(userId);
	}
	
	/**
	 * Get count of user's friends
	 * @param userId
	 * @return
	 */
	private int getFriendsCount(int userId) {
		return FriendCountCacheAdapter.getInstance().getFriendCount(userId);
	}
	
	/**
	 * Get count of login in month
	 * @param userId
	 * @return
	 */
	private int getLoginCount(int userId) {
		int count = 0;
		ScoreCacheAdapter<DefaultScoreCache> adapter = new ScoreCacheAdapter<DefaultScoreCache>(
				new DefaultScoreCacheFactory());
		DefaultScoreCache data = adapter.getScoreData(userId);
		List<Boolean> activeDaysList = data.getActiveDays();
		for (boolean each : activeDaysList) {
			if (each) {
				++count;
			}
		}
		return count;
	}
	
}
