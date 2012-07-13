package com.renren.tripod.configclient;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import com.danga.MemCached.MemCachedClient;
import com.danga.MemCached.SockIOPool;

import com.renren.tripod.configutil.ConfigClient;
import com.renren.tripod.configutil.ConfigData;

public class CacheConfigClient extends ConfigClient {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(CacheConfigClient.class);

	private ConcurrentHashMap<String, HashHolder<MemCachedClient>> _holderMap = new ConcurrentHashMap<String, HashHolder<MemCachedClient>>();

	public CacheConfigClient(String zkAddress, String identifier) {
		super(zkAddress, identifier);
		_logger.info("CacheConfigClient.CacheConfigClient zkAddress "
				+ zkAddress + " identifier " + identifier);
	}

	@Override
	protected boolean update(ConfigData newConfig) {
		_logger.info("CacheConfigClient.update");
		ConfigData cacheConfig = newConfig._childrenDatas
				.get(ConfigData.TRIPOD_CACHE);
		if (cacheConfig == null) {
			_logger.error("CacheConfigClient.update cacheConfig == null!");
			return false;
		}

		for (Entry<String, ConfigData> nConfig : newConfig._childrenDatas
				.get(ConfigData.TRIPOD_CACHE)._childrenDatas.entrySet()) {
			HashHolder<MemCachedClient> tHH = new HashHolder<MemCachedClient>(
					100);
			Map<String, MemCachedClient> tClients = new HashMap<String, MemCachedClient>();
			for (Entry<String, ConfigData> cConfig : nConfig.getValue()._childrenDatas
					.entrySet()) {
				String[] parts = cConfig.getKey().split(":");
				if (parts.length != 2) {
					return false;
				}
				SockIOPool pool = SockIOPool.getInstance(parts[0] + ":"
						+ parts[1]);
				String[] address = new String[] { parts[0] + ":" + parts[1] };
				if (!pool.isInitialized()) {
					pool.setServers(address);
					pool.setInitConn(1);
					pool.setMinConn(1);
					pool.setMaxConn(50);
					pool.setMaxIdle(1000 * 60 * 60 * 6);
					pool.setNagle(false);
					pool.setSocketTO(300);
					pool.setMaintSleep(30);
					pool.setSocketConnectTO(0);
					pool.initialize();
				}

				MemCachedClient tMcc = new MemCachedClient(parts[0] + ":"
						+ parts[1]);
				tClients.put(cConfig.getKey(), tMcc);
			}
			tHH.renew(tClients);
			_holderMap.put(nConfig.getKey(), tHH);
		}

		return true;
	}

	public MemCachedClient getPrx(String key, String namespaceId) {
		HashHolder<MemCachedClient> hh = _holderMap.get(namespaceId);
		if (hh != null) {
			return hh.get(key);
		}
		_logger.error("CacheConfigClient.getPrx no prx for namespaceId "
				+ namespaceId);
		return null;
	}

	public Map<MemCachedClient, List<String>> getPrxToKeysMap(
			List<String> keys, String namespaceId) {
		HashHolder<MemCachedClient> hh = _holderMap.get(namespaceId);
		if (hh != null) {
			return hh.get(keys);
		}
		_logger.error("CacheConfigClient.getPrx no prx for namespaceId "
				+ namespaceId);
		return null;
	}

}
