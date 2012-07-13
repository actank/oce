package com.renren.tripod.configclient;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.renren.tripod.CacheManagerPrx;
import com.renren.tripod.CacheManagerPrxHelper;
import com.renren.tripod.configutil.ConfigClient;
import com.renren.tripod.configutil.ConfigData;

public class CacheManagerConfigClient extends ConfigClient {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(CacheManagerConfigClient.class);

	private Ice.Communicator _ic = Ice.Util.initialize();
	private HashHolder<CacheManagerPrx> _hashHolder = new HashHolder<CacheManagerPrx>(
			100);

	public CacheManagerConfigClient(String zkAddress, String identifier) {
		super(zkAddress, identifier);
		_logger.info("CacheManagerConfigClient.CacheManagerConfigClient zkAddress "
				+ zkAddress + " identifier " + identifier);
	}

	@Override
	protected boolean update(ConfigData newConfig) {
		_logger.info("CacheManagerConfigClient.update");
		ConfigData cmConfig = newConfig._childrenDatas
				.get(ConfigData.TRIPOD_MANAGER);
		if (cmConfig == null) {
			_logger.error("CacheManagerConfigClient.update cmConfig == null!");
			return false;
		}
		Map<String, CacheManagerPrx> clients = new HashMap<String, CacheManagerPrx>();
		for (Entry<String, ConfigData> ent : cmConfig._childrenDatas.entrySet()) {
			String[] parts = ent.getKey().split(":");
			if (parts.length != 2) {
				return false;
			}
			CacheManagerPrx prx = CacheManagerPrxHelper.uncheckedCast(_ic
					.stringToProxy("M:tcp -h " + parts[0] + " -p " + parts[1] + " -t " + 1000));
			if (prx == null) {
				_logger.error("CacheManagerConfigClient.update prx == null for string "
						+ ent.getKey());
				return false;
			}
			clients.put(ent.getKey(), prx);
		}
		_hashHolder.renew(clients);
		return true;
	}

	public CacheManagerPrx getPrx(String key) {
		return _hashHolder.get(key);
	}

	public Map<CacheManagerPrx, List<String>> getPrxToKeysMap(List<String> keys) {
		return _hashHolder.get(keys);
	}

}
