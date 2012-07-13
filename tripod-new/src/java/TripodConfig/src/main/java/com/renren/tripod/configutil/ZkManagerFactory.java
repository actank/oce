package com.renren.tripod.configutil;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public class ZkManagerFactory {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ZkManagerFactory.class);
	private static ConcurrentHashMap<String, ZkManager> _zkManagerMap = new ConcurrentHashMap<String, ZkManager>();

	public static synchronized ZkManager get(String zkAddress)
			throws IOException {
		_logger.info("ZkManagerFactory.get zkAddress " + zkAddress);
		ZkManager zm = _zkManagerMap.get(zkAddress);
		if (null == zm) {
			_logger.info("ZkManagerFactory.get new ZkManager for zkAddress "
					+ zkAddress);
			zm = new ZkManager();
			try {
				if(zm.initialize(zkAddress)) {
					_zkManagerMap.put(zkAddress, zm);	
				}else {
					return null;
				}
			} catch (IOException e) {
				_logger.error("ZkManagerFactory.get IOException " + e
						+ " for zkAddress " + zkAddress);
				throw e;
			}
		}
		return zm;
	}
}