package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import xce.socialgraph.recommend.friendcluster.Recommend;
import xce.socialgraph.recommend.friendcluster.RecommendAdapter;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * cluster friends data builder
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class FriendClusterBuilder implements DataBuilder {
	private static int CACHE_SHARES_SIZE = 10;		//show 10 common friends
	
	public List<DataItem> getData(int hostId) {
		List<DataItem> clusterList = new ArrayList<DataItem>();
		List<Recommend> friendClusterList = RecommendAdapter.getInstance().getRecommendList(hostId);
		Collections.shuffle(friendClusterList);
		
		for (Recommend fc : friendClusterList) {
			Item.Builder item = Item.newBuilder();
			item.setId(fc.getRcdId());
			item.setField(fc.getCommonFriendCount());
			item.setMessage(CreateRecommendDataThread.FREINDS_CLUSTER);
			for (Integer shareId : fc.getCommonFriendList()) {
				if (item.getFieldsCount() >= CACHE_SHARES_SIZE) {
					break;
				}
				item.addFields(shareId);
			}
			DefaultDataItem clusterItem = new DefaultDataItem((long)fc.getRcdId(), item);
			clusterList.add(clusterItem);
		}
		return clusterList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.FREINDS_CLUSTER;
	}
}
