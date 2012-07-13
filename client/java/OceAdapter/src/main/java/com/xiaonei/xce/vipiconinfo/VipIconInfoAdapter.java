package com.xiaonei.xce.vipiconinfo;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import Ice.TimeoutException;

import com.xiaonei.xce.icon2urlinfo.DefaultIcon2UrlInfo;
import com.xiaonei.xce.icon2urlinfo.DefaultIcon2UrlInfoFactory;
import com.xiaonei.xce.icon2urlinfo.Icon2UrlInfo;
import com.xiaonei.xce.icon2urlinfo.Icon2UrlInfoAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.vipmemberinfo.DefaultVipMemberInfo;
import com.xiaonei.xce.vipmemberinfo.DefaultVipMemberInfoFactory;
import com.xiaonei.xce.vipmemberinfo.VipMemberInfo;
import com.xiaonei.xce.vipmemberinfo.VipMemberInfoAdapter;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.tools.MathUtil;
import xce.vipinfo.VipIconData;
import xce.vipinfo.VipIconDataReaderPrx;
import xce.vipinfo.VipIconDataReaderPrxHelper;

public final class VipIconInfoAdapter<T extends VipIconInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoReader";

	private static final String service_name = "VI";
	
	private static int _interval = 120;

	protected VipIconInfoFactory<T> _factory;

	public VipIconInfoAdapter(VipIconInfoFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	protected VipIconDataReaderPrx getVipIconDataReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			VipIconDataReaderPrx prx = VipIconDataReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public Map<Integer, T> getOneTypeVipIconInfoMap(List<Integer> ids, int type) {
		TimeCost cost = TimeCost.begin("VipIconInfoReaderAdapter.getVipIconInfoSeqMap id size: " + ids.size());
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(type);
		Map<Integer, List<T> > data = getVipIconInfoSeqMap(ids, typeList);
		Map<Integer,T> res = new HashMap<Integer, T >();
		for(Entry<Integer,List<T> > ent : data.entrySet()) {
			if(!ent.getValue().isEmpty()){
				res.put(ent.getKey(), ent.getValue().get(0));	
			}
		}
		cost.endFinally();
		return res;
	}
	
	public Map<Integer, List<T>> getVipIconInfoSeqMap(List<Integer> ids, List<Integer> types) {
		TimeCost cost = TimeCost.begin("VipIconInfoReaderAdapter.getVipIconInfoSeqMap id size: " + ids.size());
	    VipIconDataReaderPrx prx = null;
	    Map<Integer, VipIconData[]> data;
	    
	    try {
			prx = getVipIconDataReaderPrx(0);
			data = prx.getVipIconDataSeqMap(MathUtil.list2array(ids), MathUtil.list2array(types));
			Map<Integer, List<T>> res = new HashMap<Integer, List<T> >();
			for(Entry<Integer, VipIconData[]> ent : data.entrySet()) {
				T o = _factory.create();
				List<T> dataList = new ArrayList<T>();
				for(VipIconData sData : ent.getValue()) {
					o.parse(sData);
					dataList.add(o);
				}
				res.put(ent.getKey(), dataList);
			}
			return res;
		} catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".getVipIconInfoSeqMap [" + prx + "] id=" 
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	}
	
	public T getOneTypeVipIconInfo(int id, int type) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(type);
		Map<Integer, List<T>> res = getVipIconInfoSeqMap(idList, typeList);
		if(!res.isEmpty()) {
			if(!res.get(id).isEmpty()){
				return res.get(id).get(0);	
			}
		}
		return null;
  }
	
	public List<T> getVipIconInfoSeq(int id, List<Integer> types) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		Map<Integer, List<T>> res = getVipIconInfoSeqMap(idList, types);
		if(!res.isEmpty()) {
			return res.get(id);
		}
		return null;
  }
	
	public static void main(String[] args) {
		VipIconInfoAdapter<DefaultVipIconInfo> fAdapter = new VipIconInfoAdapter<DefaultVipIconInfo>(new DefaultVipIconInfoFactory());
		
		
		int id = 386246921;
		List<Integer> types = new ArrayList<Integer>();
		types.add(0);

		VipMemberInfoAdapter<DefaultVipMemberInfo> adapter = new VipMemberInfoAdapter<DefaultVipMemberInfo>(new DefaultVipMemberInfoFactory());
		VipMemberInfo vmData = new VipMemberInfo();
		VipMemberInfo oldVmData = adapter.getVipMemberInfoSeq(id, types).get(0);
		oldVmData.setType(0);
		oldVmData.setIconId(oldVmData.getIconId());
		
		vmData.setType(0);
		vmData.setIconId((oldVmData.getIconId()+1) % 10);
		
		try {
			adapter.setVipMemberInfo(id, vmData);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		DefaultVipIconInfo viData = fAdapter.getOneTypeVipIconInfo(id, 0);
		if(viData.getIconId()!= vmData.getIconId()){
			System.out.println("Set vipmember icon failed!");
		}
		
		try {
			adapter.setVipMemberInfo(id, oldVmData);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Icon2UrlInfoAdapter<DefaultIcon2UrlInfo> iAdapter = new Icon2UrlInfoAdapter<DefaultIcon2UrlInfo>(new DefaultIcon2UrlInfoFactory());
		Icon2UrlInfo iData = new Icon2UrlInfo();
		Icon2UrlInfo oldIData = iAdapter.getIcon2UrlInfo(oldVmData.getIconId());
		oldIData.setHideUrl(oldIData.getHideUrl());
		iData.setHideUrl(String.valueOf(id));
		iAdapter.setIcon2UrlInfo(oldIData.getIconId(), iData);
		
		DefaultVipIconInfo viData1 = fAdapter.getOneTypeVipIconInfo(id, 0);
		if(!viData1.getHideUrl().equals( iData.getHideUrl())){
			System.out.println("Set icon url failed!");
		}
		
		iAdapter.setIcon2UrlInfo(oldIData.getIconId(), oldIData);
		
		System.exit(0);
		
	}

}
