package com.renren.tripod.configclient;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.renren.tripod.ProducerManagerPrx;
import com.renren.tripod.ProducerManagerPrxHelper;
import com.renren.tripod.configutil.ConfigClient;
import com.renren.tripod.configutil.ConfigData;

public class ProducerManagerConfigClient extends ConfigClient {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ProducerManagerConfigClient.class);
	private Ice.Communicator _ic = Ice.Util.initialize();
	private HashHolder<ProducerManagerPrx> _ccHashHolder = new HashHolder<ProducerManagerPrx>(
			100);
	private HashHolder<ProducerManagerPrx> _javaHashHolder = new HashHolder<ProducerManagerPrx>(
			100);

	public ProducerManagerConfigClient(String zkAddress, String identifier) {
		super(zkAddress, identifier);
		_logger.info("ProducerManagerConfigClient.ProducerManagerConfigClient zkAddress "
				+ zkAddress + " identifier " + identifier);
	}

	@Override
	protected boolean update(ConfigData newConfig) {
		_logger.info("ProducerManagerConfigClient.update");
		ConfigData cmConfig = newConfig._childrenDatas
				.get(ConfigData.TRIPOD_MANAGER);
		if (cmConfig == null) {
			_logger.error("ProducerManagerConfigClient.update cmConfig == null!");
			return false;
		}

		ConfigData ccConfig = cmConfig._childrenDatas
				.get(ConfigData.TRIPOD_PRODUCER_CC);
		if (ccConfig == null) {
			_logger.error("ProducerManagerConfigClient.update ccConfig == null!");
			return false;
		}

		ConfigData javaConfig = cmConfig._childrenDatas
				.get(ConfigData.TRIPOD_PRODUCER_JAVA);
		if (javaConfig == null) {
			_logger.error("ProducerManagerConfigClient.update javaConfig == null!");
			return false;
		}

		Map<String, ProducerManagerPrx> ccClients = new HashMap<String, ProducerManagerPrx>();
		for (Entry<String, ConfigData> ent : ccConfig._childrenDatas.entrySet()) {
			String[] parts = ent.getKey().split(":");
			if (parts.length != 2) {
				return false;
			}
			ProducerManagerPrx prx = ProducerManagerPrxHelper.uncheckedCast(_ic
					.stringToProxy("M:tcp -h " + parts[0] + " -p " + parts[1] + " -t " + 1000));
			if (prx == null) {
				_logger.error("ProducerManagerConfigClient.update prx == null for string "
						+ ent.getKey());
				return false;
			}
			ccClients.put(ent.getKey(), prx);
		}
		_ccHashHolder.renew(ccClients);

		Map<String, ProducerManagerPrx> javaClients = new HashMap<String, ProducerManagerPrx>();
		for (Entry<String, ConfigData> ent : javaConfig._childrenDatas
				.entrySet()) {
			String[] parts = ent.getKey().split(":");
			if (parts.length != 2) {
				return false;
			}
			ProducerManagerPrx prx = ProducerManagerPrxHelper.uncheckedCast(_ic
					.stringToProxy("M:tcp -h " + parts[0] + " -p " + parts[1] + " -t " + 1000));
			if (prx == null) {
				_logger.error("ProducerManagerConfigClient.update prx == null for string "
						+ ent.getKey());
				return false;
			}
			javaClients.put(ent.getKey(), prx);
		}
		_javaHashHolder.renew(javaClients);
		return true;
	}

	public ProducerManagerPrx getCCPrx(String key) {
		return _ccHashHolder.get(key);
	}

	public ProducerManagerPrx getJavaPrx(String key) {
		return _javaHashHolder.get(key);
	}

	public Map<ProducerManagerPrx, List<String>> getCCPrxToKeysMap(
			List<String> keys) {
		return _ccHashHolder.get(keys);
	}

	public Map<ProducerManagerPrx, List<String>> getJavaPrxToKeysMap(
			List<String> keys) {
		return _javaHashHolder.get(keys);
	}

}
