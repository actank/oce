package com.renren.tripod.configutil;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.zookeeper.KeeperException;

public class ConfigManagerFactory {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ConfigManagerFactory.class);
	private static ConcurrentHashMap<String, ConfigManager> _cmManagerMap = new ConcurrentHashMap<String, ConfigManager>();

	public static synchronized ConfigManager get(String zkAddress)
			throws IOException, KeeperException, InterruptedException {
		_logger.info("ConfigManagerFactory.get zkAddress " + zkAddress);
		ConfigManager cm = _cmManagerMap.get(zkAddress);
		if (null == cm) {
			_logger.info("ConfigManagerFactory.get new ConfigManager for zkAddress "
					+ zkAddress);
			cm = new ConfigManager();
			try {
				cm.initialize(zkAddress);
				_cmManagerMap.put(zkAddress, cm);
			} catch (IOException e) {
				_logger.error("ConfigManagerFactory.get IOExcption " + e
						+ " for zkAddress " + zkAddress);
				throw e;
			} catch (KeeperException e) {
				_logger.error("ConfigManagerFactory.get KeeperException " + e
						+ " for zkAddress " + zkAddress);
				e.printStackTrace();
				throw e;
			} catch (InterruptedException e) {
				_logger.error("ConfigManagerFactory.get InterruptedException "
						+ e + " for zkAddress " + zkAddress);
				e.printStackTrace();
				throw e;
			}
		}
		return cm;
	}
}