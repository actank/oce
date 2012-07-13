package com.xiaonei.xce.feed;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import xce.feed.FeedAssistantPrx;
import xce.feed.FeedAssistantPrxHelper;
import xce.feed.FeedReply;
import xce.feed.FeedSeed;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

class  FeedAssistantAdapter extends AbstractModProxyHolder<Integer, FeedAssistantPrx>{

	private static FeedAssistantAdapter instance;
	private static String endpoints = "@FeedAssistant";

	public FeedAssistantAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"FA"+endpoints, 1);
	}

	public static FeedAssistantAdapter getInstance() {
		if (instance == null) {
			instance = new FeedAssistantAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedAssistantPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedAssistantPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private FeedAssistantPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedAssistantPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	public void dispatch(Feed feed) throws Exception{
		FeedSeed seed = feed.getFeedSeed();
		getManagerOneway(0).dispatch(seed);
	}
	
	public void dispatchAgain(long source, int stype , int actor, int toid){
		getManagerOneway(stype).dispatchAgain(source, stype, actor, toid);
	}
	
	public void dispatch(Feed feed,FeedReply reply) throws Exception{
		FeedSeed seed = feed.getFeedSeed();
		getManagerOneway(0).dispatchWithReply(seed,reply);
	}
	
	public void dispatchEDM(Feed feed)throws Exception{
		FeedSeed seed = feed.getFeedSeed();
		getManagerOneway(0).dispatchEDM(seed);
	}
	
	public void addFeedReply(long source, int stype, int actor, FeedReply reply){
		getManagerOneway(0).addFeedReply(source, stype, actor, reply);
	}

        /*	
	public void updateFeedReply(long source, int stype, int actor,
			Map<String, String> newReplyMap, Map<String, String> oldReplyMap,
			long replyId, boolean isDel){
		System.out.println("begin");
		try {
		getManagerOneway(0).updateFeedReply(source, stype, actor, newReplyMap, oldReplyMap, replyId, isDel);
		} catch (Exception e) {
			System.out.println(e);
		}
		System.out.println("end");
	}
        */
	
	public void addFeedReply(int stype, long feedId, FeedReply reply){
		getManagerOneway(0).addFeedReplyById(stype,feedId,reply);
	}
	
	public void addFeedReplyWithConfig(long source, int stype, int actor, 
			FeedReply reply, Map<String, String> replyConfig){
		getManagerOneway(0).addFeedReplyWithConfig(source, stype, actor, reply, replyConfig);
	}

	
//	public void addFeedReply(long feed, long replyId, String reply){
//		getManagerOneway(0).addFeedReplyByFeedId(feed, replyId, reply);
//	}
	public void removeFeedById(long feedId){
		getManagerOneway(0).removeFeedById(feedId);
	}
	public void removeFeedBySource(long source, int stype){
		getManagerOneway(0).removeFeedBySource(source, stype);
	}
	public void removeFeedByParent(long parent, int ptype){
		getManagerOneway(0).removeFeedByParent(parent, ptype);
	}
	public void removeFeed(long source, int stype, int actor){
		getManagerOneway(0).removeFeed(source, stype, actor);
	}
	
	public void removeFeedReply(long source, int stype, int actor,long removeReplyId, FeedReply reply){
		getManagerOneway(0).removeFeedReply(source, stype, actor, removeReplyId,reply);
	}
	public void removeFeedReply(int stype, long feedid, long removeReplyId, FeedReply reply){
		getManagerOneway(0).removeFeedReplyById(stype, feedid, removeReplyId,reply);
	}

	public void readNews(int user, long feedid){
		getManagerOneway(0).readNews(user, feedid);
	}
	public void readAllNews(int user){
		getManagerOneway(0).readAllNews(user);
	}
	public void readMini(int user, long feedid){
		getManagerOneway(0).readMini(user, feedid);
	};
	public void readAllMini(int user){
		getManagerOneway(0).readAllMini(user);
	};
	
	public void ReplaceXML(long source, int stype, int actor,String xml){
		getManagerOneway(0).ReplaceXML(source,stype,actor,xml);
	};
	
	public void DirectedSend(long source, int stype, int actor,int[] targets){
		getManagerOneway(0).DirectedSend(source,stype,actor,targets);
	};
	public static void main(String[] args) {
		FeedReply rf = new FeedReply();
		FeedAssistantAdapter.getInstance().addFeedReply(2180336678L, 502, 327580522, rf);
		// TODO Auto-generated method stub
		Map<String,String> newmap=new HashMap();
		Map<String,String> oldmap=new HashMap();
//		fr.SetNewReplyBody("xxxxxxxxxxx");
//		fr.SetNewReplyFromId(225092473L);
//		fr.SetNewReplyFromName("ddddd");
//		fr.SetNewReplyId(33333333);
//		fr.SetNewReplyTime(new Date());
//		fr.SetReplyCount(1);
		//FeedAssistantAdapter.getInstance().updateFeedReply(2180336678L, 502, 327580522, newmap, oldmap, 33333333, false);
	     return ;
	}
}
