package com.renren.tripod.configclient;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import redis.clients.jedis.JedisPool;

import com.renren.tripod.configutil.ConfigClient;
import com.renren.tripod.configutil.ConfigData;

/**
 * RedisCacheConfigClient
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class RedisCacheConfigClient extends ConfigClient {
    private static org.apache.commons.logging.Log logger = org.apache.commons.logging.LogFactory
	    .getLog(CacheConfigClient.class);

    private ConcurrentHashMap<String, HashHolder<JedisPool>> namespaceToHolderMap = new ConcurrentHashMap<String, HashHolder<JedisPool>>();

    public RedisCacheConfigClient(String zkAddress, String identifier) {
	super(zkAddress, identifier);
    }

    @Override
    protected boolean update(ConfigData newConfig) {
	ConfigData cacheConfig = newConfig._childrenDatas
		.get(ConfigData.TRIPOD_CACHE);
	if (cacheConfig == null) {
	    logger.error("CacheConfigClient.update cacheConfig == null!");
	    return false;
	}
	for (Entry<String, ConfigData> namespaceConfig : cacheConfig._childrenDatas
		.entrySet()) {
	    String namespace = namespaceConfig.getKey();
	    Map<String, JedisPool> poolMap = new HashMap<String, JedisPool>();
	    for (Entry<String, ConfigData> nodeConfig : namespaceConfig
		    .getValue()._childrenDatas.entrySet()) {
		String configString = nodeConfig.getKey();
		String[] parts = configString.split(":");
		if (parts.length != 2) {
		    logger.error("error config string:" + configString);
		    continue;
		}
		String host = parts[0];
		int port = -1;
		try {
		    port = Integer.parseInt(parts[1]);
		} catch (Exception e) {
		    logger.error("redis port config is not number");
		    continue;
		}
		JedisPool pool = new JedisPool(host, port);
		poolMap.put(configString, pool);
	    }
	    if (!poolMap.isEmpty()) {
		HashHolder<JedisPool> holder = new HashHolder<JedisPool>(100);
		holder.renew(poolMap);
		namespaceToHolderMap.put(namespace, holder);
	    }
	}
	return true;
    }

    public JedisPool getPrx(String key, String namespace) {
	HashHolder<JedisPool> holder = namespaceToHolderMap.get(namespace);
	if (holder != null) {
	    return holder.get(key);
	}
	return null;
    }

}
