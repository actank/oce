package com.xiaonei.xce.test.buddycachebyid;

import java.util.Arrays;
import java.util.Collection;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.buddy.BuddyRelation;
import mop.hi.oce.domain.buddy.BuddyRelation.BuddyDesc;

import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.buddycachebyid.BuddyCacheByIdAdapter;


@RunWith(Parameterized.class)
public class BuddyCacheByIdTestCase {
	private static BuddyCacheByIdAdapter _adapter=BuddyCacheByIdAdapter.getInstance();
	
	private int userId1, userId2;

	public BuddyCacheByIdTestCase(int userId1, int userId2) {
		this.userId1 = userId1;
		this.userId2 = userId2;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][]{
		      	{700000001, 700000003},		//测试第一组
		});
	}
	
	@Test
	public void test() {
		
		AdapterFactory.getBuddyCoreAdapter().addFriend(userId1, userId2);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		int friendCount1 = _adapter.getFriendListOrderByIdDesc(userId1).length;
		int friendCount2 = _adapter.getFriendListOrderByIdDesc(userId2).length;
		Assert.assertEquals(friendCount1, friendCount2);
		
		AdapterFactory.getBuddyCoreAdapter().removeFriend(new BuddyRelation(userId1,userId2,BuddyDesc.Friend));
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		int[] ids=_adapter.getFriendListOrderByIdDesc(userId2);
		System.out.println(ids.length);
		boolean removeflag=true;
		for(int id:ids){
			System.out.println(id);
			if(id==700000001){
				removeflag=false;
			}
		}
		Assert.assertTrue(removeflag);
		AdapterFactory.getBuddyCoreAdapter().addFriend(userId1,userId2);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		ids=_adapter.getFriendListOrderByIdDesc(userId2);
		System.out.println(ids.length);
		boolean addflag=false;
		for(int id:ids){
			System.out.println(id);
			if(id==userId1){
				addflag=true;
			}
		}
		Assert.assertTrue(addflag);
	}
}
