package com.xiaonei.xce.socialgraph.autogroupwoker;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Set;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
/**
 * 本地分组数据
 * @author zhangnan
 *
 */
public class GroupManager {
	private Map<Integer, Group> groupRecommend;
	private Map<String, List<Integer>> labelIndex;
	private Items groupResult;
	
	public GroupManager(Items groupResult) {
		this.groupResult = groupResult;
		groupRecommend = new HashMap<Integer, Group>();
		labelIndex = new HashMap<String, List<Integer>>();
	}
	
	/**
	 * 初始化本地结构
	 */
	public void initialize() {
		int index = 1;
		int undef = 1;
		for (Item r : groupResult.getItemsList()) {		//用Item初始化本地group结构
			Group gitem = new Group();
			if (r.getMessagesList().isEmpty()) {
				gitem.addLabel("未命名" + undef++);			//没有打上tag的组命名为"未命名 + index"
			} else {
				String labeIndexName = "";
				for (String label : r.getMessagesList()) {
					gitem.addLabel(label);		//向Group结构中输入label列表
					labeIndexName += label;		//初始化查找列表
					if (labelIndex.containsKey(labeIndexName)) {
						labelIndex.get(labeIndexName).add(index);
					} else {
						List<Integer> newIndexList = new ArrayList<Integer>();
						newIndexList.add(index);
						labelIndex.put(labeIndexName, newIndexList);
					}
				}
			}
			for (Integer id : r.getFieldsList()) {
				gitem.addMember(id);
			}
			groupRecommend.put(index++, gitem);
		}
	}
	
	/**
	 * 获取全部组推荐
	 * @return
	 */
	public Map<Integer, Group> getGroupMap() {
		return groupRecommend;
	}
	
	/**
	 * 根据提供的资料获取相应的推荐列表
	 * @param condition	过滤的条件
	 * @return
	 */
	public List<Integer> getGroupMember(FliterCondition condition) {
		List<String> searchLabelList = new ArrayList<String>();
		String tag = "";
		if (condition.getSchoolName() != "") {
			tag += condition.getSchoolName();			//学校
			searchLabelList.add(tag);
		}
		
		if (condition.getAcademe() != "") {
			tag += condition.getAcademe();		//学校 ＋ 学院
			searchLabelList.add(tag);
		}
		
		if (condition.getEnrollYear() != 0) {
			tag += String.valueOf(condition.getEnrollYear());		//学校 ＋ 学院 ＋ 入学年
			searchLabelList.add(tag);
		}
		
		List<Integer> resultList = new ArrayList<Integer>();
		ListIterator<String> iter = searchLabelList.listIterator();
		while (iter.hasNext()) {		//一定要先把指针指向最后？？
			iter.next();
		}
		while (iter.hasPrevious()) {	//从后先前匹配，先精确后模糊
			resultList = getProperIds(iter.previous());
			if (!resultList.isEmpty()) {
				break;
			}
		}
		return resultList;
	}
	
	/**
	 * 寻找最适合的组成员方法,提供模糊匹配
	 * @param label
	 * @return
	 */
	private List<Integer> getProperIds(String label) {
		List<Integer> idList = new ArrayList<Integer>();
		if (labelIndex.containsKey(label)) {
			Set<Integer> newMember = new HashSet<Integer>();
			List<Integer> indexList = labelIndex.get(label);
			for (Integer index : indexList) {
				Group group = groupRecommend.get(index);
				if (null != group) {
					for (Integer id : group.getGroupMember()) {
						newMember.add(id);
					}
				}
			}
			idList.addAll(newMember);
		}
		return idList;
	}
}
