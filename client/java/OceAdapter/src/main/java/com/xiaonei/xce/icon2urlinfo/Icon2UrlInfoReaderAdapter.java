package com.xiaonei.xce.icon2urlinfo;

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
import xce.vipinfo.Icon2UrlDataReaderPrx;
import xce.vipinfo.Icon2UrlDataReaderPrxHelper;

public final class Icon2UrlInfoReaderAdapter<T extends Icon2UrlInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoReader";

	private static final String service_name = "IU";
	
	private static int _interval = 120;

	protected Icon2UrlInfoFactory<T> _factory;

	public Icon2UrlInfoReaderAdapter(Icon2UrlInfoFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	protected Icon2UrlDataReaderPrx getIcon2UrlDataReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			Icon2UrlDataReaderPrx prx = Icon2UrlDataReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public Map<Integer, T> getIcon2UrlInfoMap(List<Integer> ids) {
		TimeCost cost = TimeCost.begin("Icon2UrlInfoReaderAdapter.getIcon2UrlInfoMap id size: " + ids.size());
	    Icon2UrlDataReaderPrx prx = null;
	    Map<Integer, String> data;
	    
	    try {
			prx = getIcon2UrlDataReaderPrx(0);
			data = prx.getIcon2UrlDataMap(MathUtil.list2array(ids));
			Map<Integer, T> res = new HashMap<Integer, T>();
			for(Entry<Integer,String> ent : data.entrySet()) {
				T o = _factory.create();
				o.setIconId(ent.getKey());
				o.setHideUrl(ent.getValue());
				res.put(ent.getKey(), o);
			}
			return res;
		} catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".getIcon2UrlInfo [" + prx + "] id=" 
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	}
	
	public T getIcon2UrlInfo(int id) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		Map<Integer, T> res = getIcon2UrlInfoMap(idList);
		if(!res.isEmpty()) {
			return res.get(id);
		}
		return null;
  }
}
