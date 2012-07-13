package com.xiaonei.xce.searchcache;

import com.xiaonei.xce.log.Oce;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import java.util.Map.Entry;

import oce.searchcache.DistSearchCache;
import xce.distcache.usercache.UserCache.UserCacheData;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;
import com.xiaonei.xce.searchcache.SearchCacheInvoker;

import xce.distcache.DistCacheAdapter;
import xce.util.tools.MathUtil;

public final class DCSearchCacheAdapter implements SearchCacheInvoker {
	
	private DistCacheAdapter _adapter;

	public DCSearchCacheAdapter() {

	}

	private DistCacheAdapter getAdapter()
	{
		if( _adapter == null )
		{
			synchronized( this )
			{
				if( _adapter == null )
				{
          _adapter = DistCacheAdapter.connect("DistSearchCache");
					return _adapter;
				}
			}
		}
		return _adapter;
	}
	

	public Map<Integer, byte[]> getSearchCacheMap2(List<Integer> userIds) {
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
	  if(getAdapter() == null)  {
      return null;
    }  
    int cluster = getAdapter().getCluster();
    for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}
		Map<Integer, byte[]> allResults = new HashMap<Integer,  byte[]>();
		if (userIds.isEmpty()){
			return allResults;
		}
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			List<Long> idsList = new ArrayList<Long>();
			if (subUserIds.isEmpty()){
				continue;
			}
			for(Integer ii:subUserIds){
				idsList.add((long)ii);
			}
			Map<Long, byte[]> subTargets = null;
      subTargets = getAdapter().get(idsList);
			for (Entry<Long, byte[]> obj : subTargets.entrySet()) {
			  allResults.put(obj.getKey().intValue(), obj.getValue());
			}
		}
		return allResults;
	}


	public static void main(String[] args) {
		try {
			DCSearchCacheAdapter adapter = new DCSearchCacheAdapter();
			List<Integer> ids=new ArrayList<Integer>();
			ids.add(68126);
			Map<Integer, byte[]> map=adapter.getSearchCacheMap2(ids);
			for(Entry<Integer,byte[]> entry:map.entrySet()){
				//System.out.println(entry.getKey()+" "+entry.getValue());
				//System.out.println("gender: " + entry.getValue());
			}
		} catch (Exception e) {
			Oce.getLogger().error(e);
		}
		System.exit(0);
	}

}

