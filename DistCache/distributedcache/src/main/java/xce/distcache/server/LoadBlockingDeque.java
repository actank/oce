package xce.distcache.server;

import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;
/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-18
 */
class LoadBlockingDeque {
	private static final int MAX_SIZE = 100;
	private BlockingDeque<Map<Long, byte[]>> _queue = new LinkedBlockingDeque<Map<Long, byte[]>>(
			MAX_SIZE);

	private static final LoadBlockingDeque _instance = new LoadBlockingDeque();

	public static LoadBlockingDeque getInstance() {
		return _instance;
	}
	
	public void push(Map<Long, byte[]> data) {
		try {
			_queue.putLast(data);
			System.out.println("LoadBlockingDeque, push, _queue.size()=" + _queue.size());
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public Map<Long, byte[]> pop() {
		Map<Long, byte[]> result = null;
		try {
			result = _queue.takeLast();
			System.out.println("LoadBlockingDeque, pop, result.size()=" + result.size() + " _queue.size=" + _queue.size());
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return result;
	}
}