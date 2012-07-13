package com.xiaonei.xce.searchcache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import Ice.ObjectPrx;
import xce.searchcache.SearchCacheManagerPrx;
import xce.searchcache.SearchCacheManagerPrxHelper;
import xce.searchcache.SearchCacheResult;
import xce.util.channel.Channel;
import xce.util.service.ReplicatedServiceAdapter;
import xce.util.tools.MathUtil;

public class SearchCacheAdapter extends ReplicatedServiceAdapter {
	private static final String endpoints = "@SearchCacheN";
	
	private Vector<SearchCacheManagerPrx> managers = new Vector<SearchCacheManagerPrx>();
	private Vector<SearchCacheManagerPrx> managersOneway = new Vector<SearchCacheManagerPrx>();	

	public SearchCacheAdapter() {
		super(endpoints, 4, Channel.newSearchCacheChannel(endpoints),"XiaoNeiSearchCache/Query");
	}
	
	protected SearchCacheManagerPrx getSearchCacheManager(int userId){
		return SearchCacheManagerPrxHelper.uncheckedCast(locateProxy(managers,"M",userId,Channel.Invoke.Twoway,SearchCacheManagerPrxHelper.class,300));
    //ObjectPrx proxy = locateProxy(managers,"M",userId,Channel.Invoke.Twoway,SearchCacheManagerPrxHelper.class,300); 
    //System.out.println("proxy=" + proxy);
		//return SearchCacheManagerPrxHelper.uncheckedCast(proxy);
	}
	
	protected SearchCacheManagerPrx getSearchCacheManagerOneway(int userId){
		return SearchCacheManagerPrxHelper.uncheckedCast(locateProxy(managersOneway,"M",userId,Channel.Invoke.Oneway,SearchCacheManagerPrxHelper.class,300));
	}
	
	public SearchCacheResult getSearchCache(int userId){
		return getSearchCacheManager(userId).getSearchCache(userId);
	}
	
	public Map<Integer,SearchCacheResult> getSearchCacheMap(List<Integer> userIds){
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}
		Map<Integer, SearchCacheResult> allResults = new HashMap<Integer, SearchCacheResult>();
		if (userIds.isEmpty()){
			return allResults;
		}
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()){
				continue;
			}
			Map<Integer, SearchCacheResult> subTargets=getSearchCacheManager(i).getSearchCacheList(MathUtil.list2array(subUserIds));
			for(Entry<Integer,SearchCacheResult> entry:subTargets.entrySet()){
				allResults.put(entry.getKey(), entry.getValue());
			}
		}
		return allResults;
	}

	public Map<Integer, byte[]> getSearchCacheMap2(List<Integer> userIds){
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
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
			if (subUserIds.isEmpty()){
				continue;
			}
			Map<Integer,  byte[]> subTargets =
				getSearchCacheManager(i).getSearchCacheList2(MathUtil.list2array(subUserIds));
			allResults.putAll(subTargets);
		}
		return allResults;
	}

	@Override
	protected boolean verifyProxy(ObjectPrx proxy) {
		return SearchCacheManagerPrxHelper.uncheckedCast(proxy).isValid();
	}

  public static void main(String []args) {
    try {

    SearchCacheAdapter adapter = new SearchCacheAdapter();
    List<Integer> userIds = new ArrayList<Integer>();
    for (int i=0; i<args.length; ++i) {
      userIds.add(Integer.parseInt(args[i]));
    }
    System.out.println("query.size=" + userIds.size() + ", list=" + userIds);
    Map<Integer, byte[]> cacheResults = adapter.getSearchCacheMap2(userIds);
    System.out.println("result.size=" + cacheResults.size() + ", list=" + cacheResults.keySet());
    System.out.println("");
    System.out.println("****************************************");
    } catch (Exception e) {

    }
    
    System.exit(0);
  }

}
