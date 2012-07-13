package com.xiaonei.xce.test.buddycorelogic;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;

import junit.framework.Assert;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
import mop.hi.oce.domain.buddy.BuddyApplyWithInfo;
import mop.hi.oce.domain.buddy.BuddyRelation;
import mop.hi.svc.model.FriendDistManagerPrx;
import mop.hi.svc.model.FriendDistManagerPrxHelper;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

@RunWith(Parameterized.class)
public class BuddyCoreLogicTestCase {	
	
	private int userId1, userId2;	
	BuddyCoreAdapterImpl _adapter = new BuddyCoreAdapterImpl();	
	FriendDistAdatper _adapterDist = FriendDistAdatper.getInstance();
	

	public BuddyCoreLogicTestCase(int userId1, int userId2) {
		this.userId1 = userId1;
		this.userId2 = userId2;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000003, 700000001},		//测试第一组
		      	{700000004, 700000002},		//测试第二组
		});
	}
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {  
	}
	@Before
	public void setUpBefore() throws Exception {
	}
	@After
	public void tearDownAfter() throws Exception {  
	}
	
	@Test
	public void Test()  {
		
		BuddyApplyWithInfo request1 = new BuddyApplyWithInfo(userId1, userId2);
		
		// 输出初始状态
		System.out.println(" --- No Relation : " + String.valueOf(userId1) + " and " + String.valueOf(userId2) + " --- ");
		int dist0 = _adapterDist.getFriendDistManager(userId2).getTotalCount(userId2);
		int buddy0 = _adapter.getBuddyCount(userId2);				
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + String.valueOf(dist0));
		Map<Integer, Integer> dist = _adapterDist.getFriendDistManager(userId2).getFriendDist(userId2);
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + dist.toString());
		System.out.println("Buddy " + String.valueOf(userId2) + " : " + String.valueOf(buddy0));		
				
		// 添加好友关系
		_adapter.addApply(request1);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}	
		_adapter.acceptApply(request1);
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		System.out.println(" --- Add Friend Relation : " + String.valueOf(userId1) + " and " + String.valueOf(userId2) + " --- ");
		int dist1 = _adapterDist.getFriendDistManager(userId2).getTotalCount(userId2);
		int buddy1 = _adapter.getBuddyCount(userId2);				
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + String.valueOf(dist1));
		dist = _adapterDist.getFriendDistManager(userId2).getFriendDist(userId2);
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + dist.toString());
		System.out.println("Buddy " + String.valueOf(userId2) + " : " + String.valueOf(buddy1));		
		
		Assert.assertEquals(dist1-dist0, buddy1-buddy0);
		
		// 移除好友关系
		BuddyRelation relation0 = new BuddyRelation(userId1, userId2, BuddyRelation.BuddyDesc.Friend);
		_adapter.removeFriend(relation0);	
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {		
			e.printStackTrace();
		}	
		System.out.println(" --- Remove Friend Relation : " + String.valueOf(userId1) + " and " + String.valueOf(userId2) + " --- ");
		int dist2 = _adapterDist.getFriendDistManager(userId2).getTotalCount(userId2);
		int buddy2 = _adapter.getBuddyCount(userId2);				
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + String.valueOf(dist2));
		dist = _adapterDist.getFriendDistManager(userId2).getFriendDist(userId2);
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + dist.toString());
		System.out.println("Buddy " + String.valueOf(userId2) + " : " + String.valueOf(buddy2));
		
		Assert.assertEquals(dist1-dist2, buddy1-buddy2);
		
	}	
}

class FriendDistAdatper extends ServiceAdapter {	
	
	private static FriendDistAdatper _adapter = new FriendDistAdatper();	
	private static final String endpoints = "@FriendDist";	
	private Vector<FriendDistManagerPrx> managers = new Vector<FriendDistManagerPrx>();
	
	private FriendDistAdatper() {
		super(endpoints, 10, Channel.newOceChannel(endpoints));		
	}
	
	public static FriendDistAdatper getInstance() {
		return _adapter;
	}
	
	protected FriendDistManagerPrx getFriendDistManager(int userId) {
		return locateProxy(managers, "FDM", userId, Channel.Invoke.Twoway,
				FriendDistManagerPrxHelper.class, 300);
	}
	
	public Map<Integer, Integer> getFriendDist(int userId) {
		return getFriendDistManager(userId).getFriendDist(userId);
	}
}
