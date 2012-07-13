package com.xiaonei.xce.usercache.impl;

import com.xiaonei.xce.log.Oce;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import java.util.Map.Entry;

import xce.distcache.usercache.UserCache.UserCacheData;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;
import com.xiaonei.xce.usercache.DefaultUserCache;
import com.xiaonei.xce.usercache.DefaultUserCacheFactory;
import com.xiaonei.xce.usercache.UserCache;
import com.xiaonei.xce.usercache.UserCacheFactory;
import com.xiaonei.xce.usercache.UserCacheInvoker;

import xce.distcache.DistCacheAdapter;
import com.xiaonei.xce.vipiconinfo.DefaultVipIconInfo;
import com.xiaonei.xce.vipiconinfo.DefaultVipIconInfoFactory;
import com.xiaonei.xce.vipiconinfo.VipIconInfo;
import com.xiaonei.xce.vipiconinfo.VipIconInfoAdapter;
import com.xiaonei.xce.vipmemberinfo.VipMemberInfo;

import java.util.concurrent.LinkedBlockingDeque;

public final class DCUserCacheAdapter<T extends UserCache> extends Thread implements UserCacheInvoker<T> {
	private UserCacheFactory<T> _factory;
	
	private DistCacheAdapter _adapter;
	
	private static final int MAX_GET_SIZE = 1000;

	private LinkedBlockingDeque<Long> _queue = new LinkedBlockingDeque<Long>(1000);

	private DCUserCacheReloaderAdapter _reloader;
	
	private VipIconInfoAdapter<DefaultVipIconInfo> _vipIconInfoAdapter;

	public DCUserCacheAdapter(UserCacheFactory<T> factory) {
		_factory = factory;
		start();
	}
	
	private VipIconInfoAdapter<DefaultVipIconInfo> getVipIconInfoAdapter()
	{
		if( _vipIconInfoAdapter == null )
		{
			synchronized( this )
			{
				if( _vipIconInfoAdapter == null )
				{
					_vipIconInfoAdapter  = new VipIconInfoAdapter<DefaultVipIconInfo>(new DefaultVipIconInfoFactory());
					return _vipIconInfoAdapter;
				}
			}
		}
		return _vipIconInfoAdapter;
	}

	private DistCacheAdapter getAdapter()
	{
		if( _adapter == null )
		{
			synchronized( this )
			{
				if( _adapter == null )
				{
					_adapter = DistCacheAdapter.connect("UserCache");
					return _adapter;
				}
			}
		}
		return _adapter;
	}
	
	private DCUserCacheReloaderAdapter getReloader()
	{
		if( _reloader == null )
		{
			synchronized( this )
			{
				if( _reloader == null )
				{
					_reloader = new DCUserCacheReloaderAdapter();
					return _reloader;
				}
			}
		}
		return _reloader;
	}
	
	public void run() {
		while(true) {
			try {
				List<Long> ids = new ArrayList<Long>();
				_queue.drainTo(ids, 100);
				int[] id = new int[ids.size()];
				for (int i=0;i<ids.size();i++) {
					id[i]=ids.get(i).intValue();
				}
				if(!ids.isEmpty()) {
					getReloader().reload(id);	
				}
				Thread.sleep(1000);
			} catch (Exception e) {
			}
		}
	}

