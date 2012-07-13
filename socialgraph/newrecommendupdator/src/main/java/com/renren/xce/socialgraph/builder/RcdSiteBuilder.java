/*
 * 作者：熊君武
 * 邮件：junwu.xiong@renren-inc.com
 * 说明：本文件用于首页推荐之小站推荐
 * 时间：2011-12-22
 */

package com.renren.xce.socialgraph.builder;

import java.util.ArrayList;
import java.util.List;

import xce.socialgraph.recommend.minisite.RcdSiteData;
import xce.socialgraph.recommend.minisite.RcdSiteUtil;

import com.renren.xce.socialgraph.common.DataBuilder;
import com.renren.xce.socialgraph.common.DataItem;
import com.renren.xce.socialgraph.updator.CreateRecommendDataThread;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

public class RcdSiteBuilder implements DataBuilder {
	private static int SITE_NAME_LENGTH = 22; //minisite name length should be less than 11 chinese charactors
	
	public List<DataItem> getData(int hostId) {
		List<DataItem> resultList = new ArrayList<DataItem>();
		List<RcdSiteData> rcdSiteList = RcdSiteUtil.getInstance().getRcdSite(hostId, 30); 
		for (RcdSiteData data : rcdSiteList) {
			 String siteName = data.getName();
			 int nameLength = siteName.length() > SITE_NAME_LENGTH ? SITE_NAME_LENGTH : siteName.length();
			 String dispName = siteName.substring(0, nameLength);
			 if (nameLength != siteName.length()) {
				 dispName = dispName + "...";   //real displayed mini site name
			 }
			 
			Item.Builder item = Item.newBuilder(); 
			int siteId = data.getSiteId();      //rcd site's unique id  
			item.setId(siteId);  		
			item.setMessage("RCD_SITE");    	//msg stored with type identity
			//item.addMessages(dispName);  	//real displayed mini site name		 
			//item.addMessages(data.getHead());//site head 
			item.addMessages(data.getUrl()); //site url
			//item.addMessages(String.valueOf(data.getFollowerCount()) ); //site's attention count
			item.setField(data.getFollowerCount()); //site's attention count
			
			DefaultDataItem siteItem = new DefaultDataItem(siteId, item);
			resultList.add(siteItem);
		}
		
		return resultList;
 	}

	public String getBuilderType() {
		return CreateRecommendDataThread.RSITE;
	}
}
