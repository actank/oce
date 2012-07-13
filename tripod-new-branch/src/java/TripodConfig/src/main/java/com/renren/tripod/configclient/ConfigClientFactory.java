package com.renren.tripod.configclient;

import java.util.concurrent.ConcurrentHashMap;

import com.renren.tripod.configutil.ConfigData;

public class ConfigClientFactory {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(ConfigClientFactory.class);
    private static ConcurrentHashMap<String, CacheManagerConfigClient> _cacheManagerConfigClientMap = new ConcurrentHashMap<String, CacheManagerConfigClient>();
    private static ConcurrentHashMap<String, ProducerManagerConfigClient> _producerManagerConfigClientMap = new ConcurrentHashMap<String, ProducerManagerConfigClient>();
    private static ConcurrentHashMap<String, CacheConfigClient> _cacheConfigClientMap = new ConcurrentHashMap<String, CacheConfigClient>();

    //
    private static ConcurrentHashMap<String, RedisCacheConfigClient> redisCacheConfigClientMap = new ConcurrentHashMap<String, RedisCacheConfigClient>();

    public static synchronized CacheManagerConfigClient getCacheMangerConfigClient(
	    String zkAddress) {
	_logger.info("ConfigClientFactory.getCacheManagerConfigClient zkAddress "
		+ zkAddress);
	CacheManagerConfigClient cc = _cacheManagerConfigClientMap
		.get(zkAddress);
	if (null == cc) {
	    _logger.info("ConfigClientFactory.getCacheManagerConfigClient new CacheManagerConfigClient for zkAddress "
		    + zkAddress);
	    cc = new CacheManagerConfigClient(zkAddress,
		    ConfigData.TRIPOD_MANAGER);
	    if (cc.initialize()) {
		_cacheManagerConfigClientMap.put(zkAddress, cc);
		return cc;
	    } else {
		return null;
	    }
	}
	return cc;
    }

    public static synchronized ProducerManagerConfigClient getProducerMangerConfigClient(
	    String zkAddress) {
	_logger.info("ConfigClientFactory.getProducerManagerConfigClient zkAddress "
		+ zkAddress);
	ProducerManagerConfigClient cc = _producerManagerConfigClientMap
		.get(zkAddress);
	if (null == cc) {
	    _logger.info("ConfigClientFactory.getProducerManagerConfigClient new ProducerManagerConfigClient for zkAddress "
		    + zkAddress);
	    cc = new ProducerManagerConfigClient(zkAddress,
		    ConfigData.TRIPOD_PRODUCER);
	    if (cc.initialize()) {
		_producerManagerConfigClientMap.put(zkAddress, cc);
		return cc;
	    } else {
		return null;
	    }
	}
	return cc;
    }

    public static synchronized CacheConfigClient getCacheConfigClient(
	    String zkAddress) {
	_logger.info("ConfigClientFactory.getCacheConfigClient zkAddress "
		+ zkAddress);
	CacheConfigClient cc = _cacheConfigClientMap.get(zkAddress);
	if (null == cc) {
	    _logger.info("ConfigClientFactory.getCacheConfigClient new CacheConfigClient for zkAddress "
		    + zkAddress);
	    cc = new CacheConfigClient(zkAddress, ConfigData.TRIPOD_CACHE);

	    if (cc.initialize()) {
		_cacheConfigClientMap.put(zkAddress, cc);
		return cc;
	    } else {
		return null;
	    }
	}
	return cc;
    }

    public static synchronized RedisCacheConfigClient getRedisCacheConfigClient(
	    String zkAddress) {
	_logger.info("ConfigClientFactory.getRedisCacheConfigClient zkAddress "
		+ zkAddress);
	RedisCacheConfigClient configClient = redisCacheConfigClientMap
		.get(zkAddress);
	if (configClient != null) {
	    return configClient;
	} else {
	    _logger.info("ConfigClientFactory.getRedisCacheConfigClient new RedisCacheConfigClient for zkAddress "
		    + zkAddress);
	    configClient = new RedisCacheConfigClient(zkAddress,
		    ConfigData.TRIPOD_CACHE);
	    if (configClient.initialize()) {
		redisCacheConfigClientMap.put(zkAddress, configClient);
		return configClient;
	    } else {
		return null;
	    }
	}
    }

}