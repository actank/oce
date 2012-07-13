package xce.distcache;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Map.Entry;

import tokyotyrant.RDB;
import xce.distcache.node.Node;
import xce.distcache.node.ReadNode;
import xce.distcache.node.WriteNode;
import xce.distcache.node.Node.Item;
import xce.distcache.node.impl.TTReadNode;

/**
 * 全局入口
 * */
public class DistCacheAdapter {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(DistCacheAdapter.class);

    /**
     * @return null if anything wrong.
     * */
    public static DistCacheAdapter connect(String name, String zkAddress) {
	DistCacheAdapter adapter = new DistCacheAdapter();
	if (adapter.initialize(name, zkAddress) > 0) {
	    return adapter;
	} else {
	    return null;
	}
    }

    public static DistCacheAdapter connect(String name) {
	DistCacheAdapter adapter = new DistCacheAdapter();
	if (adapter.initialize(name, null) > 0) {
	    return adapter;
	} else {
	    return null;
	}
    }

    private ClusterManager _clusterManager;
    private final PreferManager _preferManager = new PreferManager();
    private Distribution _dist;
    private int _blockSize;
    private Random _random;
    private String _name;

    private DistCacheAdapter() {
    }

    private int initialize(String name, String zkAddress) {
	try {
		_clusterManager = ClusterManager.getInstance(name, zkAddress);
		_blockSize = _clusterManager.initialize(name, zkAddress);
	} catch (IOException e) {
		// TODO Auto-generated catch block
		 _logger.error("DistCacheAdapter::get readnodes with upgrading size: 0");
		return -1;
	}
	_dist = Distribution.instance(_blockSize);
	_random = new Random(System.currentTimeMillis());
	_name = name;
	return _blockSize;
    }

    public byte[] get(long id) {
	int block = _dist.get(id);
	List<ReadNode> backends = _clusterManager.getReadNodes(block);
	if (backends.isEmpty()) {
	    backends = _clusterManager.getReadNodesWithUpgrading(block);
	}
	if (backends.isEmpty()) {
	    _logger
		    .error("DistCacheAdapter::get readnodes with upgrading size: 0");
	    return new byte[0];
	}
	ReadNode b = _preferManager.choose(backends);
	return b.get(new Node.Item(block, id));
    }

    public int getCluster() {
	return _clusterManager.getNumBlocks();
    }

    public Map<Long, byte[]> get(List<Long> ids) {
	Map<Integer, List<Long>> seperated = new HashMap<Integer, List<Long>>();
	for (Long id : ids) {
	    int block = _dist.get(id);
	    List<Long> seped_id_list = seperated.get(block);
	    if (seped_id_list == null) {
		seped_id_list = new ArrayList<Long>();
		seperated.put(block, seped_id_list);
	    }
	    seped_id_list.add(id);
	}

	Map<ReadNode, List<Item>> categoried = new HashMap<ReadNode, List<Item>>();
	for (Entry<Integer, List<Long>> entry : seperated.entrySet()) {
	    int block = entry.getKey();
	    List<ReadNode> backends = _clusterManager.getReadNodes(block);
	    if (backends.isEmpty()) {
		backends = _clusterManager.getReadNodesWithUpgrading(block);
	    }
	    if (backends.isEmpty()) {
		_logger
			.error("DistCacheAdapter::get readnodes with upgrading size: 0");
		continue;
	    }
	    ReadNode b = _preferManager.choose(backends, categoried);
	    List<Item> items = categoried.get(b);
	    if (items == null) {
		items = new ArrayList<Item>();
		categoried.put(b, items);
	    }
	    for (Long id : entry.getValue()) {
		items.add(new Item(block, id));
	    }

	}

	Map<Long, byte[]> result = new HashMap<Long, byte[]>();
	for (Entry<ReadNode, List<Item>> entry : categoried.entrySet()) {
	    Map<Item, byte[]> one_batch = entry.getKey().get(entry.getValue());
	    for (Entry<Item, byte[]> one_entry : one_batch.entrySet()) {
		result.put(one_entry.getKey().id, one_entry.getValue());
	    }
	}
	return result;
    }

    public void set(long id, byte[] data) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("DistCacheAdapter::set id=" + id);
	}
	int block = _dist.get(id);
	List<WriteNode> x = _clusterManager.getWriteNodes();
	_logger.debug("x.size=" + x.size());
	if (!x.isEmpty()) {
	    x.get(_random.nextInt(x.size())).set(_name, new Item(block, id),
		    data);
	} else {
	    _logger.error("DistCacheAdapter::set writenodes size: 0");
	}
    }

    public void set(Map<Long, byte[]> bulkDatas) {
	try {
	    if (_logger.isDebugEnabled()) {
		_logger.debug("DistCacheAdapter::set bulkDatas.size"
			+ bulkDatas.size());
	    }
	    List<WriteNode> x = _clusterManager.getWriteNodes();
	    _logger.debug("x.size=" + x.size());
	    if (!x.isEmpty()) {
		x.get(_random.nextInt(x.size())).set(_name, bulkDatas);
	    } else {
		_logger.error("DistCacheAdapter::set writenodes size: 0");
	    }
	} catch (Exception e) {
	    _logger.error(this + " " + e);
	}
    }

    public void add(Map<Long, byte[]> bulkDatas) {
	try {
	    if (_logger.isDebugEnabled()) {
		_logger.debug("DistCacheAdapter::add bulkDatas.size"
			+ bulkDatas.size());
	    }
	    List<WriteNode> x = _clusterManager.getWriteNodes();
	    _logger.debug("x.size=" + x.size());
	    if (!x.isEmpty()) {
		x.get(_random.nextInt(x.size())).add(_name, bulkDatas);
	    } else {
		_logger.error("DistCacheAdapter::add writenodes size: 0");
	    }
	} catch (Exception e) {
	    _logger.error(this + " " + e);
	}
    }

    public ClusterContent getClusterContent() {
	return _clusterManager.getClusterContent();
    }

    public void setNodeStatusUpgrading2On() {
	_clusterManager.setNodeStatusUpgrading2On();
    }

    public static void main(String[] args) throws InterruptedException {
	DistCacheAdapter adapter = DistCacheAdapter.connect(null, "");
	List<ReadNode> nodes = adapter.getClusterManager().getUpgradingNodes(0);
	System.out.println(nodes.size());
    }

    public ClusterManager getClusterManager() {
	return _clusterManager;
    }
    
    public byte[] getBackup(long id) {
		int block = _dist.get(id);
		List<ReadNode> backends = _clusterManager.getBackupNodes(block);
		if (0 != backends.size()) {
			return backends.get(0).get(new Node.Item(block,id));
		} else {
			_logger.warn("DistCacheAdapter::getBackup backupnodes size: 0");
			return null;
		}
	}
	
	public void setBackup(long id, byte[] data) {
		int block = _dist.get(id);
		List<ReadNode> backends = _clusterManager.getBackupNodes(block);
		if (0 != backends.size()) {
			backends.get(0).add(new Node.Item(block, id), data);
		} else {
			_logger.warn("DistCacheAdapter::getBackup backupnodes size: 0");
		}
	}

}
