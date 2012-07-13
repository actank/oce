package com.xiaonei.xce.friendfinderbyactiveuser;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

public class RecommendData {
	private Map<Integer, List<Integer>> _recommendMap = new HashMap<Integer, List<Integer>>();
	
	public void set(Map<Integer, int[]> recommend) {
		for(Entry<Integer, int[]> e : recommend.entrySet()) {
			List<Integer> friends = new ArrayList<Integer>();
			for(int o : e.getValue()) {
				friends.add(o);
			}
			Collections.synchronizedMap(_recommendMap).put(e.getKey(), friends);
		}
	}
	
	public Map<Integer, List<Integer>> get() {
		return Collections.synchronizedMap(_recommendMap);
	}
}