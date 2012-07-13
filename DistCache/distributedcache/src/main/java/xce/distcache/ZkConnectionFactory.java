package xce.distcache;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;


public class ZkConnectionFactory {
    private static final String ZK_ADDRESS = System
	    .getProperty(
		    "zk_address",
		    "UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/DistCache");

    private static ConcurrentHashMap<String, ZooKeeper> _cachedZk = new ConcurrentHashMap<String, ZooKeeper>();
    
    public static synchronized ZooKeeper  create(String zkAddress, Watcher watcher)
	    throws IOException , IllegalArgumentException{
    	if(zkAddress == null) {
    		ZooKeeper zk = _cachedZk.get(ZK_ADDRESS);
        	if(zk == null) {
        		zk = new ZooKeeper(ZK_ADDRESS, 2000, watcher);
        		_cachedZk.put(ZK_ADDRESS, zk);
        	}
        	return zk;
    	}else {
    		ZooKeeper zk =_cachedZk.get(zkAddress);
    		if(zk == null) {
    			zk = new ZooKeeper(zkAddress, 2000, watcher);
    			_cachedZk.put(zkAddress, zk);
    		}
    		return zk;
    	}
    }
    
    public static synchronized ZooKeeper  reCreate(ZooKeeper zk, String zkAddress, Watcher watcher)
    throws IOException , IllegalArgumentException{
	if(zkAddress == null) {
		if( zk == _cachedZk.get(ZK_ADDRESS) ) {
			ZooKeeper reZk = new ZooKeeper(ZK_ADDRESS, 2000, watcher);
			_cachedZk.put(ZK_ADDRESS, reZk);
			return reZk;
		}
    	return _cachedZk.get(ZK_ADDRESS);
	}else {
		if ( zk == _cachedZk.get(zkAddress) ) {
			ZooKeeper reZk = new ZooKeeper(zkAddress, 2000, watcher);
			_cachedZk.put(zkAddress, reZk);
			return reZk;
		}
		
		return _cachedZk.get(zkAddress);
	}
}
    
}
