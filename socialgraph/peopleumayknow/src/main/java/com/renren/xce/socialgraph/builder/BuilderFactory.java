package com.renren.xce.socialgraph.builder;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;

/**
 * BuilderFactory used to create different builder by type
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class BuilderFactory {
	public static DataBuilder createBuilder(String type) {
		if (type.equals(CreateRecommendDataThread.COMMON_FRIENDS)) {
			return new CommonFriendsBuilder();
		} else if (type.equals(CreateRecommendDataThread.FREINDS_CLUSTER)) {
			return new FriendClusterBuilder();
		}	else if (type.equals(CreateRecommendDataThread.INFO)) {
			return new NewSameInfoFriendsBuilder();
		}
		return null;
	}
}
