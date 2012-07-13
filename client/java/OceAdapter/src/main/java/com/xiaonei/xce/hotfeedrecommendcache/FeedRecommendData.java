package com.xiaonei.xce.hotfeedrecommendcache;

import xce.socialgraph.HotFeed;

public class FeedRecommendData {
	private int _userId;
	private long _feedId;
	private int _type;
	
	public FeedRecommendData(HotFeed result) {
		_userId = result.userId;
		_feedId = result.feedId;
		_type = result.feedType;
	}
	
	public int getUserId() {
		return _userId;
	}
	
	public long getFeedId() {
		return _feedId;
	}
	
	public int getFeedType() {
		return _type;
	}
	
	public boolean isMultiPhoto() {
		if (_type == 709) {   //multiphoto定义为709
			return true;
		}
		return false;
	}
	
	public boolean isSinglePhoto() {
		if (_type == 701) {         //singlephoto定义为701
			return true;
		}
		return false;
	}
	
	public boolean isBlog() {
		if (_type == 601) {         //blog定义为601
			return true;
		}
		return false;
	}
}
