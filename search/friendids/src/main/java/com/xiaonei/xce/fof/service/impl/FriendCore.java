package com.xiaonei.xce.fof.service.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentMap;

import com.google.common.collect.MapMaker;

/*
 * 
 * 
 0

 222905898
 229399879
 235405804
 243009105
 253925514
 265700373
 283859063
 323640180
 358516004

 2147483647
 */
public class FriendCore {

	private HashMap<Integer, ConcurrentMap<Integer, int[]>> friendIdsMap;

	public int[] _partitions;
	public int _nodeId;

	public FriendCore(int nodeId,int[] partitions) {
		_nodeId = nodeId;
		_partitions = partitions;
		friendIdsMap = new HashMap<Integer, ConcurrentMap<Integer, int[]>>(
				partitions.length);
		for (int partition : partitions) {
			ConcurrentMap<Integer, int[]> newMap = new MapMaker()
					.concurrencyLevel(1024).makeMap();
			friendIdsMap.put(partition, newMap);
		}
	}

	public List<int[]> getFriendIds(int partition, List<Integer> friends) {
		List<int[]> res = new ArrayList<int[]>(friends.size());
		ConcurrentMap<Integer, int[]> curMap = friendIdsMap.get(partition);
		if (curMap == null) {
			System.out.println("pass error partition");
			return res;
		}
		for (Integer friend : friends) {
			int[] ids = curMap.get(friend);
			if (ids != null)
				res.add(ids);
			else{
				System.out.println("miss uid "+friend);
			}
		}
		return res;
	}

	public void setFriendIds(int partition, Map<Integer, int[]> friends) {
		friendIdsMap.get(partition).putAll(friends);
	}
}
