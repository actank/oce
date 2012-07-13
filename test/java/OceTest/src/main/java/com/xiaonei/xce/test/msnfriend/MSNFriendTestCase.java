package com.xiaonei.xce.test.msnfriend;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.mfd.MSNFriend;

import com.xiaonei.xce.msnfriend.MSNFriendAdapter;

@RunWith(Parameterized.class)
public class MSNFriendTestCase {
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {{},});
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
	public void test() {
		MSNFriend[] friends = MSNFriendAdapter.getInstance().getFriendWithCommon(700000001, 10);
		for(int i=0;i<friends.length;i++){
			System.out.println(friends[i]);
		}
	}
}
