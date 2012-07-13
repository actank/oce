package com.xiaonei.xcetest.tester;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.xiaonei.xce.follow.FollowAdapter;
import com.xiaonei.xce.follow.FollowType;
import junit.framework.TestCase;

public class FollowTest extends TestCase {
	
	public void add(){
		FollowAdapter.getInstance().addFollowing(1115, 1116, false);
		FollowAdapter.getInstance().addFollowing(1115, 1117, false);
		FollowAdapter.getInstance().addFollowing(1115, 1114, false);
		FollowAdapter.getInstance().addFollowing(1115, 1118, true);
		FollowAdapter.getInstance().addFollowing(1115, 1119, false);
	}
	public boolean equal(List<Integer> l, int[] a){
		if(l.size() != a.length){
			return false;
		}
		for(int i=0; i<l.size(); i++){
			if(l.get(i) != a[i]){
				return false;
			}
		}
		return true;
	}
	public boolean checkUser(int userid, int[] f, int[] fed, int[] a, 
									int[] aed, int[] b, int[] bed){
		List<Integer> l;
		l = FollowAdapter.getInstance().getFollowing(userid, 0, 100);
		if(!equal(l, f)){
			System.out.println("Following error");
			return false;
		}
		l = FollowAdapter.getInstance().getFollowed(userid, 0, 100);
		if(!equal(l, fed)){
			System.out.println("Followed error");
			return false;
		}
		l = FollowAdapter.getInstance().getApplying(userid, 0, 100);
		if(!equal(l, a)){
			System.out.println("Applying error");
			return false;
		}
		l = FollowAdapter.getInstance().getApplied(userid, 0, 100);
		if(!equal(l, aed)){	
			System.out.println("Applied error");
			return false;
		}
		l = FollowAdapter.getInstance().getBlock(userid, 0, 100);
		if(!equal(l, b)){
			System.out.println("Block error");
			return false;
		}
		return true;
	}
	public boolean checkAdd(){
		FollowAdapter.getInstance().addFollowing(1115, 1116, false);
		FollowAdapter.getInstance().addFollowing(1115, 1117, false);
		FollowAdapter.getInstance().addFollowing(1115, 1114, false);
		FollowAdapter.getInstance().addFollowing(1115, 1118, true);
		FollowAdapter.getInstance().addFollowing(1115, 1119, false);
		int[] f = {};
		int[] fed = {1114,1116,1117,1119};
		int[] a = {};
		int[] aed = {1118};
		int[] b = {};
		System.out.println("checking 1115");
		if(!checkUser(1115, f, fed, a, aed, b, b)){
			System.out.println("failed");
			return false;
		}
		
		int[] f2 = {1115};
		int[] fed2 = {};
		int[] a2 = {};
		int[] aed2 = {};
		int[] b2 = {};
		System.out.println("checking 1114");
		if(!checkUser(1114, f2, fed2, a2, aed2, b2, b2)){
			System.out.println("failed");
			return false;
		}
		
		int[] f3 = {1115};
		int[] fed3 = {};
		int[] a3 = {};
		int[] aed3 = {};
		int[] b3 = {};
		System.out.println("checking 1116");
		if(!checkUser(1116, f3, fed3, a3, aed3, b3, b3)){
			System.out.println("failed");
			return false;
		}
		
		int[] f4 = {1115};
		int[] fed4 = {};
		int[] a4 = {};
		int[] aed4 = {};
		int[] b4 = {};
		System.out.println("checking 1116");
		if(!checkUser(1116, f4, fed4, a4, aed4, b4, b4)){
			System.out.println("failed");
			return false;
		}
		return true;
	}
	public void checkF(){
		List<Integer> l;
		l = FollowAdapter.getInstance().getFollowed(1115, 0, 100);
		System.out.println("followed" + l);
		l = FollowAdapter.getInstance().getFollowing(1114, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1116, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1117, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1118, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1119, 0, 100);
		System.out.println("following" + l);
		System.out.println("");
	}
	public void displayF(){
		List<Integer> l;
		l = FollowAdapter.getInstance().getFollowed(1115, 0, 100);
		System.out.println("followed" + l);
		l = FollowAdapter.getInstance().getFollowing(1114, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1116, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1117, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1118, 0, 100);
		System.out.println("following" + l);
		l = FollowAdapter.getInstance().getFollowing(1119, 0, 100);
		System.out.println("following" + l);
		System.out.println("");
	}
	public void displayA(){
		List<Integer> l;
		l = FollowAdapter.getInstance().getApplied(1115, 0, 100);
		System.out.println("applied" + l);
		l = FollowAdapter.getInstance().getApplying(1114, 0, 100);
		System.out.println("applying" + l);
		l = FollowAdapter.getInstance().getApplying(1116, 0, 100);
		System.out.println("applying" + l);
		l = FollowAdapter.getInstance().getApplying(1117, 0, 100);
		System.out.println("applying" + l);
		l = FollowAdapter.getInstance().getApplying(1118, 0, 100);
		System.out.println("applying" + l);
		l = FollowAdapter.getInstance().getApplying(1119, 0, 100);
		System.out.println("applying" + l);
		System.out.println("");
	}
	public void displayB(){
		List<Integer> l;
		l = FollowAdapter.getInstance().getBlock(1115, 0, 100);
		System.out.println("Block" + l);
		l = FollowAdapter.getInstance().getBlock(1114, 0, 100);
		System.out.println("Block" + l);
		l = FollowAdapter.getInstance().getBlock(1116, 0, 100);
		System.out.println("Block" + l);
		l = FollowAdapter.getInstance().getBlock(1117, 0, 100);
		System.out.println("Block" + l);
		l = FollowAdapter.getInstance().getBlock(1118, 0, 100);
		System.out.println("Block" + l);
		l = FollowAdapter.getInstance().getBlock(1119, 0, 100);
		System.out.println("Block" + l);
		System.out.println("");
	}
	public void display(int userid){
		for(int i=0; i<10; i++){
			System.out.println("******"+userid);
			List<Integer> l;
			l = FollowAdapter.getInstance().getFollowing(userid, 0, 100);
			System.out.print(l);
			l = FollowAdapter.getInstance().getFollowed(userid, 0, 100);
			System.out.println(" " + l);
			l = FollowAdapter.getInstance().getApplying(userid, 0, 100);
			System.out.print(l);
			l = FollowAdapter.getInstance().getApplied(userid, 0, 100);
			System.out.println(" " + l);
			l = FollowAdapter.getInstance().getBlock(userid, 0, 100);
			System.out.println(l);
			System.out.println("");
		}
	}
	public void displayAll(){
		display(1114);
		display(1115);
		display(1116);
		display(1117);
		display(1118);
		display(1119);
	}
/*	void count(){
		getFollowingCount(int subscriber)
		getFollowedCount(int publisher)
		getApplyingCount(int subscriber)
		getAppliedCount(int publisher)
		getBlockCount(int publisher)
		getRelation(int publisher, int subscriber)
	}*/
	public void remove(){
		FollowAdapter.getInstance().cancelFollowing(1115, 1119);
	}
	public void block(){
		FollowAdapter.getInstance().blockFollowed(1115, 1119);
	}
	/*public void accept(){
		FollowAdapter.getInstance().cancelFollowing(1115, 1119);
	}*/
	
