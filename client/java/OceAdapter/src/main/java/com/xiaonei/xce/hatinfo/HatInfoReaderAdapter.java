package com.xiaonei.xce.hatinfo;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.tools.MathUtil;
import xce.vipinfo.HatData;
import xce.vipinfo.HatDataReaderPrx;
import xce.vipinfo.HatDataReaderPrxHelper;

public final class HatInfoReaderAdapter<T extends HatInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoReader";

	private static final String service_name = "IH";

	private static int _interval = 120;

	protected HatInfoFactory<T> _factory;

	public HatInfoReaderAdapter(HatInfoFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	protected HatDataReaderPrx getHatDataReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 300);
		if (prx0 != null) {
			HatDataReaderPrx prx = HatDataReaderPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public Map<Integer, T> getHatInfoMap(List<Integer> ids) {
		TimeCost cost = TimeCost.begin("HatInfoReaderAdapter.getHatInfoMap id size: " + ids.size());
		HatDataReaderPrx prx = null;
		Map<Integer, HatData> data;

		try {
			prx = getHatDataReaderPrx(0);
			data = prx.getHatDataMap(MathUtil.list2array(ids));
			Map<Integer, T> res = new HashMap<Integer, T>();
			for (Entry<Integer, HatData> ent : data.entrySet()) {
				T o = _factory.create();
				o.parse(ent.getValue());
				res.put(ent.getKey(), o);
			}
			return res;
		} catch (TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getHatInfo [" + prx + "] id=" + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getHatInfo(int id) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		Map<Integer, T> res = getHatInfoMap(idList);
		if (!res.isEmpty()) {
			return res.get(id);
		}
		return null;
	}

}