	public Map<Integer, T> getUserCacheMap(List<Integer> userIds) {
		Map<Integer, T> allResults = new HashMap<Integer, T>();

		//为空直接返回
		if (userIds.isEmpty()) {
			XceStat.getLogger().warn(this.getClass().getName() + ".getUserCacheMap " + " idSize=0");
			return allResults;
		}

		//过滤重复id
		Set<Integer> idsSet = new HashSet<Integer>(userIds);    
		List<Integer> finalIdsList = new ArrayList<Integer>();      
		finalIdsList.addAll(idsSet);
		if(userIds.size() != finalIdsList.size()) {
			XceStat.getLogger().warn(this.getClass().getName() + ".getUserCacheMap " + " there are duplicate ids inIdsSize = " + userIds.size() + "outIdsSize = " + finalIdsList.size());
		}

		//按cluster进行散id
		int cluster = getAdapter().getCluster();
		Map<Integer, List<Integer> > splitIdsLists = new HashMap<Integer, List<Integer> >();
		for (int i = 0;i < cluster; ++i) {
			splitIdsLists.put(i, new ArrayList<Integer>());
		}
		for (Integer i:finalIdsList) {
			splitIdsLists.get(i % cluster).add(i);
		}

		//对每个mod取UserCache
		for (Entry<Integer, List<Integer> > entry : splitIdsLists.entrySet()) {
			if (entry.getValue().size() > MAX_GET_SIZE) {
				XceStat.getLogger().error(this.getClass().getName() + ".getUserCacheMap " + " there are too many ids in one get idSize = " + entry.getValue().size());
			}
			int times = (int)Math.ceil((double)entry.getValue().size()/(double)MAX_GET_SIZE);
			int idsSize = entry.getValue().size();
			for(int i = 0; i < times; ++i) {
				int startIndex = i*MAX_GET_SIZE;
				int endIndex = startIndex + MAX_GET_SIZE;
				if(endIndex > idsSize) {
					endIndex = idsSize;
				}
				List<Integer> tempList = entry.getValue().subList(startIndex, endIndex);
				try {
					Map<Long, byte[]> subTargets = null;
					List<Long> idsList = new ArrayList<Long>();
					
					for(Integer ii:tempList){
						idsList.add((long)ii);
					}

					subTargets = getAdapter().get(idsList);
					for (Entry<Long, byte[]> obj : subTargets.entrySet()) {
						T o = _factory.createUserCache();
						o.setBase(UserCacheData.parseFrom(obj.getValue()));
						allResults.put(obj.getKey().intValue(), o);
					}
				} catch(ConnectTimeoutException e){
					Oce.getLogger().error(this.getClass().getName() + ".getUserCacheMap " + " idSize=" + tempList.size() + " Ice.ConnectTimeoutException");
					e.printStackTrace();
				} catch (TimeoutException e) {
					Oce.getLogger().error(this.getClass().getName() + ".getUserCacheMap " + " idSize=" + tempList.size() + " Ice.TimeoutException");
					e.printStackTrace();
				} catch (java.lang.Throwable e) {
					Oce.getLogger().error(this.getClass().getName() + ".getUserCacheMap " + " idSize=" + tempList.size() , e);
					e.printStackTrace();
				}
			}
		}
		for (Integer lid:idsSet) {
			if(!allResults.containsKey(lid)) {
				_queue.offer(lid.longValue());
			}
		}
		return allResults;
	}

	public List<T> getUserCacheList(List<Integer> userIds) {
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"GetUserCacheList");
		Map<Integer, T> allResults = getUserCacheMap(userIds);
		List<T> result = new ArrayList<T>();
		for (int userId : userIds) {
			T obj = allResults.get(userId);
			if (obj == null)
				continue;
			result.add(obj);
		}
		cost.endFinally();
		return result;
	}

	public void load(int id) {
		int userIds[] = new int[1];
		userIds[0] = id;
		//getUserCacheManager(id).load(userIds);
	}


	public static void main(String[] args) {
		try {
			DCUserCacheAdapter<DefaultUserCache> adapter = new DCUserCacheAdapter<DefaultUserCache>(
					new DefaultUserCacheFactory());
			List<Integer> ids=new ArrayList<Integer>();
			ids.add(68126);
			Map<Integer, DefaultUserCache> map=adapter.getUserCacheMap(ids);
			for(Entry<Integer,DefaultUserCache> entry:map.entrySet()){
				System.out.println(entry.getKey()+" "+entry.getValue().getName());
				System.out.println("gender: " + entry.getValue().getGender());
			}
		} catch (Exception e) {
			Oce.getLogger().error(e);
		}
		System.exit(0);
	}

	@Override
	public Map<Integer, T> getUserCacheMap(List<Integer> userIds,
			List<Integer> types) {
		// TODO Auto-generated method stub
		Map<Integer, T> res = getUserCacheMap(userIds);
		List<Integer> vipIds = new ArrayList<Integer>();
		for(Entry<Integer, T> ent : res.entrySet()) {
			if(ent.getValue().isVipMember()) {
				vipIds.add(ent.getKey());
			}
		}

    if(vipIds.isEmpty()) {
      return res;
    }
		
    Map<Integer, List<DefaultVipIconInfo>> vipIconRes = getVipIconInfoAdapter()
				.getVipIconInfoSeqMap(vipIds, types);
		for (Entry<Integer, List<DefaultVipIconInfo>> ent : vipIconRes
				.entrySet()) {
			List<VipIconInfo> vList = new ArrayList<VipIconInfo>();
			vList.addAll(ent.getValue());
			res.get(ent.getKey()).setVipIconInfoList(vList);
		}

		return res;
	}

}
