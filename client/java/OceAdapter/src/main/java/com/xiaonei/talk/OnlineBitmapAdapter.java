package com.xiaonei.talk;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;

import talk.onlinebitmap.OnlineBitmapManagerPrx;
import talk.onlinebitmap.OnlineBitmapManagerPrxHelper;
import talk.online.UserOnlineType;
//import talk.wevent.WEventManagerPrx;
//import talk.wevent.WEventManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class OnlineBitmapAdapter extends ServiceAdapter {
	private static final String endpoints = "@OnlineBitmap";
	private Vector<OnlineBitmapManagerPrx> managersOneway = new Vector<OnlineBitmapManagerPrx>();;

	private static OnlineBitmapAdapter instance = new OnlineBitmapAdapter();

	public static OnlineBitmapAdapter instance() {
		return instance;
	}

	OnlineBitmapAdapter() {
		super(endpoints, 0, Channel.newTalkChannel(endpoints));
	}
	
	public OnlineBitmapManagerPrx getManager(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Twoway,
				OnlineBitmapManagerPrxHelper.class, 300);
	}
	//返回 没有隐身登录的用户的id列表
	public List<Integer> getVisibleUsers(int [] uids){
		//List<UserOnlineType> list = new ArrayList<UserOnlineType>();
		List<Integer> list = new ArrayList<Integer>();
		UserOnlineType[] types =  getManager(0).getUserStatByIdSeq(uids);
		for(int i = 0 ;i < types.length; ++i){
			if(types[i].onlineType != 1){
				list.add(types[i].userId);
			}
		} 
		return list;
	}
	//判断用户是否是人人桌面隐身
	public boolean IfUserInvisible(int userid){
		UserOnlineType online_stat = new UserOnlineType();
		online_stat = getManager(0).getUserStat(userid);
		if(online_stat.onlineType == 1){
			return true;
		}
		return false;
	}
	//获取用户在线类型的批量接口
	public List<UserOnlineType> getUserOnlineTypeByIdSeq(int[] uids){
		List<UserOnlineType> list = new ArrayList<UserOnlineType>();
		UserOnlineType[] types = getManager(0).getUserTypeByIdSeq(uids);
		for(int i = 0; i < types.length; ++i){
			list.add(types[i]);
		}
		return list;
	}
	//获取在线用户的接口
	public List<UserOnlineType> getOnlineUsers(int[] uids){
		List<UserOnlineType> list = new ArrayList<UserOnlineType>();
		UserOnlineType[] types = getManager(0).getOnlineUsers(uids);
		for(int i = 0; i < types.length; ++i){
			list.add(types[i]);
		}
		return list;
	}
	public boolean isUserWebOnline(int userid){
		UserOnlineType type = getManager(0).getUserType(userid);
		if(type.onlineType == 2){
			return true;
		}
		return false;
	}
	public Map<Integer,Boolean> getWebOnlineUsers(int[] uids){
		Map<Integer, Boolean> m = new HashMap<Integer, Boolean>();
		List<UserOnlineType> list = new ArrayList<UserOnlineType>();
		UserOnlineType[] types = getManager(0).getUserTypeByIdSeq(uids);
		for(int i = 0; i < types.length; ++i){
			boolean b = false;
			if(types[i].onlineType == 2){
				b = true;
			}
			m.put(types[i].userId, b); 
		}
		return m;
	}
	//获取在线好友数量的接口
	public int getOnlineCount(int[] uids){
		return getManager(0).getOnlineCount(uids);
	}
}
