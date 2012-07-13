package com.renren.tripod.impl;

import java.util.HashMap;
import java.util.Map;

import com.renren.tripod.TripodProducer;

public class TestTripodProducer extends TripodProducer {

	@Override
	public Map<String, String> create(String[] keys, boolean isMaster) {
		// TODO Auto-generated method stub
    Map<String, String> res = new HashMap<String, String>();

    for(String key : keys) {
      res.put(key, key + key);
    }
		return res; 
	}

	@Override
	public String[] createKeys(String beginKey, int limit) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String[] createHotKeys(String beginKey, int limit) {
		// TODO Auto-generated method stub
		return null;
	}

}
