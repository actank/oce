package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import xce.socialgraph.recommend.recommendbyinfonew.InfoRecommendItem;
import xce.socialgraph.recommend.recommendbyinfonew.RecommendByInfoNewUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * Get recommendations of Same info which include school name
 * @author Administrator
 *
 */
public class NewSameInfoFriendsBuilder implements DataBuilder {

	public List<DataItem> getData(int hostId) {
		Set<Integer> filterSet = new HashSet<Integer>();
		List<DataItem> resultList = new ArrayList<DataItem>();
		
		List<InfoRecommendItem> univRecommends = RecommendByInfoNewUtil.getInstance().getUnivFriends(hostId);
		addResult(resultList, univRecommends, filterSet, "UNIV");
		
		List<InfoRecommendItem> highschoolRecommends = RecommendByInfoNewUtil.getInstance().getHighSchoolFriends(hostId);
		addResult(resultList, highschoolRecommends, filterSet, "HIGHSCHOOL");
		
		List<InfoRecommendItem> juniorschoolRecommends = RecommendByInfoNewUtil.getInstance().getJuniorSchoolFriends(hostId);
		addResult(resultList, juniorschoolRecommends, filterSet, "JUNIORSCHOOL");
		
		List<InfoRecommendItem> eleschoolRecommends = RecommendByInfoNewUtil.getInstance().getElementarySchoolFriends(hostId);
		addResult(resultList, eleschoolRecommends, filterSet, "ELESCHOOL");
		
		Collections.shuffle(resultList);		//random strategy
		return resultList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.INFO;
	}

	private void addResult(List<DataItem> resultList, List<InfoRecommendItem> sourceList,
			Set<Integer> filterSet, String type) {
		for (InfoRecommendItem it : sourceList) {
			if (filterSet.contains(it.getUserId())) {
				continue;
			}
			filterSet.add(it.getUserId());
			
			Item.Builder item = Item.newBuilder();
			item.setId(it.getUserId());
			item.setMessage(type);
			item.addMessages(it.getSchoolName());		//store school name in first position of messages
			
			DefaultDataItem infoItem = new DefaultDataItem((long)it.getUserId(), item);
			resultList.add(infoItem);
		}
	}
}
