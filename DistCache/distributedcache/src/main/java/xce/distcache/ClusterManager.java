package xce.distcache;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.KeeperException.NoNodeException;
import org.apache.zookeeper.Watcher.Event.KeeperState;


import xce.distcache.node.LoadNode;
import xce.distcache.node.MetaNode;
import xce.distcache.node.Node;
import xce.distcache.node.NodeFactory;
import xce.distcache.node.ReadNode;
import xce.distcache.node.WriteNode;
import xce.distcache.node.Node.Item;

/**
 * <p>
 * 管理block->node关系。 只返回正确的代理。
 * </p>
 * <p>
 * 根目录下包括：
 * </p>
 * <p>
 * /info，包含ClusterInfo信息，包括集群份数和冗余份数设置。
 * </p>
 * <p>
 * /{node_id}/info，必须存在，包含Backend的配置信息，包括类型和位置
 * </p>
 * <p>
 * /{node_id}/status，可选，包括所有backend的状态[current/upgrading]。
 * 内容是每个backend的状态转换过程是：offline->upgrading->online->offline
 * </p>
 * <p>
 * /{node_id}/blocks/{block_id}，包含Backend下Block的名称，不包括信息
 * </p>
 * <p>
 * 要构造block->node的列表，需要客户端遍历这个结构，建立反向的索引。
 * 这样设计的目的是希望block和backend的改变只需要修改一个位置，保证原子性。
 * </p>
 * */

public class ClusterManager {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(ClusterManager.class);

    

    private class ClusterConfig {
	@SuppressWarnings("unchecked")
	public void init(int block_size) {
	    blockSize = block_size;
	    metaNodes = new LinkedList<MetaNode>();
	    writeNodes = new LinkedList<WriteNode>();
	    loadNodes = new LinkedList<LoadNode>();
	    readOnlineNodes = new List[block_size];
	    readUpgradingNodes = new List[block_size];
	    readOfflineNodes = new List[block_size];
	    readBackupNodes = new List[block_size];
	    for (int init = 0; init < block_size; ++init) {
		readOnlineNodes[init] = new ArrayList<ReadNode>();
		readUpgradingNodes[init] = new ArrayList<ReadNode>();
		readOfflineNodes[init] = new ArrayList<ReadNode>();
		readBackupNodes[init] = new ArrayList<ReadNode>();
	    }
	}

	int blockSize;
	LinkedList<MetaNode> metaNodes;
	LinkedList<WriteNode> writeNodes;
	LinkedList<LoadNode> loadNodes;
	List<ReadNode>[] readOnlineNodes;
	List<ReadNode>[] readUpgradingNodes;
	List<ReadNode>[] readOfflineNodes;
	List<ReadNode>[] readBackupNodes;

	@Override
	public String toString() {
	    StringBuffer buff = new StringBuffer();
	    buff.append("ClusterConfig [");
	    buff.append("blockSize=").append(blockSize).append(";");
	    buff.append("metaNodes=").append(metaNodes.size()).append(";");
	    buff.append("writeNodes=").append(writeNodes.size()).append(";");
	    buff.append("readOnlineNodes=[");
	    for (int i = 0; i < readOnlineNodes.length; ++i) {
		buff.append("[block=").append(i).append(
			" size=" + readOnlineNodes[i].size()).append("]");
	    }
	    buff.append("] readUpgradingNodes=[");
	    for (int i = 0; i < readUpgradingNodes.length; ++i) {
		buff.append("[block=").append(i).append(
			" size=" + readUpgradingNodes[i].size()).append("]");
	    }
	    buff.append("] readOfflineNodes=[");
	    for (int i = 0; i < readOfflineNodes.length; ++i) {
		buff.append("[block=").append(i).append(
			" size=" + readOfflineNodes[i].size()).append("]");
	    }
	    buff.append("]]");
	    return buff.toString();
	}
    }

    private ReadWriteLock _lock = new ReentrantReadWriteLock();
    private ClusterConfig _config;
    private String _name;
    private String _zkAddress;
    private static ConcurrentHashMap<String, ClusterManager> _clusterManagerMap = new ConcurrentHashMap<String, ClusterManager>();
    
