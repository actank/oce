
package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.feed.FeedData;
import xce.feed.FeedDataRes;
import xce.feed.FeedViewPrx;
import xce.feed.FeedViewPrxHelper;
import xce.util.channel.Channel;

public class FeedViewHaAdapter extends ReplicatedClusterAdapter {

	private static FeedViewHaAdapter instance;
	
	private static final String ctr_endpoints = "ControllerFeedViewR";

	private static int _interval = 1;
	
	
	public static FeedViewHaAdapter getInstance() {
		if (instance == null) {
			instance = new FeedViewHaAdapter();
		}
		return instance;
	}

	private FeedViewHaAdapter() {
		super(ctr_endpoints, _interval, Channel.newFeedChannel("ControllerFeedViewR"));
		
	}
	


	private FeedViewPrx getManagerOneway(int uid) {
				
		Ice.ObjectPrx prx0 = getValidProxy(uid, 180);
		if (prx0 != null) {
			FeedViewPrx prx = FeedViewPrxHelper.uncheckedCast(prx0.ice_timeout(180).ice_oneway());
			return prx;
		} else {
			System.out.println("FeedView ha one way prx is null !");
			return null;
		}
	}

	private FeedViewPrx getManager(int uid) {
	
		Ice.ObjectPrx prx0 = getValidProxy(uid, 180);
		//System.out.println("FeedView prx:" + prx0);
		if (prx0 != null) {
			FeedViewPrx prx = FeedViewPrxHelper.uncheckedCast(prx0.ice_timeout(180).ice_twoway());
			return prx;
		} else {
			System.out.println("FeedView ha prx is null !");
			return null;
		}
	}

	// uid, name 始终为访问者的

	// 由调用者传入name，可以减少一次远程调用

