package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import xce.socialgraph.recommend.recommendbyinfo.RecommendByInfoUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
/**
 * same information friends builder
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class SameInfoFriendsBuilder implements DataBuilder {

	public List<DataItem> getData(int hostId) {
		Set<Integer> filterSet = new HashSet<Integer>();
		List<DataItem> resultList = new ArrayList<DataItem>();
		
		List<Integer> univRecommends = RecommendByInfoUtil.getInstance().getUnivFriends(hostId);
		addResult(resultList, univRecommends, filterSet, "UNIV");
		
		List<Integer> highschoolRecommends = RecommendByInfoUtil.getInstance().getHighSchoolFriends(hostId);
		addResult(resultList, highschoolRecommends, filterSet, "HIGHSCHOOL");
		
		List<Integer> juniorschoolRecommends = RecommendByInfoUtil.getInstance().getJuniorSchoolFriends(hostId);
		addResult(resultList, juniorschoolRecommends, filterSet, "JUNIORSCHOOL");
		
		List<Integer> eleschoolRecommends = RecommendByInfoUtil.getInstance().getElementarySchoolFriends(hostId);
		addResult(resultList, eleschoolRecommends, filterSet, "ELESCHOOL");
		
		Collections.shuffle(resultList);		//random strategy
		return resultList;
	}

	public String getBuilderType() {
		return CreateRecommendDataThread.INFO;
	}
	
	private void addResult(List<DataItem> resultList, List<Integer> sourceList,
			Set<Integer> filterSet, String type) {
		for (Integer sourceId : sourceList) {
			if (filterSet.contains(sourceId)) {
				continue;
			} 
			filterSet.add(sourceId);
			
			Item.Builder item = Item.newBuilder();
			item.setId(sourceId);
			item.setMessage(type);
			
			DefaultDataItem infoItem = new DefaultDataItem((long)sourceId, item);
			resultList.add(infoItem);
		}
	}
	
//  For test
//	private void PrintList(List<Integer> idList) {
//		String list = "";
//		for (Integer id : idList) {
//			list += id + " ";
//		}
//		Clogging.Debug("PrintList size:" + idList.size() + " PrintList:" + list);
//	}
//	
//	private void PrintSet(Set<Integer> filterSet) {
//		String list = "";
//		for (Integer id : filterSet) {
//			list += id + " "; 
//		}
//		Clogging.Debug("PrintSet size:" + filterSet.size() + " PrintSet:" + list);
//	}
//	
//	private void PrintDataItemList(List<DataItem> dataItemList) {
//		for (DataItem item : dataItemList) {
//			Clogging.Debug(item.getItem().getMessage() + " : " + item.getId());
//		}
//	}
}