    public static synchronized ClusterManager getInstance(String name, String zkAddress) {
    	ClusterManager cm = _clusterManagerMap.get(name+zkAddress);
    	if( cm == null ) {
    		cm = new ClusterManager();
    		_clusterManagerMap.put(name+zkAddress,cm);
    	}
    	return cm;
    }
    
    /**
     * 遍历zk保存的数据结构，构造block->backend关系。
     * 
     * @return BlockSize
     * @throws IOException
     * @throws InterruptedException
     * @throws KeeperException
     * @throws IOException
     * */

    public int initialize(String name, String zkAddress) throws IOException {
	_name = name;
	_zkAddress = zkAddress;
	ZkManager zm;
	try {
		System.out.println("ClusterManager,initialize.ZkManager");
	    zm = ZkManager.getInstance(_zkAddress);
	    zm.registerCluster(_name, this);
	} catch (IOException e) {
	    _logger.error("ClusterConfig.initialize initZk " + e);
	    throw e;
	}
	int block_size = -1;
	try {
		System.out.println("ClusterManager,initialize.updateConfig");
		block_size = updateConfig(zm.getZk());
	}catch(Throwable e) {
		e.printStackTrace();
		_logger.error("ClusterConfig.initialize updateConfig " + e);
    	}
	return block_size;
    }

    public int updateConfig(ZooKeeper zk) throws KeeperException, InterruptedException {
	int block_size = -1;
	ClusterConfig newconfig = new ClusterConfig();
	try {
		System.out.println("ClusterManager.updateConfig");
	    block_size = init(zk,newconfig);
	} catch (NoNodeException e) {
	    e.printStackTrace();
	    _logger.error("ClusterConfig.updateConfig " + e);
	    throw e;
	} catch (KeeperException e) {
	    e.printStackTrace();
	    _logger.error("ClusterConfig.updateConfig " + e);
	    throw e;
	}catch (InterruptedException e) {
		e.printStackTrace();
		_logger.error("ClusterConfig.updateConfig " + e);
		throw e;
	} catch (Throwable e) {
	    e.printStackTrace();
	    _logger.error("ClusterConfig.updateConfig " + e);
	}
	if (block_size == -1) {
	    return block_size;
	}
	Lock lock = _lock.writeLock();
	try {
	    lock.lock();
	    if (_config != null && _config.blockSize != newconfig.blockSize) {
	    }
	    _config = newconfig;
	} finally {
	    lock.unlock();
	}
	return block_size;
    }

