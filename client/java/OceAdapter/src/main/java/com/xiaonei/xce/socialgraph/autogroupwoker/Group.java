package com.xiaonei.xce.socialgraph.autogroupwoker;

import java.util.ArrayList;
import java.util.List;

/**
 * 存储小组的结构
 * @author zhangnan
 *
 */
public class Group {
	private List<String> labelList;
	private List<Integer> memberList;
	
	public Group() {
		labelList = new ArrayList<String>();
		memberList = new ArrayList<Integer>();
	}
	
	public void addLabel(String label) {
		labelList.add(label);
	}
	
	public void addMember(Integer id) {
		memberList.add(id);
	}
	
	public List<String> getGroupLabel() {
		return labelList;
	}
	
	public List<Integer> getGroupMember() {
		return memberList;
	}
}
