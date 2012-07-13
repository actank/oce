package com.renren.tripod.internalclient;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import com.renren.tripod.ProducerManagerPrx;
import com.renren.tripod.ProducerManagerPrxHelper;
import com.renren.tripod.TripodProducerNotSupportedException;
import com.renren.tripod.configclient.ProducerManagerConfigClient;
import com.renren.tripod.configclient.ConfigClientFactory;
import com.renren.tripod.configutil.ConfigData;
import com.xiaonei.xce.log.Tripod;

public class ProducerManagerClient {
	private ProducerManagerConfigClient _config = null;
	private String _zkAddress = null;
	private ConcurrentHashMap<String, String> _prxToccOrjavaMap = new ConcurrentHashMap<String, String>();

	public ProducerManagerClient(String zkAddress) {
		_zkAddress = zkAddress;
	}

	private ProducerManagerConfigClient getConfigClient() {
		if (_config == null) {
			synchronized (this) {
				_config = ConfigClientFactory
						.getProducerMangerConfigClient(_zkAddress);
			}
		}
		return _config;
	}

	public Map<String, byte[]> produce(List<String> keys, String businessId,
			boolean useMaster) throws TripodProducerNotSupportedException {
		return produce(keys, businessId, useMaster, 1000);
	}

	public Map<String, byte[]> produce(List<String> keys, String businessId,
			boolean useMaster, int timeout)
			throws TripodProducerNotSupportedException {
		Map<String, byte[]> res = new HashMap<String, byte[]>();
		String useWhich = _prxToccOrjavaMap.get(businessId);
		String newUseWhich = null;
		if (useWhich.isEmpty()) {
			Tripod.getLogger().info("ProducerManager.produce businessId " + businessId
					+ " useWhich is empty, useCc first");
			try {
				res = produceUseCc(keys, businessId, useMaster, timeout);
				newUseWhich = ConfigData.TRIPOD_PRODUCER_CC;
			} catch (TripodProducerNotSupportedException e) {
				Tripod.getLogger().info("ProducerManager.produce businessId " + businessId
						+ " useJava instead of useCc");
				try {
					res = produceUseJava(keys, businessId, useMaster, timeout);
					newUseWhich = ConfigData.TRIPOD_PRODUCER_JAVA;
				} catch (TripodProducerNotSupportedException e1) {
					throw e1;
				}
			}
		}

		if (useWhich.equals(ConfigData.TRIPOD_PRODUCER_CC)) {
			try {
				res = produceUseCc(keys, businessId, useMaster, timeout);
				newUseWhich = ConfigData.TRIPOD_PRODUCER_CC;
			} catch (TripodProducerNotSupportedException e) {
				Tripod.getLogger().error("ProducerManager.produce businessId "
						+ businessId + " useCc but NotSupportedException!");
				try {
					res = produceUseJava(keys, businessId, useMaster, timeout);
					newUseWhich = ConfigData.TRIPOD_PRODUCER_JAVA;
				} catch (TripodProducerNotSupportedException e1) {
					throw e1;
				}

			}
		}

		if (useWhich.equals(ConfigData.TRIPOD_PRODUCER_JAVA)) {
			try {
				res = produceUseJava(keys, businessId, useMaster, timeout);
				newUseWhich = ConfigData.TRIPOD_PRODUCER_JAVA;
			} catch (TripodProducerNotSupportedException e) {
				Tripod.getLogger().error("ProducerManager.produce businessId "
						+ businessId + " useJava but NotSupportedException!");
				try {
					res = produceUseCc(keys, businessId, useMaster, timeout);
					newUseWhich = ConfigData.TRIPOD_PRODUCER_CC;
				} catch (TripodProducerNotSupportedException e1) {
					throw e1;
				}
			}
		}
		if (!newUseWhich.isEmpty() && !useWhich.equals(newUseWhich)) {
			Tripod.getLogger().info("ProducerManager.produce businessId " + businessId
					+ " newUseWhich " + newUseWhich);
			_prxToccOrjavaMap.put(businessId, newUseWhich);
		}
		return res;
	}

	private Map<String, byte[]> produceUseCc(List<String> keys,
			String businessId, boolean useMaster, int timeout)
			throws TripodProducerNotSupportedException {
		ProducerManagerConfigClient config = getConfigClient();
		if (config == null) {
			Tripod.getLogger().error("ProducerManagerClient.produceUseCc config == null, businessId " + businessId);
			return null;
		}
		Map<ProducerManagerPrx, List<String>> prxsMap = config
				.getCCPrxToKeysMap(keys);
		Map<String, byte[]> res = new HashMap<String, byte[]>();
		for (Entry<ProducerManagerPrx, List<String>> ent : prxsMap.entrySet()) {
			ProducerManagerPrx prx = ProducerManagerPrxHelper.uncheckedCast(ent
					.getKey().ice_twoway().ice_timeout(timeout));
			Map<String, byte[]> singleRes = prx.produce(
					ent.getValue().toArray(new String[ent.getValue().size()]), businessId, useMaster);
			res.putAll(singleRes);
		}
		return res;
	}

	private Map<String, byte[]> produceUseJava(List<String> keys,
			String businessId, boolean useMaster, int timeout)
			throws TripodProducerNotSupportedException {
		ProducerManagerConfigClient config = getConfigClient();
		if (config == null) {
			Tripod.getLogger().error("ProducerManagerClient.produceUseJava config == null, businessId = " + businessId);
			return null;
		}
		Map<ProducerManagerPrx, List<String>> prxsMap = config
				.getCCPrxToKeysMap(keys);
		Map<String, byte[]> res = new HashMap<String, byte[]>();
		for (Entry<ProducerManagerPrx, List<String>> ent : prxsMap.entrySet()) {
			ProducerManagerPrx prx = ProducerManagerPrxHelper.uncheckedCast(ent
					.getKey().ice_twoway().ice_timeout(timeout));
			Map<String, byte[]> singleRes = prx.produce(
					ent.getValue().toArray(new String[ent.getValue().size()]), businessId, useMaster);
			res.putAll(singleRes);
		}
		return res;
	}

}
