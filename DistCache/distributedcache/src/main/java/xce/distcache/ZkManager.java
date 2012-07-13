package xce.distcache;

import java.io.IOException;
import java.util.HashSet;
import java.util.Set;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.Watcher.Event.EventType;

public class ZkManager {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
    .getLog(ZkManager.class);
	public class ZkWatcher implements Watcher {

		public void process(WatchedEvent event) {
			_logger
		    .info("ZkManager.ZkWatcher.process get notify : path = "
			    + event.getPath()
			    + " type = "
			    + event.getType()
			    + " state = " + event.getState());
			if (event.getType() == EventType.None) {
				switch (event.getState()) {
				case SyncConnected:
				    synchronized (this) {
				    	Set<String> failClusters = notifyChangeAll();
				    	if(failClusters.size() > 0) {
				    		_reNotifyThread.addCluster(failClusters);
				    	}
				    	this.notifyAll();
				    }
				    break;
				case Expired:
				    while (true) {
				    	try {
							try {
								_zk.close();
							} catch (InterruptedException e) {
								_logger
									.error("ZkManager.ZkWatcher.process close "
											+ e);
							}
						    _zk = ZkConnectionFactory.reCreate(_zk, _zkAddress, _watcher);
						    return;
						} catch (Throwable e) {
						    _logger
							    .error("ZkManager.ZkWatcher.process reCreate "
								    + e);
						    e.printStackTrace();
						    try {
							Thread.sleep(1000);
						    } catch (Throwable te) {
							_logger
								.error("ZkManager.ZkWatcher.process Thread.sleep "
									+ e);
							continue;
						    }
						}
				    }
				}
			}else if(event.getPath().length() != 0) {
				if(notifyChange(getClusterName(event.getPath())) == false ) {
					_reNotifyThread.addCluster(getClusterName(event.getPath()));
				}
			}
		}
	}
	
	private class ReNotifyThread implements Runnable {

		private Set<String> _clusterSet = new HashSet<String>();
		
		public void addCluster(String cluster) {
			synchronized(_clusterSet) {
				_clusterSet.add(cluster);
				_clusterSet.notify();
			}
		}
		
		public void addCluster(Set<String> clusters) {
			synchronized(_clusterSet) {
				_clusterSet.addAll(clusters);
				_clusterSet.notify();
			}
		}
		
		@Override
		public void run() {
			while(true) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
					_logger.error("NotifyThread.run notifyChance() " + e1);
				}
				Set<String> clusterSet;
				synchronized(_clusterSet) {
					if(_clusterSet.size() ==0) {
						try {
							_clusterSet.wait();
						} catch (InterruptedException e) {
							_logger.error("NotifyThread.run _cluserSet.wait() " + e);
						}
					}
					clusterSet = _clusterSet;
					_clusterSet.clear();
				}
				for(String cluster : clusterSet) {
					if(notifyChange(cluster)  == false) {
						addCluster(cluster);
					}
				}
			}
		}
	}
	
	private ZooKeeper _zk;
	private String _zkAddress;
	private ZkWatcher _watcher = new ZkWatcher();
	ReNotifyThread _reNotifyThread = new ReNotifyThread();
	private ConcurrentHashMap<String, ClusterManager> _clusterMap = new ConcurrentHashMap<String, ClusterManager>();
	private static ConcurrentHashMap<String, ZkManager> _zkManagerMap = new ConcurrentHashMap<String, ZkManager>();
	
	private static final String ZK_ADDRESS = System
    .getProperty(
	    "zk_address",
	    "UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/DistCache");
	
	public static synchronized ZkManager getInstance(String zkAddress) throws IOException {
		if(zkAddress == null) {
			zkAddress = ZK_ADDRESS;
		}
		ZkManager zm = _zkManagerMap.get(zkAddress); 
		if( zm == null ) {
			zm = new ZkManager();
			try {
				zm.initialize(zkAddress);
				_zkManagerMap.put(zkAddress, zm);
			} catch (IOException e) {
				_logger.error("ZkManager.getInstance " + e);
				throw e;
			}	
		}
		return zm;
	}
	
	public boolean initialize(String zkAddress) throws IOException {
		_zkAddress = zkAddress;
		try {
		    synchronized (_watcher) {
			_zk = ZkConnectionFactory.create(zkAddress, _watcher);
			_watcher.wait();
		    }
		} catch (IllegalArgumentException e) {
		    _logger.error("ZkManager.initialize " + e);
		    throw e;
		} catch (IOException e) {
		    _logger.error("ZkManager.initialize " + e);
		    throw e;
		} catch (Throwable e) {
		    e.printStackTrace();
		    return false;
		}
		Thread thr = new Thread(_reNotifyThread);
		thr.start();
		return true;
	}
	
	public void registerCluster(String clusterName, ClusterManager clusterManager) {
		_logger.info("ZkManager.registerCluster cluster " + clusterName);
		_clusterMap.put(clusterName, clusterManager);
	}
	
	public ZooKeeper getZk() {
		return _zk;
	}
	
	private String getClusterName(String path) {
		String[] part = path.split("/");
		return part[1];
	}
	
	private boolean notifyChange(String clusterName) {
		_logger.info("ZkManager.notifyChange cluster " + clusterName);
		ClusterManager cm = _clusterMap.get(clusterName);
		if( cm != null ) {
			try {
				cm.updateConfig(_zk);	
			}catch(KeeperException e) {
				_logger.error("ZkManager.notifyChange cluster " + clusterName + " " + e);
				return false;
			}catch(InterruptedException e) {
				_logger.error("ZkManager.notifyChange cluster " + clusterName + " " + e);
				return false;
			}catch(Throwable e) {
				_logger.error("ZkManager.notifyChange cluster " + clusterName + " " + e);
			}
			return true;
		}
		return false;
	}
	
	private Set<String> notifyChangeAll() {
		_logger.info("ZkManager.notifyChangeAll");
		Set<String> failClusters = new HashSet<String>();
		for(Entry<String,ClusterManager> entry :_clusterMap.entrySet()) {
			try {
				entry.getValue().updateConfig(_zk);
			}catch(KeeperException e) {
				_logger.error("ZkManager.notifyChangeAll cluster " + entry.getKey() + " " + e);
				failClusters.add(entry.getKey());
			}catch(InterruptedException e) {
				_logger.error("ZkManager.notifyChangeAll cluster " + entry.getKey() + " " + e);
				failClusters.add(entry.getKey());
			}catch(Throwable e) {
				_logger.error("ZkManager.notifyChangeAll cluster " + entry.getKey() + " " + e);
			}
		}
		return failClusters;
	}
}
