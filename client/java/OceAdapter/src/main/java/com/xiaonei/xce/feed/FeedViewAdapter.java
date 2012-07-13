package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedData;
import xce.feed.FeedDataRes;
import xce.feed.FeedViewPrx;
import xce.feed.FeedViewPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

@Deprecated
public class FeedViewAdapter extends
		AbstractModProxyHolder<Integer, FeedViewPrx> {

	private static FeedViewAdapter instance;
	private static String endpoints = "@FeedViewN";

	public static FeedViewAdapter getInstance() {
		if (instance == null) {
			instance = new FeedViewAdapter();
		}
		return instance;
	}

	public FeedViewAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED), "feedview"
				+ endpoints, 10);
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedViewPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedViewPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private FeedViewPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedViewPrx getManager(int uid) {
		return getProxy(uid);
	}

	// uid, name 始终为访问者的

	// 由调用者传入name，可以减少一次远程调用
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	@Deprecated
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
	
	
	
	
//	public static void main(String[] args) {
//	// TODO Auto-generated method stub
//    int[] arr = new int[0];
//	List<FeedData> res = FeedViewAdapter.getInstance().getNewsDataByStype(225092473, "刘迪", 0, arr,
//			0, 20);
//	FeedData fd = res.get(0);
//	System.out.println("fd:" + fd.feed + " size:" + res.size());
//}
			
			
}
