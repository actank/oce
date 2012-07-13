package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import xce.socialgraph.recommend.commonfriends.CommonFriend;
import xce.socialgraph.recommend.commonfriends.CommonFriendsUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * common friends data builder
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class CommonFriendsBuilder implements DataBuilder {
	private static int CACHE_SHARES_SIZE = 10;		//show 10 common friends
	
	public List<DataItem> getData(int hostId) {
		List<DataItem> resultList = new ArrayList<DataItem>();
		List<CommonFriend> commList = CommonFriendsUtil.getInstance().getCommonFriends(hostId, -1);
		Collections.shuffle(commList);		//random strategy
		
		for (CommonFriend comm : commList) {
			Item.Builder item = Item.newBuilder();
			item.setId(comm.getUserId());
			item.setField(comm.getsharesCount());
			item.setMessage(CreateRecommendDataThread.COMMON_FRIENDS);
			for (Integer shareId : comm.getShares()) {
				if (item.getFieldsCount() >= CACHE_SHARES_SIZE) {
					break;
				}
				item.addFields(shareId);
			}
			
			DefaultDataItem commItem = new DefaultDataItem((long)comm.getUserId(), item);
			resultList.add(commItem);
		}
		return resultList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.COMMON_FRIENDS;
	}
}
