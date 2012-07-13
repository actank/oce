package com.renren.tripod.configutil;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.KeeperException.NoNodeException;

public class ConfigManager {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ConfigManager.class);

	private String _zkAddress = "";
	private ConfigData _config = null;
	private List<ConfigClient> _clients = new ArrayList<ConfigClient>();
	private ReadWriteLock _clientsLock = new ReentrantReadWriteLock();
	private ReadWriteLock _lock = new ReentrantReadWriteLock();

	public boolean initialize(String zkAddress) throws IOException,
			KeeperException, InterruptedException {
		_logger.info("ConfigManager.initialize zkAddress " + zkAddress);
		_zkAddress = zkAddress;
		ZkManager zm = ZkManagerFactory.get(_zkAddress);
		return updateConfig(zm.getZk());
	}

	public void registerClient(ConfigClient cc) {
		_logger.info("ConfigManager.registerClient  " + cc.toString());
		Lock wLock = _clientsLock.writeLock();
		wLock.lock();
		_clients.add(cc);
		wLock.unlock();
	}

	public boolean updateConfig(ZooKeeper zk) throws KeeperException,
			InterruptedException {
		_logger.info("ConfigManager.updateConfig");
		try {
			return init(zk);
		} catch (NoNodeException e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateConfig " + e);
			throw e;
		} catch (KeeperException e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateConfig " + e);
			throw e;
		} catch (InterruptedException e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateConfig " + e);
			throw e;
		} catch (Throwable e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateConfig " + e);
		}
		return false;
	}

	private boolean init(ZooKeeper zk) throws KeeperException,
			InterruptedException {
		_logger.info("ConfigManager.init");
		ConfigData oldConfig = getConfigData();
		if (oldConfig == null) {
			_logger.info("ConfigManager.init oldConfig == null");
			ConfigData newCacheManagerConfig = updateCacheManagerConfig(null,
					zk);
			ConfigData newProducerManagerConfig = updateProducerManagerConfig(
					null, zk);
			ConfigData newCacheConfig = updateCacheConfig(null, zk);

			ConfigData newConfig = new ConfigData();
			if (newCacheManagerConfig != null
					&& newProducerManagerConfig != null
					&& newCacheConfig != null) {
				newConfig._childrenDatas.put(ConfigData.TRIPOD_MANAGER,
						newCacheManagerConfig);
				newConfig._childrenDatas.put(ConfigData.TRIPOD_PRODUCER,
						newProducerManagerConfig);
				newConfig._childrenDatas.put(ConfigData.TRIPOD_CACHE,
						newCacheConfig);
				return setConfigData(newConfig);
			}
			_logger.error("ConfigManager.init failed");
			return false;
		}

		ConfigData oldCacheManagerConfig = oldConfig._childrenDatas
				.get(ConfigData.TRIPOD_MANAGER);
		if (oldCacheManagerConfig == null) {
			_logger.error("ConfigManager.init no cachemanager in oldConfig");
			return false;
		}
		ConfigData newCacheManagerConfig = updateCacheManagerConfig(
				oldCacheManagerConfig, zk);

		ConfigData oldProducerManagerConfig = oldConfig._childrenDatas
				.get(ConfigData.TRIPOD_PRODUCER);
		if (oldProducerManagerConfig == null) {
			_logger.error("ConfigManager.init no producermanager in oldConfig");
			return false;
		}
		ConfigData newProducerManagerConfig = updateProducerManagerConfig(
				oldProducerManagerConfig, zk);

		ConfigData oldCacheConfig = oldConfig._childrenDatas
				.get(ConfigData.TRIPOD_CACHE);
		if (oldCacheConfig == null) {
			_logger.error("ConfigManager.init no cache in oldConfig");
			return false;
		}
		ConfigData newCacheConfig = updateCacheConfig(oldCacheConfig, zk);

		boolean updateFlag = false;
		ConfigData newConfig = oldConfig;

		if (newCacheManagerConfig != null) {
			_logger.info("ConfigManager.init cachemanager has new info");
			newConfig._childrenDatas.put(ConfigData.TRIPOD_MANAGER,
					newCacheManagerConfig);
			updateFlag = true;
		}

		if (newProducerManagerConfig != null) {
			_logger.info("ConfigManager.init producermanager has new info");
			newConfig._childrenDatas.put(ConfigData.TRIPOD_PRODUCER,
					newProducerManagerConfig);
			updateFlag = true;
		}

		if (newCacheConfig != null) {
			_logger.info("ConfigManager.init cache has new info");
			newConfig._childrenDatas.put(ConfigData.TRIPOD_CACHE,
					newCacheConfig);
			updateFlag = true;
		}

		_logger.info("ConfigManager.init updateFlag " + updateFlag);
		if (updateFlag) {
			setConfigData(newConfig);
		}

		return true;
	}

	private ConfigData updateCacheManagerConfig(ConfigData oldConfig,
			ZooKeeper zk) throws KeeperException, InterruptedException {
		_logger.info("ConfigManager.updateCacheManagerConfig");
		ConfigData newConfig;
		if (oldConfig == null) {
			_logger.info("ConfigManager.updateCacheManagerConfig oldConfig == null");
			newConfig = new ConfigData();
			newConfig._name = ConfigData.TRIPOD_MANAGER;
		} else {
			newConfig = oldConfig.clone();
		}

		try {
			if (zk.exists("/" + ConfigData.TRIPOD_MANAGER, true) == null) {
				_logger.error("ConfigManager.updateCacheManagerConfig path "
						+ "/" + ConfigData.TRIPOD_MANAGER + " not in zk!");
				return null;
			}

			String version = new String(zk.getData("/"
					+ ConfigData.TRIPOD_MANAGER, true, null));

			if (oldConfig != null) {
				if (version.equals(oldConfig._version)) {
					_logger.info("ConfigManager.updateCacheManagerConfig oldConfig and newConfig hava same version");
					return null;
				}
			}

			newConfig._version = version;

			List<String> nodeNames = zk.getChildren("/"
					+ ConfigData.TRIPOD_MANAGER, false);

			newConfig._childrenDatas.clear();
			for (String nodeName : nodeNames) {
				ConfigData tConfig = new ConfigData();
				tConfig._name = nodeName;
				newConfig._childrenDatas.put(nodeName, tConfig);
			}
		} catch (NoNodeException e) {
			_logger.error("ConfigManager.updateCacheManagerConfig " + e);
			throw e;
		} catch (KeeperException e) {
			_logger.error("ConfigManager.updateCacheManagerConfig " + e);
			throw e;
		} catch (IllegalArgumentException e) {
			_logger.error("ConfigManager.updateCacheManagerConfig " + e);
			throw e;
		} catch (InterruptedException e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateCacheManagerConfig " + e);
			throw e;
		} catch (Throwable e) {
			_logger.error("ConfigManager.updateCacheManagerConfig " + e);
		}
		_logger.info("ConfigManager.updateCacheManagerConfig " + newConfig);
		return newConfig;
	}

	private ConfigData updateProducerManagerConfig(ConfigData oldConfig,
			ZooKeeper zk) throws KeeperException, InterruptedException {
		_logger.info("ConfigManager.updateProducerManagerConfig");
		ConfigData newConfig;
		if (oldConfig == null) {
			_logger.info("ConfigManager.updateProducerManagerConfig oldConfig == null");
			newConfig = new ConfigData();
			newConfig._name = ConfigData.TRIPOD_PRODUCER;
		} else {
			newConfig = oldConfig.clone();
		}

		try {

			if (zk.exists("/" + ConfigData.TRIPOD_PRODUCER + "/"
					+ ConfigData.TRIPOD_PRODUCER_CC, true) == null) {
				_logger.error("ConfigManager.updateProducerManagerConfig path "
						+ "/" + ConfigData.TRIPOD_PRODUCER + "/"
						+ ConfigData.TRIPOD_PRODUCER_CC + " not in zk!");
				return null;
			}

			if (zk.exists("/" + ConfigData.TRIPOD_PRODUCER + "/"
					+ ConfigData.TRIPOD_PRODUCER_JAVA, true) == null) {
				_logger.error("ConfigManager.updateProducerManagerConfig path "
						+ "/" + ConfigData.TRIPOD_PRODUCER + "/"
						+ ConfigData.TRIPOD_PRODUCER_JAVA + " not in zk!");
				return null;
			}

			String ccVersion = new String(zk.getData("/"
					+ ConfigData.TRIPOD_PRODUCER + "/"
					+ ConfigData.TRIPOD_PRODUCER_CC, true, null));

			String javaVersion = new String(zk.getData("/"
					+ ConfigData.TRIPOD_PRODUCER + "/"
					+ ConfigData.TRIPOD_PRODUCER_JAVA, true, null));

			boolean ccUpdateFlag = true;
			boolean javaUpdateFlag = true;
			ConfigData ccConfig = null;
			ConfigData javaConfig = null;
			if (oldConfig != null) {
				ccConfig = oldConfig._childrenDatas
						.get(ConfigData.TRIPOD_PRODUCER_CC);
				if (ccConfig == null) {
					_logger.error("ConfigManager.updateProducerManagerConfig no cc producer in oldConfig!");
					return null;
				}
				javaConfig = oldConfig._childrenDatas
						.get(ConfigData.TRIPOD_PRODUCER_JAVA);
				if (javaConfig == null) {
					_logger.error("ConfigManager.updateProducerManagerConfig no java producer in oldConfig!");
					return null;
				}
				if (ccVersion.equals(ccConfig._version)) {
					_logger.info("ConfigManager.updateProducerManagerConfig oldConfig and newConfig have same version for cc");
					ccUpdateFlag = false;
				}
				if (javaVersion.equals(javaConfig._version)) {
					_logger.info("ConfigManager.updateProducerManagerConfig oldConfig and newConfig have same version for java");
					ccUpdateFlag = false;
				}
			}

			if (!ccUpdateFlag && !javaUpdateFlag) {
				_logger.info("ConfigManager.updateProducerManagerConfig ccUpdateFlag "
						+ ccUpdateFlag + " javaUpdateFlag" + javaUpdateFlag);
				return null;
			}

			if (ccUpdateFlag) {
				_logger.info("ConfigManager.updateProducerManagerConfig ccUpdateFlag "
						+ ccUpdateFlag);
				if (ccConfig == null) {
					_logger.info("ConfigManager.updateProducerManagerConfig ccConfig == null");
					ccConfig = new ConfigData();
					ccConfig._name = ConfigData.TRIPOD_PRODUCER_CC;
				}
				ccConfig._version = ccVersion;
				ccConfig._childrenDatas.clear();

				List<String> nodeNames = zk.getChildren("/"
						+ ConfigData.TRIPOD_PRODUCER + "/"
						+ ConfigData.TRIPOD_PRODUCER_CC, false);

				for (String nodeName : nodeNames) {
					ConfigData tConfig = new ConfigData();
					tConfig._name = nodeName;
					ccConfig._childrenDatas.put(nodeName, tConfig);
				}
				newConfig._childrenDatas.put(ConfigData.TRIPOD_PRODUCER_CC,
						ccConfig);
			}

			if (javaUpdateFlag) {
				_logger.info("ConfigManager.updateProducerManagerConfig javaUpdateFlag "
						+ javaUpdateFlag);
				if (javaConfig == null) {
					_logger.info("ConfigManager.updateProducerManagerConfig javaConfig == null");
					javaConfig = new ConfigData();
					javaConfig._name = ConfigData.TRIPOD_PRODUCER_JAVA;
				}
				javaConfig._version = javaVersion;
				javaConfig._childrenDatas.clear();

				List<String> nodeNames = zk.getChildren("/"
						+ ConfigData.TRIPOD_PRODUCER + "/"
						+ ConfigData.TRIPOD_PRODUCER_JAVA, false);

				for (String nodeName : nodeNames) {
					ConfigData tConfig = new ConfigData();
					tConfig._name = nodeName;
					javaConfig._childrenDatas.put(nodeName, tConfig);
				}
				newConfig._childrenDatas.put(ConfigData.TRIPOD_PRODUCER_JAVA,
						javaConfig);
			}
		} catch (NoNodeException e) {
			_logger.error("ConfigManager.updateProducerManagerConfig " + e);
			throw e;
		} catch (KeeperException e) {
			_logger.error("ConfigManager.updateProducerManagerConfig " + e);
			throw e;
		} catch (IllegalArgumentException e) {
			_logger.error("ConfigManager.updateProducerManagerConfig " + e);
			throw e;
		} catch (InterruptedException e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateProducerManagerConfig " + e);
			throw e;
		} catch (Throwable e) {
			_logger.error("ConfigManager.updateProducerManagerConfig " + e);
		}
		_logger.info("ConfigManager.updateProducerManagerConfig " + newConfig);
		return newConfig;
	}

	private ConfigData updateCacheConfig(ConfigData oldConfig, ZooKeeper zk)
			throws KeeperException, InterruptedException {
		_logger.info("ConfigManager.updateCacheConfig");
		ConfigData newConfig = new ConfigData();
		newConfig._name = ConfigData.TRIPOD_CACHE;
		if (oldConfig == null) {
			_logger.info("ConfigManager.updateCacheConfig oldConfig == null");
		}

		try {

			boolean updateFlag = false;
			if (zk.exists("/" + ConfigData.TRIPOD_CACHE, true) == null) {
				_logger.error("ConfigManager.updateCacheConfig path " + "/"
						+ ConfigData.TRIPOD_CACHE + " not in zk!");
				return null;
			}

			List<String> nodeNames = zk.getChildren("/"
					+ ConfigData.TRIPOD_CACHE, false);

			for (String nodeName : nodeNames) {
				boolean tUpdateFlag = false;
				String version = null;
				if (oldConfig == null) {
					_logger.info("ConfigManager.updateCacheConfig oldConfig == null");
					tUpdateFlag = true;
					updateFlag = true;
				} else {
					ConfigData oldNConfig = oldConfig._childrenDatas
							.get(nodeName);
					if (oldNConfig != null) {
						version = new String(zk.getData("/"
								+ ConfigData.TRIPOD_CACHE + "/" + nodeName,
								true, null));
						if (!version.equals(oldNConfig._version)) {
							_logger.info("ConfigManager.updateCacheConfig newConfig and oldConfig hava different version");
							tUpdateFlag = true;
							updateFlag = true;
						}else {
							newConfig._childrenDatas.put(nodeName, oldNConfig);
						}
					}else {
						tUpdateFlag = true;
						updateFlag = true;
					}
				}
				if (tUpdateFlag == true) {
					_logger.info("ConfigManager.updateCacheConfig tUpdateFlag "
							+ tUpdateFlag);
					ConfigData tConfig = new ConfigData();
					tConfig._name = nodeName;
					tConfig._version = version;
					List<String> tNodeNames = zk.getChildren("/"
							+ ConfigData.TRIPOD_CACHE + "/" + nodeName, false);
					for (String tNodeName : tNodeNames) {
						ConfigData ttConfig = new ConfigData();
						ttConfig._name = tNodeName;
						tConfig._childrenDatas.put(tNodeName, ttConfig);
					}
					newConfig._childrenDatas.put(nodeName, tConfig);
				}
			}
			
			if (!updateFlag) {
				_logger.info("ConfigManager.updateCacheConfig updateFlag "
						+ updateFlag);
				return null;
			}
		} catch (NoNodeException e) {
			_logger.error("ConfigManager.updateCacheConfig " + e);
			throw e;
		} catch (KeeperException e) {
			_logger.error("ConfigManager.updateCacheConfig " + e);
			throw e;
		} catch (IllegalArgumentException e) {
			_logger.error("ConfigManager.updateCacheConfig " + e);
			throw e;
		} catch (InterruptedException e) {
			e.printStackTrace();
			_logger.error("ConfigManager.updateCacheConfig " + e);
			throw e;
		} catch (Throwable e) {
			_logger.error("ConfigManager.updateCacheConfig " + e);
		}
		_logger.info("ConfigManager.updateCacheConfig " + newConfig);
		return newConfig;
	}

	public ConfigData getConfigData() {
		_logger.info("ConfigManager.getData");
		Lock rLock = _lock.readLock();
		try {
			rLock.lock();
			if (_config != null) {
				return _config.clone();
			} else {
				_logger.error("ConfigManager.getData _config is null!");
				return null;
			}
		} catch (Exception e) {
			_logger.error("ConfigManager.getData Exception " + e);
			e.printStackTrace();
		}finally {
			rLock.unlock();
		}
		return null;
	}

	private boolean setConfigData(ConfigData config) {
		_logger.info("ConfigManager.setConfigData");
		{
			Lock wLock = _lock.writeLock();
			wLock.lock();
			_config = config;
			wLock.unlock();
		}
		Lock rLock = _clientsLock.readLock();
		rLock.lock();
		for (ConfigClient cc : _clients) {
			cc.notifyHandle();
		}
		rLock.unlock();
		return true;
	}

}
