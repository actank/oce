package com.renren.xce.socialgraph.strategy;

import java.util.ArrayList;
import java.util.List;

import com.renren.xce.socialgraph.common.Category;
import com.renren.xce.socialgraph.common.Strategy;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;

/**
 * create general strategy
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class GeneralStrategy implements Strategy {
	private static int CF_SAVE_COMMON_SIZE = 20;		//20 common friends in recommendation
	private static int CF_SAVE_SAMEINFO_SIZE = 15;		//15 same info friends in recommendation
	private static int CF_SAVE_CLUSTER_SIZE = 15;		//15 friend cluster in recommendation
	
	/**
	 * get strategy
	 */
	public List<Category> getStrategy(int userId) {
		List<Category> categoryList = new ArrayList<Category>();
		try {
			categoryList = getDefaultStrategy();
		} catch (Exception e) {
			e.printStackTrace();
			return getDefaultStrategy();		//return default strategy
		}
		
		return categoryList;
	}
	
	/**
	 * get default strategy
	 * @return
	 */
	private List<Category> getDefaultStrategy() {
		List<Category> categoryList = new ArrayList<Category>();
		
		CommonCategory sameinfoFriends = new CommonCategory(
				CreateRecommendDataThread.INFO, CF_SAVE_SAMEINFO_SIZE, 10);
		categoryList.add(sameinfoFriends);
		
		CommonCategory friendCluster = new CommonCategory(				
				CreateRecommendDataThread.FREINDS_CLUSTER, CF_SAVE_CLUSTER_SIZE, 10);	
		categoryList.add(friendCluster);
		
		CommonCategory commonFreinds = new CommonCategory(
				CreateRecommendDataThread.COMMON_FRIENDS, CF_SAVE_COMMON_SIZE, 10);
		categoryList.add(commonFreinds);
		
		return categoryList;
	}

}