    @SuppressWarnings("unchecked")
    private int init(ZooKeeper zk,final ClusterConfig config) throws NoNodeException,
	    KeeperException, InterruptedException {
	int block_size = -1;
	System.out.println("ClusterManager, init");
	try {
	    if (zk.exists("/" + _name, true) == null) {
		return -1;
	    }
	    // init the cluster info
	    if (zk.exists("/" + _name + "/info", true) == null) {
		return -1;
	    }
	    byte[] cluster_info_data = zk.getData("/" + _name + "/info", true,
		    null);
	    DistributedCacheSchema.ClusterInfo cluster_info = DistributedCacheSchema.ClusterInfo
		    .parseFrom(cluster_info_data);
	    block_size = cluster_info.getBlockSize();
	    config.init(block_size);
	    // init the backends

	    List<String> node_names = zk.getChildren("/" + _name, true);
	    System.out.println("node_names.size " + node_names.size());
	    for (String node_name : node_names) {
		if (node_name.equals("info")) {
		    continue;
		}
		if (zk.exists("/" + _name + "/" + node_name + "/info", true) == null) {
		    return -1;
		}
		byte[] node_info_data = zk.getData("/" + _name + "/" + node_name
			+ "/info", true, null);
		DistributedCacheSchema.NodeInfo node_info = DistributedCacheSchema.NodeInfo
			.parseFrom(node_info_data);
		if (zk.exists("/" + _name + "/" + node_name + "/status", true) == null) {
		    return -1;
		}
		byte[] node_status_data = zk.getData("/" + _name + "/"
			+ node_name + "/status", true, null);
		DistributedCacheSchema.NodeStatus node_status = DistributedCacheSchema.NodeStatus
			.parseFrom(node_status_data);

		DistributedCacheSchema.NodeCategoryEnum category = node_info
			.getCategory();
		DistributedCacheSchema.NodeStatusEnum status = node_status
			.getStatus();
		DistributedCacheSchema.NodeTypeEnum type = node_info.getType();
		String node_config = node_info.getConfig();
		System.out.println("NodeFactory before, category " + category + " type " + type + " node_name "
				+ node_name + " node_config " + node_config + " status " + status);
		Node node = NodeFactory.create(category, type, node_name,
			node_config);
		if (category == DistributedCacheSchema.NodeCategoryEnum.META) {
		    config.metaNodes.add((MetaNode) node);
		} else if (category == DistributedCacheSchema.NodeCategoryEnum.READ) {
		    if (zk.exists("/" + _name + "/" + node_name + "/blocks",
			    true) == null) {
			return -1;
		    }
		    List<String> blocks = zk.getChildren("/" + _name + "/"
			    + node_name + "/blocks", true);
		    for (String blockStr : blocks) {
			int block = Integer.valueOf(blockStr);
			if (status == DistributedCacheSchema.NodeStatusEnum.UPGRADING) {
				System.out.println("ReadNode.upgrading");
			    config.readUpgradingNodes[block].add((ReadNode) node);
			} else if (status == DistributedCacheSchema.NodeStatusEnum.ONLINE) {
				System.out.println("ReadNode.online");
			    config.readOnlineNodes[block].add((ReadNode) node);
			} else if (status == DistributedCacheSchema.NodeStatusEnum.OFFLINE) {
			    config.readOfflineNodes[block].add((ReadNode) node);
			}	else if (status == DistributedCacheSchema.NodeStatusEnum.BACKUP) {
				System.out.println("ReadNode.backup");
			    config.readBackupNodes[block].add((ReadNode) node);
			}
		    }
		} else if (category == DistributedCacheSchema.NodeCategoryEnum.WRITE) {
		    config.writeNodes.add((WriteNode) node);
		} else if (category == DistributedCacheSchema.NodeCategoryEnum.LOAD) {
			config.loadNodes.add((LoadNode) node);
		}
	    }
	} catch (NoNodeException e) {
	    _logger.error("ClusterConfig.init " + e);
	    throw e;
	} catch (KeeperException e) {
	    _logger.error("ClusterConfig.init " + e);
	    throw e;
	} catch (IllegalArgumentException e) {
	    _logger.error("ClusterConfig.init " + e);
	    throw e;
	} catch (InterruptedException e) {
		e.printStackTrace();
		_logger.error("ClusterConfig.init " + e);
		throw e;
	} catch (Throwable e) {
	    _logger.error("ClusterConfig.init " + e);
	    block_size = -1;
	    return block_size;
	}
	_logger.info("ClusterConfig.init " + config);
	return block_size;
    }

    public List<ReadNode> getReadNodes(int block) {
	Lock lock = _lock.readLock();
	try {
	    lock.lock();
	    return _config.readOnlineNodes[block];
	} finally {
	    lock.unlock();
	}
    }
    
    public List<ReadNode> getBackupNodes(int block) {
    	return _config.readBackupNodes[block];
    }

    public List<ReadNode> getReadNodesWithUpgrading(int block) {
	List<ReadNode> ret = new ArrayList<ReadNode>();
	List<ReadNode> online = null;
	List<ReadNode> upgrading = null;
	Lock lock = _lock.readLock();
	try {
	    lock.lock();
	    online = _config.readOnlineNodes[block];
	    upgrading = _config.readUpgradingNodes[block];
	} finally {
	    lock.unlock();
	}
	if (null != online) {
	    ret.addAll(online);
	}
	if (null != upgrading) {
	    ret.addAll(upgrading);
	}
	return ret;
    }

    public List<MetaNode> getMetaNodes() {
	Lock lock = _lock.readLock();
	try {
	    lock.lock();
	    return _config.metaNodes;
	} finally {
	    lock.unlock();
	}
    }

