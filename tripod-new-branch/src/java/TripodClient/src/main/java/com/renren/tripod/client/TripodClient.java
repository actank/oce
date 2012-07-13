package com.renren.tripod.client;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.renren.tripod.internalclient.CacheClient;
import com.renren.tripod.internalclient.CacheManagerClient;
import com.renren.tripod.internalclient.ClientFactory;
import com.xiaonei.xce.log.Tripod;

public class TripodClient {
	
	private String _zkAddress = null;
	private String _namespaceId = null;
	private String _businessId = null;

	private CacheClient _cacheClient = null;
	private CacheManagerClient _cacheManagerClient = null;

	public TripodClient(String zkAddress, String namespaceId, String businessId) {
		_zkAddress = zkAddress;
		_namespaceId = namespaceId;
		_businessId = businessId;
	}

	public Map<String, String> get(List<String> keys, List<String> missedKeys) {
		CacheClient cc = getCacheClient();
		if (cc == null) {
			Tripod.getLogger().error("TripodClient.get cacheclient == null for zkAddress "
					+ _zkAddress);
			return null;
		}
		Map<String, String> res = cc.get(keys, _namespaceId, _businessId);
		getMissedKeys(res, keys, missedKeys);
		return res;
	}

	public Map<String, String> getMissed(List<String> keys, long expiredTime,
			boolean useMaster) {
		return getMissed(keys, expiredTime, useMaster, 300);
	}

	public Map<String, String> getMissed(List<String> keys, long expiredTime,
			boolean useMaster, int timeout) {
		CacheManagerClient cc = getCacheManagerClient();
		if (cc == null) {
			Tripod.getLogger().error("TripodClient.getMissed cachemanagerclient == null for zkAddress "
					+ _zkAddress);
			return null;
		}
		return cc.get(keys, _namespaceId, _businessId, expiredTime, useMaster,
				timeout);
	}
	
	public Map<String, String> getWithMissed(List<String> keys, long expiredTime,
			boolean useMaster, int timeout) {
		List<String> missedKeys = new ArrayList<String>();
		Map<String, String> res = get(keys, missedKeys);
		
		if(!missedKeys.isEmpty()) {
			Map<String, String> missedRes = getMissed(missedKeys, expiredTime, useMaster, timeout);
			res.putAll(missedRes);
		}
		return res;
	}

	public boolean load(List<String> keys, long expiredTime, boolean useMaster) {
		return load(keys, expiredTime, useMaster, 1000);
	}

	public boolean load(List<String> keys, long expiredTime, boolean useMaster,
			int timeout) {
		CacheManagerClient cc = getCacheManagerClient();
		if (cc == null) {
			Tripod.getLogger().error("TripodClient.load cachemanagerclient == null for zkAddress "
					+ _zkAddress);
			return false;
		}
		return cc.load(keys, _namespaceId, _businessId, expiredTime, useMaster,
				timeout);
	}

	public boolean deleteCache(List<String> keys) {
		return deleteCache(keys, 1000);
	}

	public boolean deleteCache(List<String> keys, int timeout) {
		CacheManagerClient cc = getCacheManagerClient();
		if (cc == null) {
			Tripod.getLogger().error("TripodClient.deleteCache cachemanagerclient == null for zkAddress "
					+ _zkAddress);
			return false;
		}
		return cc.deleteCache(keys, _namespaceId, _businessId, timeout);
	}

	private List<String> getMissedKeys(Map<String, String> res,
			List<String> keys, List<String> missedKeys) {
		for (String str : keys) {
			if (res.get(str) == null) {
				missedKeys.add(str);
			}
		}
		return missedKeys;
	}

	private CacheClient getCacheClient() {
		if (_cacheClient == null) {
			synchronized (this) {
				if (_cacheClient == null) {
					_cacheClient = ClientFactory.getCachegClient(_zkAddress);
				}
			}
		}
		return _cacheClient;
	}

	private CacheManagerClient getCacheManagerClient() {
		if (_cacheManagerClient == null) {
			synchronized (this) {
				if (_cacheManagerClient == null) {
					_cacheManagerClient = ClientFactory
							.getCacheManagerClient(_zkAddress);
				}
			}
		}
		return _cacheManagerClient;
	}
	
	public static void main(String[] args) {
		
		//PropertyConfigurator.configure("/Volumes/Work/code/forxcecache/tripod-new/src/java/TripodClient/src/main/java/com/renren/tripod/client/log4j.properties");
		TripodClient tc = new TripodClient("TestZooKeeper1:2181/Tripod", "test", "TEST");
		List<String> keys = new ArrayList<String>();
		keys.add("123");
		
		System.out.println("load " + tc.load(keys, 1000, false));
		
		List<String> missedKeys = new ArrayList<String>();
		Map<String, String> res = tc.get(keys, missedKeys);
		System.out.println("get size " + res.size());
		System.out.println("get size " + res.get("123"));
		System.out.println("get missed size " + missedKeys.size());
		
		System.out.println("delete " + tc.deleteCache(missedKeys));
		
		res = tc.getMissed(keys, 1000, false);
		System.out.println("getmissed size " + res.size());
		
		System.exit(0);
		
	}
	
}