	public void test1() throws Exception
	{ 
//		display(702613154);
		//checkAdd();
		/*List<Integer> l;
		l = FollowAdapter.getInstance().getFollowed(1115, 1, 2);
		System.out.println("~~~" + l);*/
		
		//add();
		//displayAll();
		/*displayF();
		displayA();
		displayB();*/
		//FollowType res = new FollowType();
//		FollowAdapter.getInstance().addFollowing(700035999, 700002970, false);

//		FollowAdapter.getInstance().blockFollowed(700035999, 700002970);
//		FollowAdapter.getInstance().addFollowing(700035999, 700002970, false);
//		System.out.println(FollowAdapter.getInstance().getRelation(700035999, 700002970));
//		System.out.println(FollowAdapter.getInstance().getRelation(700002970,700035999));
//		
		
		
		//int res = FollowAdapter.getInstance().addFollowing(1115, 1114, false);
		//FollowAdapter.getInstance().addFollowing(1115, 1117, true);
		//FollowAdapter.getInstance().addFollowing(1117, 1115, false);
		//FollowAdapter.getInstance().addFollowing(1118, 1115, false);
		//FollowAdapter.getInstance().addFollowing(1117, 1115, false);
		//FollowAdapter.getInstance().cancelBlocked(1115, 1119);
		//FollowAdapter.getInstance().cancelBlocked(1115, 1118);
		//FollowAdapter.getInstance().cancelBlocked(1115, 1117);
		//FollowAdapter.getInstance().cancelFollowing(1117, 1115);
		//FollowAdapter.getInstance().acceptApply(1115, 1118);
		//FollowAdapter.getInstance().refuseApply(1115, 1118);
		//displayAll();
		//FollowAdapter.getInstance().blockFollowed(1115, 1117);
		//System.out.println("*********");
		//System.out.println(FollowAdapter.getInstance().getRelation(1115, 1114));
		//displayAll();
		/*int[] a = {1117, 1118, 12321, 1114};
		List<Integer> aa = new ArrayList<Integer>();
		for(int i=0; i<a.length; i++){
			aa.add(a[i]);
		}
		System.out.println(aa.size());
		Map<Integer, Integer> m = FollowAdapter.getInstance().getRelations(1115, aa);
		for(Entry<Integer, Integer> e:m.entrySet()){
			System.out.println(e.getKey() + " " + e.getValue());
		}*/
		
		/*displayF();
		displayA();
		displayB();*/
		//block();
		//remove();
		//displayF();
		//displayA();
		//Map<Integer, Integer> map = adapter.getFriendDist(128487631);
	
		//addFollowing(int publisher, int subscriber, boolean needApply);
		/*FollowAdapter.getInstance().addFollowing(1115, 1116, false);
		FollowAdapter.getInstance().addFollowing(1115, 1117, false);
		FollowAdapter.getInstance().addFollowing(1115, 1114, false);
		FollowAdapter.getInstance().addFollowing(1115, 1118, false);
		*/
		/*FollowAdapter.getInstance().addFollowing(1115, 1119, true);
		List<Integer> l;*/
		/*l = FollowAdapter.getInstance().getFollowed(1115, 0, 100);
		System.out.println(l);*/
		
		//FollowAdapter.getInstance().cancelFollowing(1115, 1118);
/*		l = FollowAdapter.getInstance().getFollowed(1115, 0, 100);
		System.out.println(l);
		l = FollowAdapter.getInstance().getFollowing(1114, 0, 100);
		System.out.println(l);
		l = FollowAdapter.getInstance().getFollowing(1116, 0, 100);
		System.out.println(l);
		l = FollowAdapter.getInstance().getFollowing(1117, 0, 100);
		System.out.println(l);
		l = FollowAdapter.getInstance().getFollowing(1118, 0, 100);
		System.out.println(l);*/

	}
//	public void testUserCount()
//	{
//		FollowAdapter.getInstance().getFollowing(700035999, 0, 1);
//		System.out.print(com.xiaonei.xce.usercount.UserCountAdapter.getInstance().get(700035999, 27));
//	}
}
