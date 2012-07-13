package com.xiaonei.xce.test.frienddist;


import java.sql.Connection;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;

import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
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

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class FriendDistTestCase {
	
	private static int userId1, userId2;
	public static BuddyCoreAdapterImpl _adapter = new BuddyCoreAdapterImpl();
	public static FriendDistAdatper _adapterDist = FriendDistAdatper.getInstance();
	public static Connection connCommonR;
	public static Statement stmtR;
	
	public FriendDistTestCase(int userId1, int userId2)  {
		this.userId1 = userId1;
		this.userId2 = userId2;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000002, 700000010},		//测试第一组
		      	{700000007, 700000009},		//测试第二组
		});
	}	
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {	
		
		/* try {
			connCommonR = XceAdapter.getInstance().getReadConnection("user_friendcount");			
			try {
				stmtR = connCommonR.createStatement();
			} catch (Exception e) {
				e.printStackTrace();
			}
		} catch (Exception e)  {			
			e.printStackTrace();			
		} */
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
	public void test() {	
		
		try {
			Connection connCommonRW = XceAdapter.getInstance().getReadConnection("bulletin_board");
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		/* BuddyApplyWithInfo request1 = new BuddyApplyWithInfo(userId1, userId2);
		
		// 输出初始状态
		System.out.println(" --- No Relation : " + String.valueOf(userId1) + " and " + String.valueOf(userId2) + " --- ");
		int dist0 = _adapterDist.getFriendDistManager(userId2).getTotalCount(userId2);						
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + String.valueOf(dist0));		
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + _adapterDist.getFriendDistManager(userId2).getFriendDist(userId2).toString());				
				
		// 添加好友关系
		_adapter.addApply(request1);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}	
		_adapter.acceptApply(request1);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		_adapterDist.rebuildFriendDist(userId2);		
		// 取数据库
		int count_inDb1 = -1;
		try {
			Thread.sleep(3000);
			String sql1 = "SELECT friendcount FROM user_friendcount WHERE id = " + String.valueOf(userId2);			
			ResultSet rs = stmtR.executeQuery(sql1);
			if ( rs.next() )  {
				count_inDb1 = rs.getInt("friendcount");
			}
		} catch (Exception e)  {
			e.printStackTrace();
		}
		System.out.println("\n\n --- Add Friend Relation : " + String.valueOf(userId1) + " and " + String.valueOf(userId2) + " --- ");
		int dist1 = _adapterDist.getFriendDistManager(userId2).getTotalCount(userId2);						
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + String.valueOf(dist1));		
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + _adapterDist.getFriendDistManager(userId2).getFriendDist(userId2).toString());
		System.out.println("Count in DB " + String.valueOf(userId2) + " : " + String.valueOf(count_inDb1));
		// 对比
		Assert.assertEquals(dist1, count_inDb1);
		
		
		// 移除好友关系
		BuddyRelation relation0 = new BuddyRelation(userId1, userId2, BuddyRelation.BuddyDesc.Friend);
		_adapter.removeFriend(relation0);	
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {		
			e.printStackTrace();
		}
		_adapterDist.rebuildFriendDist(userId2);
		// 取数据库
		int count_inDb2 = -1;
		try {
			Thread.sleep(3000);
			String sql1 = "SELECT friendcount FROM user_friendcount WHERE id = " + String.valueOf(userId2);			
			ResultSet rs = stmtR.executeQuery(sql1);
			if ( rs.next() )  {
				count_inDb2 = rs.getInt("friendcount");
			}
		} catch (Exception e)  {
			e.printStackTrace();
		}
		System.out.println(" --- Remove Friend Relation : " + String.valueOf(userId1) + " and " + String.valueOf(userId2) + " --- ");
		int dist2 = _adapterDist.getFriendDistManager(userId2).getTotalCount(userId2);						
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + String.valueOf(dist2));		
		System.out.println("FriendDist " + String.valueOf(userId2) + " : " + _adapterDist.getFriendDistManager(userId2).getFriendDist(userId2).toString());		
		System.out.println("Count in DB " + String.valueOf(userId2) + " : " + String.valueOf(count_inDb2));
		// 对比
		Assert.assertEquals(dist2, count_inDb2); */
	}
}

class FriendDistAdatper extends ServiceAdapter {	
	
	private static FriendDistAdatper _adapter = new FriendDistAdatper();	
	private static final String endpoints = "@FriendDist";	
	private Vector<FriendDistManagerPrx> managers = new Vector<FriendDistManagerPrx>();
	private Vector<FriendDistManagerPrx> managersOneway = new Vector<FriendDistManagerPrx>();		
	
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
	
	protected FriendDistManagerPrx getFriendDistManagerOneway(int userId) {
		return locateProxy(managersOneway, "FDM", userId,
				Channel.Invoke.Oneway, FriendDistManagerPrxHelper.class, 300);
	}

	public Map<Integer, Integer> getFriendDist(int userId) {
		return getFriendDistManager(userId).getFriendDist(userId);
	}
	
	public int getTotalCount(int userId)  {
		return getFriendDistManager(userId).getTotalCount(userId);	
	}
	
	public mop.hi.svc.model.FriendDistWithTotal getFriendDistWithTotal(int userId) {
		 return getFriendDistManager(
				userId).getFriendDistWithTotal(userId);			
	}

	public int getFriendDistByUniversity(int userId, int universityId) {
		return getFriendDistManager(userId).getFriendDistByUniv(userId,
				universityId);		
	}	
	
	public void rebuildFriendDist(int userId) {
		getFriendDistManagerOneway(userId).rebuildFriendDist(userId);
	}	
}
