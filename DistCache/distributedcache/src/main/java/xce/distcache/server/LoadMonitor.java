package xce.distcache.server;

import java.io.IOException;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.Watcher.Event.EventType;
import org.apache.zookeeper.Watcher.Event.KeeperState;
import xce.distcache.ZkConnectionFactory;
import xce.distcache.server.LoadServerI;
/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-31
 */
public class LoadMonitor implements Watcher {
	
	private String _name;
	private ZooKeeper _zk;
	String _znode;
	private LoadServerI _server;
	
	public LoadMonitor(String name, LoadServerI server) throws IllegalArgumentException, IOException {
		this._name = name;
		this._server = server;
		this._zk = ZkConnectionFactory.create(null, this);
	}

	@Override
	public void process(WatchedEvent event) {
		// TODO Auto-generated method stub
		System.out.println("path: " + event.getPath() + " state: " + event.getState() + " type: " + event.getType());
		if (KeeperState.SyncConnected == event.getState()) {
			System.out.println("LoadMonitor.KeeperState.SyncConnected");
			_znode = event.getPath();
		}
		
		if (EventType.NodeChildrenChanged == event.getType() || 
				EventType.NodeDataChanged == event.getType()) {
			System.out.println("LoadMonitor.KeeperState.Child or Data changed!");
			_server.loadByNode();
		}	
		
		try {
			_zk.getChildren("/" + _name, this);
		} catch (KeeperException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("LoadMonitor, end!");
	}

}