    public List<WriteNode> getWriteNodes() {
	Lock lock = _lock.readLock();
	try {
	    lock.lock();
	    return _config.writeNodes;
	} finally {
	    lock.unlock();
	}
    }
    
    public List<LoadNode> getLoadNodes() {
    Lock lock = _lock.readLock();
    try {
    	lock.lock();
    	return _config.loadNodes;
    } finally {
    	lock.unlock();
    }
    }

    public List<ReadNode> getUpgradingNodes(int block) {
	Lock lock = _lock.readLock();
	try {
	    lock.lock();
	    return _config.readUpgradingNodes[block];
	} finally {
	    lock.unlock();
	}
    }

    public int getNumBlocks() {
	return _config.blockSize;
    }

    public boolean allocId2Block(long id, int block) {
	for (int x = 0; x < _config.metaNodes.size(); ++x) {
	    try {
		_config.metaNodes.get(x).alloc(new Item(block, id));
		return true;
	    } catch (Throwable e) {
		e.printStackTrace();
	    }
	}
	return false;
    }

    public void setNodeStatusUpgrading2On() {
	_logger.info("ClusterManager.setNodeStatusUpgrading2On for " + _name);
	DistributedCacheSchema.NodeStatus.Builder builder = DistributedCacheSchema.NodeStatus
		.newBuilder();
	builder.setStatus(DistributedCacheSchema.NodeStatusEnum.ONLINE);

	ClusterContent content = getClusterContent();
	List<ClusterNode> nodes = content.getNodes();
	for (ClusterNode node : nodes) {
	    if (node.getNodeInfo().getCategory() == DistributedCacheSchema.NodeCategoryEnum.READ
		    && node.getNodeStatus().getStatus() == DistributedCacheSchema.NodeStatusEnum.UPGRADING) {
		try {
		    _logger.info("/" + _name + "/" + node.getName()
			    + "/status set on");
		    ZkManager.getInstance(_zkAddress).getZk().setData("/" + _name + "/" + node.getName() + "/status",
			    builder.build().toByteArray(), -1);
		} catch (Exception e) {
		    _logger.error(e);
		}
	    }
	}

    }

    public ClusterContent getClusterContent() {
	try {
		ZooKeeper zk = ZkManager.getInstance(_zkAddress).getZk();
	    byte[] cluster_info_data = zk.getData("/" + _name + "/info", true,
		    null);
	    DistributedCacheSchema.ClusterInfo cluster_info = DistributedCacheSchema.ClusterInfo
		    .parseFrom(cluster_info_data);
	    List<ClusterNode> nodes = new ArrayList<ClusterNode>();

	    List<String> node_names = zk.getChildren("/" + _name, true);
	    for (String node_name : node_names) {
		if (node_name.equals("info")) {
		    continue;
		}
		byte[] node_info_data = zk.getData("/" + _name + "/" + node_name
			+ "/info", true, null);
		DistributedCacheSchema.NodeInfo node_info = DistributedCacheSchema.NodeInfo
			.parseFrom(node_info_data);

		byte[] node_status_data = zk.getData("/" + _name + "/"
			+ node_name + "/status", true, null);
		DistributedCacheSchema.NodeStatus node_status = DistributedCacheSchema.NodeStatus
			.parseFrom(node_status_data);
		List<Integer> blockss = new ArrayList<Integer>();
		if (node_info.getCategory() == DistributedCacheSchema.NodeCategoryEnum.READ) {
		    List<String> blocks = zk.getChildren("/" + _name + "/"
			    + node_name + "/blocks", true);
		    for (String blockStr : blocks) {
			int block = Integer.valueOf(blockStr);
			blockss.add(block);
		    }
		}
		ClusterNode cluster_node = new ClusterNode(node_name,
			node_info, node_status, blockss);
		nodes.add(cluster_node);
	    }
	    return new ClusterContent(_name, cluster_info, nodes);
	} catch (Throwable e) {
	    e.printStackTrace();
	}
	return new ClusterContent("", null, null);
    }

}
