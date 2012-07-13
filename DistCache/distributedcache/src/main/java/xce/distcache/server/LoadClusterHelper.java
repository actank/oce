package xce.distcache.server;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Map.Entry;
import tokyotyrant.RDB;
import xce.distcache.ClusterManager;
import xce.distcache.Distribution;
import xce.distcache.node.LoadNode;
import xce.distcache.node.Node;
import xce.distcache.node.ReadNode;
import xce.distcache.node.Node.Item;
import xce.distcache.node.impl.TTReadNode;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-18
 */
public class LoadClusterHelper {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(LoadClusterHelper.class);

	private final ClusterManager _clusterManager = new ClusterManager();
	private Distribution _dist;
	private int _blockSize;
	private int _totalTTSize;
	private int _currentFinishSize;
	private Random _random;
	private String _name;
	private Boolean _isReady = true;

	private static final LoadClusterHelper _instance = new LoadClusterHelper();

	public static LoadClusterHelper getInstance() {
		return _instance;
	}

	public void initialize(String name) {
		System.out.println("LoadClusterHelper, name :" + name);
		try {
			_blockSize = _clusterManager.initialize(name, null);
			_dist = Distribution.instance(_blockSize);
			_random = new Random(System.currentTimeMillis());
			_name = name;
			_currentFinishSize = 0;
			
			_totalTTSize = 0;
			for (int block = 0; block < _blockSize; block++) {
				_totalTTSize += this.getTTClusterSize(block);
			}

			System.out.println("cluster.getBackupSize: "
					+ _clusterManager.getBackupNodes(0).size());
			System.out.println("cluster.getReadSize: "
					+ _clusterManager.getReadNodes(0).size());
			System.out.println("cluster.getLoadSize: "
					+ _clusterManager.getLoadNodes().size());
			System.out.println("cluster.getWriteSize: "
					+ _clusterManager.getWriteNodes().size());
			System.out.println("name : " + name + " TotalTTSize: "
					+ _totalTTSize);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public RDB getRDB(int block) {
		TTReadNode backend = (TTReadNode) _clusterManager.getBackupNodes(block)
				.get(0);// 0默认为一个，如果是多个要策略去取，当前线程取第几个。注意不一定和memcached对应
		return backend.getRDB();
	}
	
	public RDB getRDB(int block, int index) {
		TTReadNode backend = (TTReadNode) _clusterManager.getBackupNodes(block)
				.get(0);// 0默认为一个，如果是多个要策略去取，当前线程取第几个。注意不一定和memcached对应
		return backend.getRDB(index);
	}

	public int getTTClusterSize(int block) {
		TTReadNode backend = (TTReadNode) _clusterManager.getBackupNodes(block)
				.get(0);
		return backend.getTTClusterSize();
	}

	public void add(Map<Long, byte[]> datas) {
		System.out.println("LoadClusterHelper,add, size()=" + datas.size());
		for (Entry<Long, byte[]> entry : datas.entrySet()) {
			long id = entry.getKey();
			int block = _dist.get(id);
			List<ReadNode> updatingList = _clusterManager
					.getUpgradingNodes(block);
			try {
				for (ReadNode xx : updatingList) {
					Item item = new Item(block, id);
					byte[] value = (byte[]) entry.getValue();
					xx.add(item, value);
				}
			} catch (Throwable e) {
				_logger.error("WriteServerI::add error " + id + block, e);
			}
		}
	}

	/*
	 * getBackup和setBackup接口可以直接读写TT
	 */
	public byte[] getBackup(long id) {
		_logger.debug("getBackup, id=" + id);
		int block = _dist.get(id);
		List<ReadNode> backends = _clusterManager.getBackupNodes(block);
		if (0 != backends.size()) {
			return backends.get(0).get(new Node.Item(block, id));
		} else {
			_logger.warn("backup node is empty!");
			return null;
		}
	}

	public void setBackup(long id, byte[] data) {
		_logger.debug("setBackup, id=" + id);
		int block = _dist.get(id);
		List<ReadNode> backends = _clusterManager.getBackupNodes(block);
		if (0 != backends.size()) {
			backends.get(0).add(new Node.Item(block, id), data);
		} else {
			_logger.warn("backup node is empty!");
		}
	}

	public int getBlockSize() {
		return _blockSize;
	}

	public boolean isNeedLoad(int block) {
		int size = _clusterManager.getUpgradingNodes(block).size();
		System.out.println("LoadClusterHelper.isNeedLoad! " + " upgradingNodeSize: " + size);
		return  size > 0 && this._isReady ? true : false;
	}

	/**
	 * 这里如果是多线程，需要判断如何判断结束，可以预先知道多少份同时加载，如果都完成，在这里结束
	 */
	public synchronized void setFinish() {
		System.out.println("LoadClusterHelper.setFinish, curentFinishSize: " +
				_currentFinishSize + " totalTTSize: " + _totalTTSize);
		_currentFinishSize ++;
		if (_currentFinishSize == _totalTTSize) {
			System.out.println("LoadClusterHelper.setFinish all is end!");
			_currentFinishSize = 0;
			_clusterManager.setNodeStatusUpgrading2On();
		}
	}
}