package com.xiaonei.talk;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import com.xiaonei.talk.muc.MucGateManagerPrx;
import com.xiaonei.talk.muc.MucGateManagerPrxHelper;
import com.xiaonei.talk.muc.MucRoomId;
import com.xiaonei.talk.muc.Permisions;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class MucGateAdapter extends ServiceAdapter {
	private static final String endpoints = "@MucGate";
	private Vector<MucGateManagerPrx> managersOneway = new Vector<MucGateManagerPrx>();;
	private Vector<MucGateManagerPrx> managersTwoway = new Vector<MucGateManagerPrx>();;

	private static MucGateAdapter instance = new MucGateAdapter();

	public static MucGateAdapter instance() {
		return instance;
	}

	MucGateAdapter() {
		super(endpoints, 10, Channel.newTalkChannel(endpoints));

	}

	public MucGateManagerPrx getManager(int id) {
		return locateProxy(managersTwoway, "M", id, Channel.Invoke.Twoway,
				MucGateManagerPrxHelper.class, 300);
	}
	
	public MucGateManagerPrx getManagerOneway(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Oneway,
				MucGateManagerPrxHelper.class, 300);
	}

	public Permisions Int2Permision(int p){
		if(1 == p){
			return Permisions.PMember;
		}
		else if(2 == p){
			return Permisions.PAdmin;
		}
		else if(3 == p){
			return Permisions.POwner;
		}
		else if(5 == p || 8 == p){
			return Permisions.PNone;
		}
		else{
			return Permisions.PMember;
		}
	}
	public void ChatSettingChange(int userid, int groupid, int set){
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		getManagerOneway(groupid).ChatSetingChange(userid, r, set);
	}
	public void CreateGroup(int groupid) {
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		getManagerOneway(groupid).CreateGroup(r);
	}
	public void CreateGroupForUgc(int groupowner, int groupid, int[] inviteusers, int invited_permision){
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		Permisions p = Int2Permision(invited_permision);
		getManagerOneway(groupowner).CreateGroupForUgc(groupowner, r, inviteusers, p);
	}
	public void RemoveGroup(int groupid){
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		getManagerOneway(groupid).RemoveGroup(r);
	}
	
	public void ReloadGroupInfo(int groupid, String groupname){
		getManagerOneway(groupid).ReloadGroupInfo(groupid, groupname);
	}
	public void AddGroupMember(int userid, int groupid, int permision){
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		Permisions p = Int2Permision(permision);
		getManagerOneway(userid).AddGroupMember(userid, r, p);
	}
	public void AddGroupMemberBySeq(int[] userids, int groupid, int permision){
		if(userids.length == 0){
			return;
		}
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		Permisions p = Int2Permision(permision);
		getManagerOneway(userids[0]).AddGroupMemberBySeq(userids, r, p);
	}
	public void RemoveGroupMember(int userid, int groupid){
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		getManagerOneway(userid).RemoveGroupMember(userid, r);
	}
	public void SetPermision(int operateuser, int groupid, int targetuser, int permision){
		MucRoomId r = new MucRoomId();
		r.domain = "group.talk.xiaonei.com";
		r.roomname = Integer.toString(groupid);
		Permisions p = Int2Permision(permision);
		getManagerOneway(operateuser).SetPermision(operateuser, r, targetuser, p);
	}

	public List<Integer> GetUserListByGroupId(int groupid){
		List<Integer> result = new ArrayList<Integer>();
		int [] v = getManager(groupid).GetUserListByGroupId(groupid);
		for(int i : v){
			result.add(i);
		}
		return result;
	}

	public List<Integer> GetGroupListByUserId(int userid){
		List<Integer> result = new ArrayList<Integer>();
		int [] v = getManager(userid).GetGroupListByUserId(userid);
		for(int i : v){
			result.add(i);
		}
		return result;
	}

}
