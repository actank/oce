package com.renren.tripod.internalclient;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.danga.MemCached.MemCachedClient;
import com.renren.tripod.configclient.CacheConfigClient;
import com.renren.tripod.configclient.ConfigClientFactory;
import com.xiaonei.xce.log.Tripod;

public class CacheClient {
	private CacheConfigClient _config = null;
	private String _zkAddress = null;

	public CacheClient(String zkAddress) {
		_zkAddress = zkAddress;
	}

	private CacheConfigClient getConfigClient() {
		if (_config == null) {
			synchronized (this) {
				_config = ConfigClientFactory.getCacheConfigClient(_zkAddress);
			}
		}
		return _config;
	}

	public Map<String, String> get(List<String> keys, String namespaceId,
			String businessId) {
		CacheConfigClient config = getConfigClient();
		if (config == null) {
			Tripod.getLogger().error("CacheClient.get config == null, namespaceId " + namespaceId + " bussinessId " + businessId);
			return null;
		}
		Map<MemCachedClient, List<String>> prxMap = config.getPrxToKeysMap(
				keys, namespaceId);
		Map<String, String> res = new HashMap<String, String>();
		for (Entry<MemCachedClient, List<String>> ent : prxMap.entrySet()) {
			List<String> bigKeys = getBigKey(ent.getValue(), businessId);
			Map<String, Object> singleRes = ent.getKey().getMulti(
					bigKeys.toArray(new String[bigKeys.size()]));
			for (Entry<String, Object> srEnt : singleRes.entrySet()) {
				res.put(getKey(srEnt.getKey(), businessId),
						(String) srEnt.getValue());
			}
		}
		return res;
	}

	public boolean set(String key, String value, String namespaceId,
			String businessId, long expiredTime) {
		CacheConfigClient config = getConfigClient();
		if (config == null) {
			Tripod.getLogger().error("CacheClient.get config == null, namespaceId " + namespaceId + " bussinessId " + businessId);
			return false;
		}

		MemCachedClient prx = config.getPrx(key, namespaceId);
		if (prx == null) {
			Tripod.getLogger().error("CacheClient.set prx == null, namespaceId " + namespaceId);
			return false;
		}
		return prx
				.set(getBigKey(key, businessId), value, new Date(expiredTime));
	}

	private String getKey(String bigKey, String businessId) {
		return bigKey.substring(businessId.length() + 1, bigKey.length());
	}

	private List<String> getBigKey(List<String> keys, String businessId) {
		List<String> bigKeys = new ArrayList<String>();
		for (String key : keys) {
			bigKeys.add(businessId + "_" + key);
		}
		return bigKeys;
	}

	private String getBigKey(String key, String businessId) {
		return businessId + "_" + key;
	}

}
