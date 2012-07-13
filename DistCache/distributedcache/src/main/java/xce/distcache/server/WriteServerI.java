package xce.distcache.server;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.Watcher.Event.EventType;


import xce.distcache.ClusterManager;
import xce.distcache.DistributedCacheSchema;
import xce.distcache.Distribution;

import xce.distcache.node.ReadNode;
import xce.distcache.node.Node.Item;
import xce.distcache.tools.Tools;
import Ice.Current;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.LogManager;;

public class WriteServerI extends xce.distcache.server._WriteServerDisp {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(WriteServerI.class);
    private static final long serialVersionUID = 3270642203159936469L;
    private ZooKeeper _zk;
    private BlockingQueue<Runnable> _queue;
    private ThreadPoolExecutor _exec;
    private static Map<String, ClusterClient> _clusterClients;
    private ReadWriteLock _lock = new ReentrantReadWriteLock();
    private ClusterWatcher _watcher = new ClusterWatcher();
    private static final String ZK_ADDRESS = System
    .getProperty(
	    "write_zk_address",
	    "UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181");
    private class ClusterWatcher implements Watcher {


	public void process(WatchedEvent event) {
	    _logger
		    .info("WriteServerI.ClusterWatcher.process get notify : path = "
			    + event.getPath()
			    + " type = "
			    + event.getType()
			    + " state = " + event.getState());
	    if (event.getType() == EventType.None) {
		switch (event.getState()) {
			case SyncConnected:
			    synchronized(this) {
				this.notifyAll();
			    }
			    break;
			case Expired:
			    while(true) {
				try {
					try {
						_zk.close();
					}catch(InterruptedException e) {
						_logger.error("WriteServerI.ClusterWatcher.process close zk " + e);
					}
				    _zk = new ZooKeeper(ZK_ADDRESS,2000,_watcher);
				    return;
				}catch(Throwable e) {
				    _logger.error("WriteServerI.ClusterWatcher.process initZk " + e);
				    e.printStackTrace();
				    try {
					Thread.sleep(1000);
				    } catch(Throwable te) {
					continue;
				    }
				}
			    }
		}
	    }

	    try {
		updateClients();
	    } catch (Exception e) {
		_logger.error("WriteServerI.ClusterWatcher.process " + e);
	    }
	}
    }

    private class ClusterClient {
	private ClusterManager _clusterManager;
	private Distribution _dist;

	public ClusterClient(ClusterManager manager, Distribution dist) {
	    _clusterManager = manager;
	    _dist = dist;
	}

	public ClusterManager getClusterManager() {
	    return _clusterManager;
	}

	public Distribution getDist() {
	    return _dist;
	}
    }
    
    private boolean initZk() throws IOException {
    	try {
    		synchronized(_watcher) {
    			_zk = new ZooKeeper(ZK_ADDRESS,2000,_watcher);
    			_watcher.wait();
    		}
    	} catch (IllegalArgumentException e) {
			_logger.error("WriteServerI.initZk " + e);
			throw e;
    	} catch (IOException e) {
    		_logger.error("WriteServerI.initZk " + e);
			throw e;
    	} catch (Throwable e) {
    	    e.printStackTrace();
    	}
    	return true;
    }

    private void updateClients() throws KeeperException, InterruptedException,
	    IOException {
	Map<String, ClusterClient> clusterClients = new HashMap<String, ClusterClient>();
	if(_zk.exists("/DistCache", true)==null){
	    return;
	}
	
	List<String> clusters = _zk.getChildren("/DistCache", true);

	for (String cluster : clusters) {
	    if (cluster.equals("upgrading_lock")) {
		continue;
	    }
	    DistributedCacheSchema.ClusterInfo ci;
	    ci = Tools.readClusterInfo(_zk, "DistCache/"+cluster, true);
	    if(ci==null){
		continue;
	    }
	    ClusterManager clusterManager = ClusterManager.getInstance(cluster,"");
	    clusterManager.initialize(cluster, null);
	    Distribution dist = Distribution.instance(ci.getBlockSize());
	    clusterClients
		    .put(cluster, new ClusterClient(clusterManager, dist));
	}
	Lock lock = _lock.writeLock();
	try {
	    lock.lock();
	    _clusterClients = clusterClients;
	    _logger.info("WriteServerI.updateClients done");
	} finally {
	    lock.unlock();
	}
    }

