package xce.distcache.preloader;

import java.util.Map;
import java.util.TreeMap;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;

import xce.distcache.DistCacheAdapter;

public class SetData {

	private static final int MAX_SIZE = 100;

	private static SetData _instance = new SetData();

	private static DistCacheAdapter _adapter;

	private BlockingDeque<Map<Long, byte[]>> _queue = new LinkedBlockingDeque<Map<Long, byte[]>>(
			MAX_SIZE);

	public static SetData getInstance() {
		return _instance;
	}

	public void setName(String cacheName) {
		_adapter = DistCacheAdapter.connect(cacheName);
		return;
	}

	public void push() {
		try {
			long start = System.currentTimeMillis();
			Map<Long, byte[]> data = _queue.takeFirst();
			long end1 = System.currentTimeMillis();
			_adapter.add(data);
			long end = System.currentTimeMillis();
			PreloaderLogger.debug("SetData.push: min: "
					+ ((TreeMap<Long, byte[]>) data).firstKey() + " max: "
					+ ((TreeMap<Long, byte[]>) data).lastKey() + " time1: "
					+ (end1 - start) + " time2: " + (end - end1));
		} catch (Exception e) {
			PreloaderLogger.warn("SetData.push, Exception " + e.toString());
		}
	}

	public void changeStatus() {
		while (true) {
			try {
				if (_queue.size() > 0) {
					Thread.sleep(10000);
				} else {
					break;
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		_adapter.setNodeStatusUpgrading2On();
	}

	public void set(Map<Long, byte[]> data) {
		try {
			if (null == data) {
				PreloaderLogger.debug("SetData.set, data is null!");
				return;
			}
			
			if (null == _queue) {
				PreloaderLogger.debug("SetData.set, queue is null!");
				return;
			}
			
			_queue.putLast(data);
			PreloaderLogger.debug("SetData.set: min: "
					+ ((TreeMap<Long, byte[]>) data).firstKey() + " max: "
					+ ((TreeMap<Long, byte[]>) data).lastKey());
		} catch (Exception e) {
			PreloaderLogger.warn("SetData.set, Exception " + e.toString());
		}
	}
}
