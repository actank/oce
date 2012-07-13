package com.xiaonei.xce.socialgraph.autogroupwoker;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
/**
 * 分组数据
 * @author zhangnan
 *
 */
public class GroupRecommend {
	private Map<String, List<Integer>> groupRecommend_ = new HashMap<String, List<Integer>>();
	
	public GroupRecommend(Items groupResult) {
		// TODO Auto-generated constructor stub
		for (Item r : groupResult.getItemsList()) {
			groupRecommend_.put(r.getMessage(), r.getFieldsList());
		}
	}
	
	public List<Integer> getGroupRecommend(String groupName) {
		if (groupRecommend_.containsKey(groupName)) {
			return groupRecommend_.get(groupName);
		}
		
		return null;
	}
	
	public Map<String, List<Integer>> getGroupRecommendMap() {
		return groupRecommend_;
	}
}
