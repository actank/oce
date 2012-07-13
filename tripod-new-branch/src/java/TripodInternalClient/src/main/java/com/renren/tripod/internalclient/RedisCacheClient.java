package com.renren.tripod.internalclient;

import java.util.ArrayList;
import java.util.List;

import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;

import com.renren.tripod.configclient.ConfigClientFactory;
import com.renren.tripod.configclient.RedisCacheConfigClient;
import com.xiaonei.xce.log.Tripod;

/**
 * RedisCacheClient
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class RedisCacheClient {
    private RedisCacheConfigClient configClient = null;
    private String zkAddress = null;

    public RedisCacheClient(String zkAddress) {
	this.zkAddress = zkAddress;
    }

    private RedisCacheConfigClient getConfigClient() {
	if (configClient == null) {
	    synchronized (this) {
		if (configClient == null) {
		    configClient = ConfigClientFactory
			    .getRedisCacheConfigClient(zkAddress);
		}
	    }
	}
	return configClient;
    }

    /**
     * 取一般列表接口
     * 
     * @param key
     * @param begin
     * @param limit
     * @param namespaceId
     * @param biz_id
     * @return
     */
    public List<String> getList(String key, int begin, int limit,
	    String namespaceId, String biz_id) {
	List<String> value = new ArrayList<String>();
	RedisCacheConfigClient configClient = getConfigClient();
	if (configClient == null) {
	    Tripod.getLogger().error(
		    "RedisCacheClient.getConfig == null, namespaceId "
			    + namespaceId + " bussinessId " + biz_id);
	    return value;
	}
	JedisPool pool = configClient.getPrx(key, namespaceId);
	if (pool == null) {
	    return value;
	}
	int startPosition = begin;
	int endPosition = limit == -1 ? -1 : (begin + limit - 1);
	Jedis jedis = pool.getResource();
	String cacheKey = getCacheKey(key, biz_id);
	value = jedis.lrange(cacheKey, startPosition, endPosition);
	pool.returnResource(jedis);
	return value;
    }

    /**
     * 取int列表接口
     * 
     * @param key
     * @param begin
     * @param limit
     * @param namespaceId
     * @param biz_id
     * @return
     */
    public List<Integer> getIntList(String key, int begin, int limit,
	    String namespaceId, String biz_id) {
	List<Integer> value = new ArrayList<Integer>();
	RedisCacheConfigClient configClient = getConfigClient();
	if (configClient == null) {
	    Tripod.getLogger().error(
		    "RedisCacheClient.getConfig == null, namespaceId "
			    + namespaceId + " bussinessId " + biz_id);
	    return value;
	}
	JedisPool pool = configClient.getPrx(key, namespaceId);
	if (pool == null) {
	    return value;
	}
	int startPosition = begin;
	int endPosition = limit == -1 ? -1 : (begin + limit - 1);
	Jedis jedis = pool.getResource();
	String cacheKey = getCacheKey(key, biz_id);
	List<String> strListValue = jedis.lrange(cacheKey, startPosition,
		endPosition);
	if (strListValue != null && !strListValue.isEmpty()) {
	    for (String v : strListValue) {
		try {
		    value.add(Integer.parseInt(v));
		} catch (Exception e) {
		    continue;
		}
	    }
	}
	pool.returnResource(jedis);
	return value;
    }

    private static String SEPARATOR = "_";

    private static String getCacheKey(String rawKey, String biz_id) {
	return biz_id + SEPARATOR + rawKey;
    }

    private static String getRawKey(String cacheKey, String biz_id) {
	return cacheKey.substring(biz_id.length() + 1, cacheKey.length());
    }
}