    protected void initialize() throws IOException, KeeperException,
	    InterruptedException {
	_clusterClients = new HashMap<String, ClusterClient>();
	_queue = new LinkedBlockingDeque<Runnable>();
	_exec = new ThreadPoolExecutor(40, 100, 10, TimeUnit.SECONDS, _queue);
	initZk();
	updateClients();
    }
    
    @Override
    public void setLogLevel(String level, Current current) {
    	Logger logger = LogManager.getLogger(WriteServerI.class);
    	Level newLevel = Level.toLevel(level);
    	logger.setLevel(newLevel);
    	 _logger.info("WriteServerI.setLogLevel WriteServer new level = " + logger.getLevel());
     	logger = LogManager.getLogger("XceCache");
    	logger.setLevel(newLevel);
    	 _logger.info("WriteServerI.setLogLevel XceCache new level = " + logger.getLevel());
    }
    
    public boolean set(String cluster, long id, int block, byte[] data,
	    boolean isSync, Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("WriteServerI::set id=" + id + " block=" + block
		    + " cluster=" + cluster + " data size: " + data.length);
	}
	if (isSync) {
	    try {
		ClusterClient cc = null;
		Lock lock = _lock.readLock();
		try {
		    lock.lock();
		    cc = _clusterClients.get(cluster);
		} finally {
		    lock.unlock();
		}
		List<ReadNode> x = cc.getClusterManager()
			.getReadNodesWithUpgrading(block);
		// _clusterManager.allocId2Block(id, block);
		for (ReadNode xx : x) {
		    if (xx.internalSet(new Item(block, id), data) == false) {
			return false;
		    }
		}
		return true;
	    } catch (Throwable e) {
		_logger.error("WriteServerI::set " + id + " " + block, e);
		return false;
	    }
	} else {
	    Map<Long, byte[]> datas = new HashMap<Long, byte[]>();
	    datas.put(id, data);
	    _exec.execute(new SetTask(cluster, datas));
	}
	return false;
    }

    public List<Long> setBatch(String clustername, Map<Long, byte[]> datas,
	    boolean isSync, Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("WriteServerI::setBatch datas.size: " + datas.size()
		    + " for cluster: " + clustername);
	}

	List<Long> res = new ArrayList<Long>();
	if (isSync) {
	    Lock lock = _lock.readLock();
	    ClusterClient cc = null;
	    try {
		lock.lock();
		cc = _clusterClients.get(clustername);
	    } finally {
		lock.unlock();
	    }
	    if (cc == null) {
		return res;
	    }
	    for (Entry<Long, byte[]> entry : datas.entrySet()) {
		long id = entry.getKey();
		int block = cc.getDist().get(id);
		try {
		    List<ReadNode> x = cc.getClusterManager()
			    .getReadNodesWithUpgrading(block);
		    for (ReadNode xx : x) {
			if (xx.internalSet(new Item(block, id), (byte[]) entry
				.getValue()) == false) {
			    res.add(id);
			    break;
			}
		    }
		} catch (Throwable e) {
		    res.add(id);
		    _logger.error("WriteServerI::setBatch error " + id + block,
			    e);
		}
	    }
	    return res;
	} else {
	    _exec.execute(new SetTask(clustername, datas));
	}
	return res;
    }

    public boolean setCAS(String cluster, long id, int block, byte[] data,
	    long cas, Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("WriteServerI::setCAS id=" + id + " block=" + block
		    + " cluster=" + cluster + " data size: " + data.length
		    + " cas=" + cas);
	}
	try {
	    ClusterClient cc = null;
	    Lock lock = _lock.readLock();
	    try {
		lock.lock();
		cc = _clusterClients.get(cluster);
	    } finally {
		lock.unlock();
	    }
	    List<ReadNode> x = cc.getClusterManager()
		    .getReadNodesWithUpgrading(block);
	    // _clusterManager.allocId2Block(id, block);
	    for (ReadNode xx : x) {
		if (xx.casSet(new Item(block, id, cas), data) == false) {
		    return false;
		}
	    }
	    return true;
	} catch (Throwable e) {
	    _logger.error("WriteServerI::setCAS " + id + " " + block, e);
	    return false;
	}
    }

    public List<Long> setBatchCAS(String clustername, Map<Long, CASData> datas,
	    Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("WriteServerI::setBatchCAS datas.size: "
		    + datas.size() + " for cluster: " + clustername);
	}

	Lock lock = _lock.readLock();
	ClusterClient cc = null;
	try {
	    lock.lock();
	    cc = _clusterClients.get(clustername);
	} finally {
	    lock.unlock();
	}

	List<Long> res = new ArrayList<Long>();

	if (cc == null) {
	    return res;
	}

	for (Entry<Long, CASData> entry : datas.entrySet()) {
	    long id = entry.getKey();
	    int block = cc.getDist().get(id);
	    try {
		List<ReadNode> x = cc.getClusterManager()
			.getReadNodesWithUpgrading(block);
		for (ReadNode xx : x) {
		    if (xx.casSet(new Item(block, id, entry.getValue().cas),
			    (byte[]) entry.getValue().data) == false) {
			res.add(id);
			break;
		    }
		}
	    } catch (Throwable e) {
		res.add(id);
		_logger.error("WriteServerI::setBatchCAS error " + id + block,
			e);
	    }
	}

	return res;
    }

    public boolean addBatch(String cluster, Map<Long, byte[]> datas,
	    boolean isSync, Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("WriteServerI::addBatch datas.size: " + datas.size()
		    + " cluster: " + cluster + " isSync: " + isSync);
	}

	if (isSync) {
	    ClusterClient cc = null;
	    Lock lock = _lock.readLock();
	    try {
		lock.lock();
		cc = _clusterClients.get(cluster);
	    } finally {
		lock.unlock();
	    }
	    if (cc == null) {
		_logger
			.error("WriteServerI::addBatch ClusterClient is Null for "
				+ cluster);
		return false;
	    }
	    boolean flag = true;
	    for (Entry<Long, byte[]> entry : datas.entrySet()) {
		long id = entry.getKey();
		int block = cc.getDist().get(id);
		try {
		    List<ReadNode> x = cc.getClusterManager()
			    .getUpgradingNodes(block);
		    for (ReadNode xx : x) {
			Item item = new Item(block, id);
			byte[] value = (byte[]) entry.getValue();
			xx.add(item, value);
		    }
		} catch (Throwable e) {
		    flag = false;
		    _logger.error("WriteServerI::addBatch error " + id + block,
			    e);
		}
	    }
	    return flag;
	}
	return true;
    }

    public void close() throws InterruptedException {
    }

    private class SetTask implements Runnable {
	private String _cluster;
	private Map<Long, byte[]> _datas;

	SetTask(String cluster, Map<Long, byte[]> datas) {
	    _cluster = cluster;
	    _datas = datas;
	}

	@Override
	public void run() {
	    ClusterClient cc = null;
	    Lock lock = _lock.readLock();
	    try {
		lock.lock();
		cc = _clusterClients.get(_cluster);
	    } finally {
		lock.unlock();
	    }
	    if (cc == null) {
		_logger.error("WriteServerI::run cluster: " + _cluster
			+ " is NULL");
	    }
	    for (Entry<Long, byte[]> entry : _datas.entrySet()) {
		long id = entry.getKey();
		int block = cc.getDist().get(id);
		try {
		    List<ReadNode> x = cc.getClusterManager()
			    .getReadNodesWithUpgrading(block);
		    for (ReadNode xx : x) {
			xx.internalSet(new Item(block, id), (byte[]) entry
				.getValue());
		    }
		} catch (Throwable e) {
		    _logger.error("WriteServerI::run error " + id + block, e);
		}
	    }
	}
    }
}
