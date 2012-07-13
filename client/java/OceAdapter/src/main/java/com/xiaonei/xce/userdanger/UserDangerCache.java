package com.xiaonei.xce.userdanger;

import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

public class UserDangerCache extends Thread {

    BlockingQueue<Integer> _queue;
    ConcurrentHashMap<Integer, Set<Integer>> _map;

    protected UserDangerCache() {
	_queue = new LinkedBlockingQueue<Integer>(100);
	_map = new ConcurrentHashMap<Integer, Set<Integer>>();
	start();
    }

    private void initialize() {
	try {
	    Map<Integer, List<Integer>> m = UserDangerAdapter.getInstance()
		    .getClientCache();
	    for (Entry<Integer, List<Integer>> e : m.entrySet()) {
		_map.put(e.getKey(), Collections.synchronizedSet(new HashSet<Integer>(e.getValue())));
	    }
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }

    private static UserDangerCache _cache = new UserDangerCache();

    public static UserDangerCache getInstance() {
	return _cache;
    }

    public void run() {
	initialize();
	while (true) {
	    try {
		Thread.sleep(600 * 1000);
		Map<Integer, List<Integer>> m = UserDangerAdapter.getInstance()
			.getClientCache();
		for (Entry<Integer, List<Integer>> e : m.entrySet()) {
		    _map.put(e.getKey(), Collections.synchronizedSet(new HashSet<Integer>(e.getValue())));
		}
		for (Integer i : _map.keySet()) {
		    if (!m.containsKey(i)) {
			_map.remove(i);
		    }
		}
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	}
    }

    public Set<Integer> getSet(int type) {
	return _map.get(type);
    }

    public void change(Map<String, String> content) {
	int userid = Integer.parseInt(content.get("ID"));
	int type = Integer.parseInt(content.get("TYPE"));
	boolean operation = Boolean.parseBoolean(content.get("OPERATION"));
	Set<Integer> s = getSet(type);
	if(s != null){
	    if(operation){
		s.add(userid);
	    }else {
		s.remove(userid);
	    }
	}
    }
}
