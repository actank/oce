package com.xiaonei.xce.socialfeedcache;

import java.util.ArrayList;
import java.util.List;

import xce.socialgraph.Feed;

public class FeedData {
	private List<Integer> friendIds = new ArrayList<Integer>();
	private List<Integer> newCommonIds = new ArrayList<Integer>();
	
	public FeedData(Feed result) {
		for(Integer o : result.friendIds) {
			friendIds.add(o);
		}
		for(Integer o : result.newCommonIds) {
			newCommonIds.add(o);
		}
	}
	
	public List<Integer> getFriendList() {
		return friendIds;
	}
	
	public List<Integer> getNewCommonList() {
		return newCommonIds;
	}
}
