package com.xiaonei.xce.autogroup;

import com.xiaonei.xce.autogroup.AutoGroupingAdapter;

import Demo.AutoGroupingPrx;
import Demo.AutoGroupingPrxHelper;
import Demo.FinalGroup;
import Demo.Group;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class AutoGroupingAdapter extends ServiceAdapter
{
	private static final String endpoints = "@AutoGrouping";
	private AutoGroupingPrx manager;
	private AutoGroupingPrx managerOneway;
	
	private int friendsNumber = 0;
	private int friendsAccount = 0;
	private float friendsRate = 0;
	private int ungroupingNumber = 0;
	private Group[] totalGroup;
	private Group ungroupingMember;
	private boolean signal = false;
	
	public AutoGroupingAdapter()
	{
		super(endpoints, 0, Channel.newSocialGraphChannel(endpoints));
//		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("AutoGrouping", Channel.Invoke.Twoway, AutoGroupingPrxHelper.class, 100000);
		managerOneway = locateProxy("AutoGrouping", Channel.Invoke.Oneway, AutoGroupingPrxHelper.class, 100000);
	}
	
	public void setBase(int userId)
	{	
		FinalGroup finalgroup = manager.getGroup(userId);
		
		if(1 == finalgroup.isEmpty )
		{
			signal = true;
			return;
		}
		
		friendsNumber = finalgroup.friendsNumber;
		friendsAccount = finalgroup.friendsAccount;
		friendsRate = finalgroup.friendsRate;
		ungroupingNumber = finalgroup.ungroupingNumber;
		totalGroup = finalgroup.totalGroupList;
		ungroupingMember = finalgroup.ungroupingMember;
	}
	
	public boolean isEmpty()	
	{
		return signal;
	}
	
	public int getFriendsNumber()
	{
		return friendsNumber;
	}
	
	public int getFriendsAccount()
	{
		return friendsAccount;
	}
	
	public float getFriendsRate()
	{
		return friendsRate;
	}
	
	public int getUngroupingNumber()
	{
		return ungroupingNumber;
	}
	
	public Group[] getTotalGroup()
	{
		return totalGroup;
	}
	
	public Group getUngroupingMember() 
	{
		return ungroupingMember;
	}
	
	public static void main(String[] args) 
	{
//		int userId = 24496255;  //叶湘
//		int userId = 238111132;  //我
//		int userId = 1331;  // 翟禹 
		int userId = 239737004;  // 蔡建山

		
		AutoGroupingAdapter adapt = new AutoGroupingAdapter();
		adapt.setBase(userId);
		
		if(adapt.isEmpty())
		{
			System.out.println("cache 中没有该用户的分组信息");
			System.exit(0);
		}
		
		Group[] totalGroup = adapt.getTotalGroup();
		
		System.out.println("您共有  "+ adapt.getFriendsNumber() +" 个好友");
		System.out.print("已分组 "+ adapt.getFriendsAccount() +" 个好友"+"      ");
		System.out.println("占已有好友比例的 "+adapt.getFriendsRate());
		System.out.println("共有分组 : "+totalGroup.length);
		
		for(int i=0; i<totalGroup.length; ++i)
		{
			Group tempMember = totalGroup[i];
			int[] friendsId = tempMember.friendsId;
			System.out.println(tempMember.groupName);
			
			for(int j=0; j<friendsId.length; ++j)
			{
				System.out.print(friendsId[j] + "  ");
			}
			System.out.println();
			System.out.println();
		}
		
		System.out.println("未分组好友数为 : "+adapt.getUngroupingNumber());
		
		Group ungroupingMembers = adapt.getUngroupingMember();
		int[] ungroupingFriends = ungroupingMembers.friendsId;
		
		System.out.println(ungroupingMembers.groupName);
		for(int i=0; i<ungroupingFriends.length; ++i)
		{
			System.out.print(ungroupingFriends[i] + "  ");
		}
		System.out.println();
		
		System.exit(0);
	}
}
