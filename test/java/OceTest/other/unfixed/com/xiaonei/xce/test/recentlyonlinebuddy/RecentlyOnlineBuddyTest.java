package com.xiaonei.xce.test.recentlyonlinebuddy;

import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.buddy.BuddyRelation;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.recentlyonlinebuddy.RecentlyOnlineBuddyManagerPrx;
import xce.recentlyonlinebuddy.RecentlyOnlineBuddyManagerPrxHelper;
import xce.util.channel.Channel;

import com.xiaonei.xce.recentlyonlinebuddy.RecentlyOnlineBuddyAdapter;

@RunWith(Parameterized.class)
public class RecentlyOnlineBuddyTest {
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {{},});
	}
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		List<BuddyRelation> list=AdapterFactory.getBuddyCoreAdapter().getFriendList(700000003, 0, -1);
		for(BuddyRelation relation:list){
			BuddyRelation r=new BuddyRelation(700000003, relation.toId,BuddyRelation.BuddyDesc.Friend);
			AdapterFactory.getBuddyCoreAdapter().removeFriend(r);
		}
		AdapterFactory.getBuddyCoreAdapter().addFriend(700000003, 700000002);
		AdapterFactory.getBuddyCoreAdapter().addFriend(700000003, 700000001);
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		AdapterFactory.getBuddyCoreAdapter().addFriend(700000003, 700000002);
		AdapterFactory.getBuddyCoreAdapter().addFriend(700000003, 700000001);
	}

	@Before
	public void setUpBefore() throws Exception {
	}

	@After
	public void tearDownAfter() throws Exception {
	}
	@Test
	public void test() {
		RecentlyOnlineBuddyManagerPrx manager1=RecentlyOnlineBuddyManagerPrxHelper.uncheckedCast(Channel.newOceChannel("RecentlyOnlineBuddy").getCommunicator().stringToProxy("M@RecentlyOnlineBuddy1"));
		RecentlyOnlineBuddyManagerPrx manager2=RecentlyOnlineBuddyManagerPrxHelper.uncheckedCast(Channel.newOceChannel("RecentlyOnlineBuddy").getCommunicator().stringToProxy("M@RecentlyOnlineBuddy2"));
		RecentlyOnlineBuddyManagerPrx manager3=RecentlyOnlineBuddyManagerPrxHelper.uncheckedCast(Channel.newOceChannel("RecentlyOnlineBuddy").getCommunicator().stringToProxy("M@RecentlyOnlineBuddy3"));
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		RecentlyOnlineBuddyAdapter.getInstance().rebuild(700000003);
		List<Integer> ids=null;
		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		manager1.somebodyOnline(700000001);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		ids=RecentlyOnlineBuddyAdapter.getInstance().get(700000003, 1000);
		System.out.println(ids.size());
		Assert.assertEquals(ids.size(),2);
		Assert.assertEquals(ids.get(0).intValue(),700000001);
		Assert.assertEquals(ids.get(1).intValue(),700000002);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		manager2.somebodyOnline(700000002);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		ids=RecentlyOnlineBuddyAdapter.getInstance().get(700000003, 1000);
		System.out.println(ids.size());
		Assert.assertEquals(ids.size(),2);
		Assert.assertEquals(ids.get(0).intValue(),700000002);
		Assert.assertEquals(ids.get(1).intValue(),700000001);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		BuddyRelation r=new BuddyRelation(700000003, 700000001,BuddyRelation.BuddyDesc.Friend);
		AdapterFactory.getBuddyCoreAdapter().removeFriend(r);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		RecentlyOnlineBuddyAdapter.getInstance().rebuild(700000003);
		
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		ids=RecentlyOnlineBuddyAdapter.getInstance().get(700000003, 1000);
		Assert.assertEquals(ids.size(),1);
		Assert.assertEquals(ids.get(0).intValue(), 700000002);
		r=new BuddyRelation(700000003, 700000002,BuddyRelation.BuddyDesc.Friend);
		AdapterFactory.getBuddyCoreAdapter().removeFriend(r);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		ids=RecentlyOnlineBuddyAdapter.getInstance().get(700000003, 1000);
		Assert.assertEquals(0, ids.size());
	}
}
