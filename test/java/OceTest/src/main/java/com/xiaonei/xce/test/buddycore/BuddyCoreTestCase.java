package com.xiaonei.xce.test.buddycore;

import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
import mop.hi.oce.domain.buddy.BuddyApply;
import mop.hi.oce.domain.buddy.BuddyApplyWithInfo;
import mop.hi.oce.domain.buddy.BuddyBlock;
import mop.hi.oce.domain.buddy.BuddyRelation;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.usercount.UserCountAdapter;

@RunWith(Parameterized.class)
public class BuddyCoreTestCase {	
	
	private int _userId1, _userId2;
	BuddyCoreAdapterImpl _adapter = new BuddyCoreAdapterImpl();
	UserCountAdapter _adapter_uc = UserCountAdapter.getInstance();	
	
	public BuddyCoreTestCase(int userId1, int userId2) {
		this._userId1 = userId1;
		this._userId2 = userId2;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
 		      	{700000001, 700000002},		//测试第一组
//		      	{700000004, 700000005},		//测试第二组
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
		BuddyRelation relation = _adapter.getRelation(_userId1, _userId2);
		if( relation.isFriend() ) {
			RemoveFriend(_userId1, _userId2);
			System.out.println("these two man is already Friend, so remove it first");
		} else if ( relation.isApply() ) {
			DenyApply(_userId1, _userId2);
			System.out.println(_userId1 + " has apply " + _userId2 + " already, so " + _userId2 + " deny it first");
		} else if ( relation.isApplied() ) {
			DenyApply(_userId2, _userId1);
			System.out.println(_userId1 + " is applied by " + _userId2 + " already, so " + _userId1 + " deny it first");
		} else if ( relation.isBlock() ) {
			RemoveBlock(_userId1, _userId2);
			System.out.println(_userId1 + " has block " + _userId2 + " already, so " + _userId1 + " deny it first");
		} else if ( relation.isBlocked() ) {
			RemoveBlock(_userId2, _userId1);
			System.out.println(_userId1 + " is blocked by " + _userId2 + " already, so " + _userId1 + " deny it first");
		}
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertFalse(relation.isFriend());
		Assert.assertFalse(relation.isApply());
		Assert.assertFalse(relation.isApplied());
		Assert.assertFalse(relation.isBlock());
		Assert.assertFalse(relation.isBlocked());

		System.out.println(" --- now begin");
		
		//申请
		int requestcount_before = _adapter_uc.get(_userId2, 3);
		
		BuddyApplyWithInfo apply = new BuddyApplyWithInfo(_userId1, _userId2);
		_adapter.addApply(apply, "Home.do.PeopleYouMayKnow");
		System.out.println(" --- AddApply : " + String.valueOf(_userId1) + " and " + String.valueOf(_userId2) + " --- ");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertTrue(relation.isApply());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertTrue(relation.isApplied());
		
		int requestcount_after = _adapter_uc.get(_userId2, 3);
		Assert.assertEquals(requestcount_before+1,requestcount_after);
		
		//接受
		AcceptApply(_userId1, _userId2);
		System.out.println(" --- AcceptApply : " + String.valueOf(_userId1) + " and " + String.valueOf(_userId2) + " --- ");
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertTrue(relation.isFriend());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertTrue(relation.isFriend());
		
		requestcount_after = _adapter_uc.get(_userId2, 3);
		Assert.assertEquals(requestcount_before,requestcount_after);
		
		//删除
		RemoveFriend(_userId1, _userId2);
		System.out.println(" --- RemoveFriend : " + String.valueOf(_userId1) + " and " + String.valueOf(_userId2) + " --- ");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertFalse(relation.isFriend());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertFalse(relation.isFriend());
		
		//再次申请
		requestcount_before = _adapter_uc.get(_userId1, 3);
		AddApply(_userId2, _userId1);
		System.out.println(" --- AddApply : " + String.valueOf(_userId2) + " and " + String.valueOf(_userId1) + " --- ");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertTrue(relation.isApply());
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertTrue(relation.isApplied());
		
		requestcount_after = _adapter_uc.get(_userId1, 3);
		Assert.assertEquals(requestcount_before+1,requestcount_after);
		
		//拒绝  
		DenyApply(_userId2, _userId1);
		System.out.println(" --- DenyApply : " + String.valueOf(_userId2) + " and " + String.valueOf(_userId1) + " --- ");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertFalse(relation.isFriend());
		Assert.assertFalse(relation.isApply());
		Assert.assertFalse(relation.isApplied());
		Assert.assertFalse(relation.isBlock());
		Assert.assertFalse(relation.isBlocked());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertFalse(relation.isFriend());
		Assert.assertFalse(relation.isApply());
		Assert.assertFalse(relation.isApplied());
		Assert.assertFalse(relation.isBlock());
		Assert.assertFalse(relation.isBlocked());

		requestcount_after = _adapter_uc.get(_userId1, 3);
		Assert.assertEquals(requestcount_before,requestcount_after);

		//黑名单
		System.out.println(" --- Block : " + String.valueOf(_userId1) + " and " + String.valueOf(_userId2) + " --- ");		
		// _userId1将_userId2加入黑名单
		AddBlock(_userId1, _userId2);
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		// _userId2向_userId1发送请求
		System.out.println("     AddApply : " + String.valueOf(_userId2) + " and " + String.valueOf(_userId1) + ", should be Blocked     ");
		AddApply(_userId2, _userId1);		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}		
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertTrue(relation.isBlock());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertTrue(relation.isBlocked());	
		// _userId1向_userId2发送请求
		System.out.println("     AddApply : " + String.valueOf(_userId1) + " and " + String.valueOf(_userId2) + ", should remove Block ");
		AddApply(_userId1, _userId2);		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}		
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertTrue(relation.isApply());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertTrue(relation.isApplied());
		
		//拒绝（回到原始状态）
		DenyApply(_userId1, _userId2);
		System.out.println(" --- DenyApply : " + String.valueOf(_userId1) + " and " + String.valueOf(_userId2) + " --- ");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		relation = _adapter.getRelation(_userId1, _userId2);
		Assert.assertFalse(relation.isFriend());
		Assert.assertFalse(relation.isApply());
		Assert.assertFalse(relation.isApplied());
		Assert.assertFalse(relation.isBlock());
		Assert.assertFalse(relation.isBlocked());
		relation = _adapter.getRelation(_userId2, _userId1);
		Assert.assertFalse(relation.isFriend());
		Assert.assertFalse(relation.isApply());
		Assert.assertFalse(relation.isApplied());
		Assert.assertFalse(relation.isBlock());
		Assert.assertFalse(relation.isBlocked());
	}
	public void RemoveFriend(int userId1,int userId2){
		BuddyRelation relation = new BuddyRelation(userId1, userId2, BuddyRelation.BuddyDesc.Friend);
		_adapter.removeFriend(relation);	
	}
	public void AddBlock(int userId1,int userId2){
		BuddyBlock block = new BuddyBlock(userId1, userId2);
		_adapter.addBlock(block);	
	}
	public void RemoveBlock(int userId1,int userId2){
		BuddyBlock block = new BuddyBlock(userId1, userId2);
		_adapter.removeBlock(block);	
	}
	public void DenyApply(int userId1,int userId2){
		BuddyApply apply = new BuddyApply(userId1, userId2);
		_adapter.denyApply(apply);	
	}
	public void AddApply(int userId1,int userId2){
		BuddyApplyWithInfo apply = new BuddyApplyWithInfo(userId1, userId2);
		_adapter.addApply(apply);	
	}
	public void AcceptApply(int userId1,int userId2){
		BuddyApplyWithInfo apply = new BuddyApplyWithInfo(userId1, userId2);
		_adapter.acceptApply(apply);	
	}	
}
