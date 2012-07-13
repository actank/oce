package com.xiaonei.xce.buddyadapter.buddyapplycache;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingDeque;

import xce.distcache.DistCacheAdapter;
import xce.distcache.buddyapplycache.BuddyApplyCacheData.BuddyApplyData;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.log.Oce;

public final class DCBuddyApplyCacheAdapter<T extends BuddyApplyCacheDataN, W extends BuddyApplyCacheWithTotalN<T>>
		extends Thread {
	private BuddyApplyCacheFactoryN<T, W> _factory;

	private DistCacheAdapter _adapter;

	private LinkedBlockingDeque<Long> _queue = new LinkedBlockingDeque<Long>(1000);

	private DCBuddyApplyCacheReloaderAdapter _reloader;

	public DCBuddyApplyCacheAdapter(BuddyApplyCacheFactoryN<T, W> factory) {
		_factory = factory;
		start();
	}

	private DistCacheAdapter getAdapter() {
		if (_adapter == null) {
			synchronized (this) {
				if (_adapter == null) {
					_adapter = DistCacheAdapter.connect("BuddyApplyCache");
					return _adapter;
				}
			}
		}
		return _adapter;
	}

	private DCBuddyApplyCacheReloaderAdapter getReloader() {
		if (_reloader == null) {
			synchronized (this) {
				if (_reloader == null) {
					_reloader = new DCBuddyApplyCacheReloaderAdapter();
					return _reloader;
				}
			}
		}
		return _reloader;
	}

	public void run() {
		while (true) {
			try {
				List<Long> ids = new ArrayList<Long>();
				_queue.drainTo(ids, 100);
				int[] id = new int[ids.size()];
				for (int i = 0; i < ids.size(); i++) {
					id[i] = ids.get(i).intValue();
				}
				getReloader().reload(id);
				Thread.sleep(1000);
			} catch (Exception e) {
			}
		}
	}

	public List<T> getBuddyApplyCache(int userId, int begin, int limit) {
		List<T> result = new ArrayList<T>();
		byte[] res = null;
		try {
			res = getAdapter().get(userId);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyApplyCache " + userId
							+ " Ice.ConnectTimeoutException");
			e.printStackTrace();
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyApplyCache " + userId
							+ " Ice.TimeoutException");
			e.printStackTrace();
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyApplyCache " + userId, e);
			e.printStackTrace();
		}

		if (res.equals(null)) {
			_queue.offer((long) userId);
			return result;
		}
		try {
			BuddyApplyData data = BuddyApplyData.parseFrom(res);
			for (int i = 0; i < data.getItems().getItemCount(); ++i) {
				T o = _factory.createBuddyApplyCacheDataN();
				o.setBase(data.getItems().getItem(i));
				result.add(o);
			}

		} catch (InvalidProtocolBufferException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		if(begin > result.size()){
			return result;
		}

		int end = result.size()> begin + limit ? begin+limit:result.size(); 
		return result.subList(begin, end);
	}

	public W getBuddyApplyCacheWithTotal(int userId, int begin, int limit) {
		W result = _factory.createBuddyApplyCacheWithTotalN();
		List<T> res = getBuddyApplyCache(userId, begin, limit);
		result.setBase(res);
		return result;
	}

	public static void main(String[] args) {
		DCBuddyApplyCacheAdapter<DefaultBuddyApplyCacheData, DefaultBuddyApplyCacheWithTotal> adapter = new DCBuddyApplyCacheAdapter<DefaultBuddyApplyCacheData, DefaultBuddyApplyCacheWithTotal>(
				new DefaultBuddyApplyCacheFactory());
		List<DefaultBuddyApplyCacheData> res = adapter.getBuddyApplyCache(356365613, 0, 100);
		System.out.println(res.size());
		DefaultBuddyApplyCacheWithTotal res2 = adapter.getBuddyApplyCacheWithTotal(356365613, 0, 100);
		System.out.println(res2.getTotalCount());
		
		System.exit(0);
	}

}
