package com.xiaonei.xce.vipfullinfo;

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
import com.xiaonei.xce.vipmemberinfo.VipMemberInfoFactory;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.tools.MathUtil;
import xce.vipinfo.VipFullDataN;
import xce.vipinfo.VipFullDataReaderPrx;
import xce.vipinfo.VipFullDataReaderPrxHelper;

public final class VipFullInfoAdapter<T extends VipFullInfo<P>, P extends VipMemberInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoReader";

	private static final String service_name = "VF";
	
	private static int _interval = 120;

	protected VipFullInfoFactory<T> _fullFactory;
	protected VipMemberInfoFactory<P> _memberFactory;

	public VipFullInfoAdapter(VipFullInfoFactory<T> fullFactory, VipMemberInfoFactory<P> memberFactory) {
		super(ctr_endpoints, _interval);
		
		_fullFactory = fullFactory;
		_memberFactory = memberFactory;
	}

	protected VipFullDataReaderPrx getVipFullDataReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			VipFullDataReaderPrx prx = VipFullDataReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public Map<Integer, List<T> > getVipFullInfoSeqMap(List<Integer> ids, List<Integer> types) {
		TimeCost cost = TimeCost.begin("VipFullInfoReaderAdapter.getVipFullInfoSeqMap id size: " + ids.size());
	    VipFullDataReaderPrx prx = null;
	    Map<Integer, VipFullDataN[]> data;
	    
	    try {
			prx = getVipFullDataReaderPrx(0);
			data = prx.getVipFullDataNSeqMap(MathUtil.list2array(ids), MathUtil.list2array(types));
			Map<Integer, List<T> > res = new HashMap<Integer, List<T> >();
			for(Entry<Integer, VipFullDataN[]> ent : data.entrySet()) {
				List<T> dataList = new ArrayList<T>();
				for(VipFullDataN sData : ent.getValue()) {
					T o = _fullFactory.create();
					P p = _memberFactory.create();
					p.parse(sData.memberData);
					o.setMemberInfo(p);
					
					o.setId(sData.id);
					o.setIconUrl(sData.iconUrl);
					o.setHatUrl(sData.hatUrl);
					o.setHatStatus(sData.hatStatus);
					dataList.add(o);
				}
				res.put(ent.getKey(), dataList);
			}
			return res;
		} catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".getVipFullInfoSeqMap [" + prx + "] id=" 
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	}
	
	public Map<Integer, T> getOneTypeVipFullInfoMap(List<Integer> ids, int type) {
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(type);
		Map<Integer, List<T> > res = getVipFullInfoSeqMap(ids, typeList);
		if(!res.isEmpty()) {
			Map<Integer, T> data = new HashMap<Integer, T>();
			for(Entry<Integer, List<T> > ent : res.entrySet()) {
				data.put(ent.getKey(), ent.getValue().get(0));
			}
			return data;
		}
		return null;
	}
	
	public T getOneTypeVipFullInfo(int id, int type) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(type);
		Map<Integer, List<T> > res = getVipFullInfoSeqMap(idList, typeList);
		if(!res.isEmpty()) {
			return res.get(id).get(0);
		}
		return null;
  }
	
	public List<T> getVipFullInfoSeq(int id, List<Integer> types) {
		List<Integer> idList = new ArrayList<Integer>();
		idList.add(id);
		Map<Integer, List<T> > res = getVipFullInfoSeqMap(idList, types);
		if(!res.isEmpty()) {
			return res.get(id);
		}
		return null;
	}
	
	
	public static void main(String[] args) {
		VipFullInfoAdapter<DefaultVipFullInfo, DefaultVipMemberInfo> fAdapter = new VipFullInfoAdapter<DefaultVipFullInfo, DefaultVipMemberInfo>(new DefaultVipFullInfoFactory(), new DefaultVipMemberInfoFactory());
		
		
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
		
		DefaultVipFullInfo viData = fAdapter.getOneTypeVipFullInfo(id, 0);
		if(viData.getMemberInfo().getIconId()!= vmData.getIconId()){
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
		
		DefaultVipFullInfo viData1 = fAdapter.getOneTypeVipFullInfo(id, 0);
		if(!viData1.getIconUrl().equals( iData.getHideUrl())){
			System.out.println("Set icon url failed!");
		}
		
		iAdapter.setIcon2UrlInfo(oldIData.getIconId(), oldIData);
		
		System.exit(0);
	}


	
}
