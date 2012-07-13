package com.xiaonei.xce.buddygroup.test;

import com.xiaonei.xce.buddygroup.BuddyGroupAdapter;

import junit.framework.TestCase;

public class BuddyGroupTest  extends TestCase{
	public void testGetGroupList(){
	
		//System.out.print(BuddyGroupAdapter.getInstance().getGroupList(128487631).size());
		System.out.print(BuddyGroupAdapter.getInstance().getBuddyGroups(264631588 , 269196925 ));
	
	}
}
