package com.renren.xce.dao;

// import net.paoding.rose.jade.annotation.SQL;

import java.util.Date;
import java.util.HashSet;
import java.util.Set;
import java.util.List;

public class PromotionConfig {
	int id;
	int level;
	Date startTime;
	Date endTime;
	int minPartners;
	int maxDailyPost;

	int daily_posted_; // 当天已经发送量
	private Set<Integer> partners_ = new HashSet<Integer>();
	public void setPartners(List<Integer> partners) {
		partners_.addAll(partners);
		//for(int i = 0; i < partners.size(); ++i) {
		//  partners_.add(partners.get(i));
		//}
	}

	public boolean isPartner(int uid) {
		return partners_.contains(uid);
	}
	public void removePartner(int uid) {
		partners_.remove(uid);
	}
	  
	public PromotionConfig() {
		daily_posted_ = 0;
	}
	
	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}
	public int getLevel() {
		return level;
	}

	public void setLevel(int level) {
		this.level = level;
	}	
	
	public Date getStartTime() {
		return startTime;
	}
	public void setStartTime(Date d) {
		startTime = d;
	}


	public Date getEndTime() {
		return endTime;
	}
	public void setEndTime(Date d) {
		endTime = d;
	}	

	public int getMinPartners() {
		return minPartners;
	}

	public void setMinPartners(int min) {
		this.minPartners = min;
	}


	public int getMaxDailyPost() {
		return maxDailyPost;
	}

	public void setMaxDailyPost(int n) {
		this.maxDailyPost = n;
	}	

	
	public boolean UnderDailyLimit() {
	  if (maxDailyPost == 0 ) {
	      return true;
	  }
	  return maxDailyPost < daily_posted_;
	}
	
	public synchronized void IncDailyPosted() {
		++ daily_posted_;
	}
	public void resetDailyPosted() {
		daily_posted_ = 0;
	}
	
	public boolean Expired() {
	    return endTime.before(new Date());
	}
}
