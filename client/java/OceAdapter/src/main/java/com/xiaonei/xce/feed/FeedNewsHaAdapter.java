
package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.feed.FeedData;
import xce.feed.FeedDataRes;
import xce.feed.FeedItem;
import xce.feed.FeedNewsManagerPrx;
import xce.feed.FeedNewsManagerPrxHelper;
import xce.util.channel.Channel;

public class FeedNewsHaAdapter extends ReplicatedClusterAdapter {

	private static FeedNewsHaAdapter instance;
	
	private static final String ctr_endpoints = "ControllerFeedNewsR";

	private static int _interval = 1;
	
	
	public static FeedNewsHaAdapter getInstance() {
		if (instance == null) {
			instance = new FeedNewsHaAdapter();
		}
		return instance;
	}

	private FeedNewsHaAdapter() {
		super(ctr_endpoints, _interval, Channel.newFeedChannel("ControllerFeedNewsR"));
		
	}
	


	private FeedNewsManagerPrx getManagerOneway(int uid) {
				
		Ice.ObjectPrx prx0 = getValidProxy(uid, 180);
		if (prx0 != null) {
			FeedNewsManagerPrx prx = FeedNewsManagerPrxHelper.uncheckedCast(prx0.ice_timeout(260).ice_oneway());
			return prx;
		} else {
			System.out.println("FeedNewsManager ha one way prx is null ! ");
			return null;
		}
	}

	private FeedNewsManagerPrx getManager(int uid) {
	
		Ice.ObjectPrx prx0 = getValidProxy(uid, 180);
		
		if (prx0 != null) {
			FeedNewsManagerPrx prx = FeedNewsManagerPrxHelper.uncheckedCast(prx0.ice_timeout(260).ice_twoway());
			return prx;
		} else {
			System.out.println("FeedNewsManager ha prx is null ! ");
			return null;
		}
	}
	
	
	public List<FeedData> getLiveFeedByStype(int uid, int[] stypes, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveFeedDataByStype(uid, stypes,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getHotFeedByStype(int uid, int[] stypes, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotFeedDataByStype(uid, stypes,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getLiveFeedByType(int uid, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveFeedDataByType(uid, types,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	

	public List<FeedData> getHotFeedByType(int uid, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotFeedDataByType(uid, types,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getLiveFeed(int uid, int begin, int limit){
		
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveFeedData(uid,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	

	public List<FeedData> getHotFeed(int uid, int begin, int limit) {

		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotFeedData(uid, begin,
				limit);
		FeedData[] contents = feed_data_res.data;
		for (int i = 0; i < contents.length; ++i) {
			list.add(contents[i]);
		}
		return list;
	}

	
	public List<FeedData> getFeedDataByIds(long[] feedIds){
		List<FeedData> list = new ArrayList<FeedData>();
		if(feedIds.length == 0){
			return list;
		}
		FeedData [] contents  = getManager((int)(feedIds[0]% 10000)).GetFeedDataByIds(feedIds);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedItem>  getOriginalFeedData(int uid) {
		List<FeedItem> list = new ArrayList<FeedItem>();
		
		FeedItem [] items = getManager(uid).GetOriginalFeedData(uid);
		for(int i = 0 ; i < items.length; ++i){
			list.add(items[i]);
		}
		return list;
	}
	
	public int  getUnviewedCount(int uid, int type) {
			
		return  getManager(uid).GetUnviewedCount(uid, type);
	
	}
	
	public Map<Integer,Integer>  getCountMapByStype(int uid, int[] stypes, boolean unviewed) {
		return getManager(uid).GetCountMapByStype(uid, stypes,unviewed);		 
	}

//	public static void main(String[] args) {
//		int [] stypes = {601,502};
//		Map< Integer, Integer > res = FeedNewsHaAdapter.getInstance().getCountMapByStype(220678672, 
//				stypes, false);
//		for (Map.Entry< Integer, Integer > entry : res.entrySet()) {
//			System.out.println("stype:" + entry.getKey() + ", name="
//					+ entry.getValue());
//		}
//	}
	
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//
//		List<FeedData> res = FeedNewsHaAdapter.getInstance().getHotFeed(225092473, 
//   			0, 20);
//		FeedData fd = res.get(0);
//		System.out.println("fd:" + fd.feed + " size:" + res.size());
//	}
			
			
}
