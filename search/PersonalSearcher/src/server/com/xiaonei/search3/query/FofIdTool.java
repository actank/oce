package com.xiaonei.search3.query;

import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;

/**
 * 取得二度好友关系以及好友的所有位置信息
 * 
 * @author xuemin.luo
 **/
public class FofIdTool {

	/** 调用此方法取得去重的二度好友关系以及好友的索引位置 **/
	public static Set<Integer> getFOFID(int hostId) {
		// 取出二度好友,并且去重
		Map<Integer, List<Integer>> map = getFOFFriend(hostId);
		Set<Integer> set = getFFSet(hostId, map);
		return set;
	}

	/** 1.取得二度好友 **/
	private static Map<Integer, List<Integer>> getFOFFriend(int hostId) {
		// 取一度好友
		List<Integer> friendList = BuddyByIdCacheAdapter.getInstance()
				.getFriendListAsc(hostId, 2000);
		// 取二度好友
		Map<Integer, List<Integer>> ffMap = BuddyByIdCacheAdapter.getInstance()
				.getFriendListsAsc(friendList);
		ffMap.put(hostId, friendList);// 加入一度好友方便去重
		return ffMap;
	}

	/** 2.去重 **/
	private static Set<Integer> getFFSet(int hostId,
			Map<Integer, List<Integer>> ffMap) {
		Set<Integer> set = new HashSet<Integer>();
		// 一度好友
		List<Integer> friendList = ffMap.get(hostId);
		ffMap.remove(hostId);
		for (Integer id : friendList) {
			set.add(id);
		}
		for (Entry<Integer, List<Integer>> entry : ffMap.entrySet()) {
			for (Integer id : entry.getValue()) {
				if (set.contains(id) == false) {
					set.add(id);
				}
			}
		}
		return set;
	}

	public static void main(String args[]) {
		Set<Integer> set = FofIdTool.getFOFID(Integer.valueOf(args[0]));
		for (Integer id : set) {
			System.out.println("id : " + id);
		}
	}
}
