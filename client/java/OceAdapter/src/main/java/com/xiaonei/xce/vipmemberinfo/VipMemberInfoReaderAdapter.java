package com.xiaonei.xce.vipmemberinfo;

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
import xce.vipinfo.VipMemberDataN;
import xce.vipinfo.VipMemberDataReaderPrx;
import xce.vipinfo.VipMemberDataReaderPrxHelper;

public final class VipMemberInfoReaderAdapter<T extends VipMemberInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoReader";

	private static final String service_name = "VM";
	
	private static int _interval = 120;

	protected VipMemberInfoFactory<T> _factory;

	public VipMemberInfoReaderAdapter(VipMemberInfoFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	protected VipMemberDataReaderPrx getVipMemberDataReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			VipMemberDataReaderPrx prx = VipMemberDataReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public Map<Integer, List<T> > getVipMemberInfoSeqMap(List<Integer> ids, List<Integer> types) {
		TimeCost cost = TimeCost.begin("VipMemberInfoReaderAdapter.getVipMemberInfoSeqMap id size: " + ids.size());
	    VipMemberDataReaderPrx prx = null;
	    Map<Integer, VipMemberDataN[]> data;
	    
	    try {
			prx = getVipMemberDataReaderPrx(0);
			data = prx.getVipMemberDataNSeqMap(MathUtil.list2array(ids), MathUtil.list2array(types));
			Map<Integer, List<T> > res = new HashMap<Integer, List<T> >();
			for(Entry<Integer, VipMemberDataN[]> ent : data.entrySet()) {
				List<T> dataList = new ArrayList<T>();
				for(VipMemberDataN sData : ent.getValue()) {
					T o = _factory.create();
					o.parse(sData);	
					dataList.add(o);
				}
				res.put(ent.getKey(), dataList);
			}
			return res;
		} catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".getVipMemberInfoSeqMap [" + prx + "] id=" 
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	}
	
	public Map<Integer, T> getOneTypeVipMemberInfoSeqMap(List<Integer> ids, int type) {
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(type);
		Map<Integer, List<T> > res = getVipMemberInfoSeqMap(ids, typeList);
		if(!res.isEmpty()) {
			Map<Integer, T> data = new HashMap<Integer, T>();
			for(Entry<Integer, List<T> > ent : res.entrySet()) {
				if(!ent.getValue().isEmpty()){
					data.put(ent.getKey(), ent.getValue().get(0));	
				}
			}
			return data;
		}
		return null;
	}
	
	public List<T> getVipMemberInfoSeq(int id, List<Integer> types) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		Map<Integer, List<T> > res = getVipMemberInfoSeqMap(idList, types);
		if(!res.isEmpty()) {
			return res.get(id);
		}
		return null;
	}
	
	public T getOneTypeVipMemberInfo(int id, int type) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(type);
		Map<Integer, List<T> > res = getVipMemberInfoSeqMap(idList, typeList);
		if(!res.isEmpty()) {
			if(!res.get(id).isEmpty()){
				return res.get(id).get(0);	
			}
		}
		return null;
  }
}
