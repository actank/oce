package xce.distcache.server;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

import org.apache.zookeeper.ZooKeeper;

import tokyotyrant.RDB;
import xce.distcache.ZkConnectionFactory;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-18
 */
public class LoadProducerThread extends Thread {
	private RDB _db = null;
	private int _block;
	private int _index;
	private static final int MAX_TRAVERSAL = 1;// 每次传输的数据

	public LoadProducerThread(int block, int index) {
		/*
		 * 核心在这里，每个ProducerThread线程对应一个TT，通过TTClusterManager按顺序选择一个
		 * 同时每个block对应一个TTClusterManager
		 */
		this._block = block;
		this._index = index;
		this._db = LoadClusterHelper.getInstance().getRDB(block, index);
		System.out.println("LoadProducerThread, constructer, block: " + block + " index: " + index);
	}

	public void invoke() {
		try {
			_db.iterinit();
			synchronized (this) {
				notify();
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return;
	}

	public void run() {
		while (true) {
			synchronized (this) {
				try {
					System.out.println("LoadProducerThread " + _index + ": start and wait()");
					wait();
				} catch (Exception e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}

			System.out.println("LoadProducerThread " + _index + ": wakeup!");
			try {
				String key;
				Map<Long, byte[]> data = new HashMap<Long, byte[]>();
				while ((key = (String) _db.iternext()) != null) {
					Object value = _db.get(key);
					System.out.println("thread: " + _index + ": loop, key=" + key + " value="
							+ new String((byte[]) value, "UTF-8"));
					if (value != null && !key.equals("key")) {
						data.put(Long.valueOf(key), (byte[]) value);
						if (data.size() > MAX_TRAVERSAL) {
							Map<Long, byte[]> batch = new HashMap<Long, byte[]>();
							batch.putAll(data);
							LoadBlockingDeque.getInstance().push(batch);
							data.clear();
						}
					}
				}
				System.out.println("LoadProducerThread " + _index + ": Loop is end!");
				LoadBlockingDeque.getInstance().push(data);
				LoadClusterHelper.getInstance().setFinish();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
