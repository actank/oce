package com.xiaonei.xce.userapicache;

import com.xiaonei.xce.log.Oce;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import java.util.Map.Entry;

import xce.distcache.userapicache.UserApiCache.UserApiCacheData;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.XceStat;

import xce.distcache.DistCacheAdapter;

import java.util.concurrent.LinkedBlockingDeque;

public final class DCUserApiCacheAdapter extends Thread implements UserApiCacheInvoker {
	
	private DistCacheAdapter _adapter;
	
	private static final int MAX_GET_SIZE = 1000;

	private LinkedBlockingDeque<Long> _queue = new LinkedBlockingDeque<Long>(1000);

	private DCUserApiCacheReloaderAdapter _reloader;

	public DCUserApiCacheAdapter() {
		start();
	}

	private DistCacheAdapter getAdapter()
	{
		if( _adapter == null )
		{
			synchronized( this )
			{
				if( _adapter == null )
				{
					_adapter = DistCacheAdapter.connect("UserApiCache");
					return _adapter;
				}
			}
		}
		return _adapter;
	}
	
	private DCUserApiCacheReloaderAdapter getReloader()
	{
		if( _reloader == null )
		{
			synchronized( this )
			{
				if( _reloader == null )
				{
					_reloader = new DCUserApiCacheReloaderAdapter();
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
				XceStat.getLogger().warn(this.getClass().getName() + ".reload " + e);
			}
		}
	}

	@Override
	public Map<Integer, UserApiCache> getUserApiCacheList(List<Integer> userIds) {
		// TODO Auto-generated method stub
		Map<Integer, UserApiCache> allResults = new HashMap<Integer, UserApiCache>();

		//为空直接返回
		if (userIds.isEmpty()) {
			XceStat.getLogger().warn(this.getClass().getName() + ".getUserApiCacheList " + " idSize=0");
			return allResults;
		}

		//过滤重复id
		Set<Integer> idsSet = new HashSet<Integer>(userIds);    
		List<Integer> finalIdsList = new ArrayList<Integer>();      
		finalIdsList.addAll(idsSet);
		if(userIds.size() != finalIdsList.size()) {
			XceStat.getLogger().warn(this.getClass().getName() + ".getUserApiCacheList " + " there are duplicate ids inIdsSize = " + userIds.size() + "outIdsSize = " + finalIdsList.size());
		}
		
		int times = (int)Math.ceil((double)finalIdsList.size()/(double)MAX_GET_SIZE);
		int idsSize = finalIdsList.size();
		for(int i = 0; i < times; ++i) {
			int startIndex = i*MAX_GET_SIZE;
			int endIndex = startIndex + MAX_GET_SIZE;
			if(endIndex > idsSize) {
				endIndex = idsSize;
			}
			List<Long> idsList = new ArrayList<Long>();
			for(Integer j : finalIdsList.subList(startIndex, endIndex) ) {
				idsList.add((long)j);
			}
			try {
				Map<Long, byte[]> tempRes = getAdapter().get(idsList);;

				for (Entry<Long, byte[]> obj : tempRes.entrySet()) {
					UserApiCache o = new UserApiCache();
					o.setBase(UserApiCacheData.parseFrom(obj.getValue()));
					allResults.put(obj.getKey().intValue(), o);
				}
			} catch(ConnectTimeoutException e){
				Oce.getLogger().error(this.getClass().getName() + ".getUserApiCacheList " + " idSize=" + idsList.size() + " Ice.ConnectTimeoutException");
				e.printStackTrace();
			} catch (TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getUserApiCacheList " + " idSize=" + idsList.size() + " Ice.TimeoutException");
				e.printStackTrace();
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".getUserApiCacheList " + " idSize=" + idsList.size() , e);
				e.printStackTrace();
			}
		}
		
		if(finalIdsList.size() != allResults.size()) {
			for (Integer lid:idsSet) {
				if(!allResults.containsKey(lid)) {
					_queue.offer(lid.longValue());
				}
			}			
		}
		return allResults;
	}

}