	public List<String> getNews(int vistor, String vistorName, int view,
			int begin, int limit) {
		String[] html = getManager(vistor).Get(vistor, vistorName, view, begin,
				limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getNewsByType(int vistor, String vistorName, int view,
			int[] types, int begin, int limit) {
		String[] html = getManager(vistor).GetByType(vistor, vistorName, view,
				types, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getNewsByStype(int vistor, String vistorName, int view,
			int[] stypes, int begin, int limit) {
		String[] html = getManager(vistor).GetByStype(vistor, vistorName, view,
				stypes, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;

	}

	public List<String> getFeedByIds(int vistor, String vistorName,
			long[] feedIds) {
		String[] html = getManager(vistor)
				.GetByIds(vistor, vistorName, feedIds);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getFeedByIds2(int vistor, String vistorName, int view,
			long[] feedIds) {
		String[] html = getManager(vistor).GetByIds2(vistor, vistorName, view,
				feedIds);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getFeedByIdsAndStype(int vistor, String vistorName,
			long[] feedIds, int stype, long firstID) {
		String[] html = getManager(vistor)
				.GetByIdsAndStype(vistor, vistorName, feedIds, stype, firstID);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getFeedByIdsAndStype2(int vistor, String vistorName, int view,
			long[] feedIds, int stype, long firstID) {
		String[] html = getManager(vistor).GetByIdsAndStype2(vistor, vistorName, view,
				feedIds, stype, firstID);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	// owner 为被访问者 id

	public List<String> getMini(int owner, int vistor, String vistorName,
			int view, int begin, int limit) {
		String[] html = getManager(owner).GetMini(owner, vistor, vistorName,
				view, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getMiniByType(int owner, int vistor, String vistorName,
			int view, int[] types, int begin, int limit) {
		String[] html = getManager(owner).GetMiniByType(owner, vistor,
				vistorName, view, types, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getMiniByStype(int owner, int vistor,
			String vistorName, int view, int[] stypes, int begin, int limit) {
		String[] html = getManager(owner).GetMiniByStype(owner, vistor,
				vistorName, view, stypes, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getFavorite(int owner, String ownerName, int view,
			int begin, int limit) {
		String[] html = getManager(owner).GetFavorite(owner, ownerName, view,
				begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getHot(int owner, String ownerName, int view,
			int begin, int limit) {
		String[] html = getManager(owner).GetHot(owner, ownerName, view, begin,
				limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getHotByType(int owner, String ownerName, int view,
			int[] types, int begin, int limit) {
		String[] html = getManager(owner).GetHotByType(owner, ownerName, view,
				types, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getHotByStype(int owner, String ownerName, int view,
			int[] stypes, int begin, int limit) {
		String[] html = getManager(owner).GetHotByStype(owner, ownerName, view,
				stypes, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getLive(int owner, String ownerName, int view,
			int begin, int limit) {
		String[] html = getManager(owner).GetLive(owner, ownerName, view,
				begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getLiveByStype(int owner, String ownerName, int view,
			int[] stypes, int begin, int limit) {
		String[] html = getManager(owner).GetLiveByStype(owner, ownerName,
				view, stypes, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}

	public List<String> getLiveByType(int owner, String ownerName, int view,
			int[] types, int begin, int limit) {
		String[] html = getManager(owner).GetLiveByType(owner, ownerName, view,
				types, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
 
	public List<String> getMiniGroupFeedData(int gid, String name, int render,
			int memberid, int begin, int limit) {
		String[] html = getManager(gid).GetMiniGroupFeedData(gid, name, render,
				memberid, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getMiniGroupFeedDataByType(int gid, String name, int render,
			int memberid, int[] types, int begin, int limit) {
		String[] html = getManager(gid).GetMiniGroupFeedDataByType(gid, name, render,
				memberid, types, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getMiniGroupFeedDatabyStype(int gid, String name, int render,
			int memberid, int[] types, int begin, int limit) {
		String[] html = getManager(gid).GetMiniGroupFeedDataByStype(gid, name, render,
				memberid, types, begin, limit);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getMiniGroupFeedData2(int gid, String name, int render,
			int memberid, int begin, int limit, boolean isAdmin) {
		String[] html = getManager(gid).GetMiniGroupFeedData2(gid, name, render,
				memberid, begin, limit, isAdmin);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getMiniGroupFeedData2CS(int gid, String name, int render,
			int memberid, int begin, int limit, boolean isAdmin, int view) {
		String[] html = getManager(gid).GetMiniGroupFeedData2CS(gid, name, render,
				memberid, begin, limit, isAdmin, view);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getMiniGroupFeedDataByType2(int gid, String name, int render,
			int memberid, int[] types, int begin, int limit, boolean isAdmin) {
		String[] html = getManager(gid).GetMiniGroupFeedDataByType2(gid, name, render,
				memberid, types, begin, limit, isAdmin);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<String> getMiniGroupFeedDatabyStype2(int gid, String name, int render,
			int memberid, int[] types, int begin, int limit, boolean isAdmin) {
		String[] html = getManager(gid).GetMiniGroupFeedDataByStype2(gid, name, render,
				memberid, types, begin, limit, isAdmin);
		List<String> res = new ArrayList<String>();
		for (int i = 0; i < html.length; ++i) {
			res.add(html[i]);
		}
		return res;
	}
	
	public List<FeedData> getHotDataByStype(int uid, String name, int render, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotDataByStype(uid, name, render, types, begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getLiveDataByStype(int uid, String name, int render, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveDataByStype(uid, name, render, types, begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getMiniDataByStype(int uid, String name, int render, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] feed_data= getManager(uid).GetMiniDataByStype(uid, name, render, types, begin, limit);
		
		for(int i = 0 ; i < feed_data.length; ++i){
			list.add(feed_data[i]);
		}
		return list;
	}

	public List<FeedData> getHotDataByStype2(int uid, String name, int render, int[] types, int begin, int limit, int view){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotDataByStype2(uid, name, render, types, begin, limit, view);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getLiveDataByStype2(int uid, String name, int render, int[] types, int begin, int limit, int view){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveDataByStype2(uid, name, render, types, begin, limit, view);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getMiniDataByStype2(int uid, String name, int render, int[] types, int begin, int limit, int view){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] feed_data= getManager(uid).GetMiniDataByStype2(uid, name, render, types, begin, limit, view);
		
		for(int i = 0 ; i < feed_data.length; ++i){
			list.add(feed_data[i]);
		}
		return list;
	}
	public List<FeedData> getFavorite2(int uid, String name, int render, int begin, int limit, int view){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] feed_data= getManager(uid).GetFavorite2(uid, name, render, begin, limit, view);

		for(int i = 0 ; i < feed_data.length; ++i){
			list.add(feed_data[i]);
		}
		return list;
	}
	public List<FeedData> getFavorite2ByStype(int uid, String name, int render, int[] types, int begin, int limit, int view){
                List<FeedData> list = new ArrayList<FeedData>();
                FeedData[] feed_data= getManager(uid).GetFavorite2ByStype(uid, name, render, types, begin, limit, view);

                for(int i = 0 ; i < feed_data.length; ++i){
                        list.add(feed_data[i]);
                }
                return list;
        }
        public List<FeedData> getGreaterThanFeedidWithUseridByType(int appid, long feedid, int limit, int uid, int[] types, int render, int view) {
        	List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] feed_data= getManager(uid).GetGreaterThanFeedidWithUseridByType(appid, feedid, limit, uid, types, render, view);
		
		for(int i = 0 ; i < feed_data.length; ++i){
                        list.add(feed_data[i]);
                }   
                return list;
 	}
	
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//
//		FeedViewHaAdapter.getInstance().getHot(22509273, "ld", 0,
//   			0, 20);
//	}
			
			
}
