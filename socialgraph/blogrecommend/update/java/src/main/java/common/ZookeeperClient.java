package common;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.Watcher.Event.EventType;
import org.apache.zookeeper.Watcher.Event.KeeperState;

/**
 * socialgraph java zk的客户端
 * 
 * @author 李勇(liyong@opi-corp.com)
 * 
 */
public class ZookeeperClient extends Subject implements Watcher {

    /** zookeeper的结点信息 */
    private static final String HOST_PORTS = "10.3.17.99:2181,10.3.17.98:2181,10.3.17.102:2181,10.3.19.60:2181,10.3.17.61:2181";

    private static ZookeeperClient instance = null;

    private ZooKeeper zk;

    private boolean isAlive;

    /** 本地缓存 */
    private ConcurrentHashMap<String, String> map = null;

    private ZookeeperClient() {
        init();
    }

    public static ZookeeperClient getInstance() {
        if (instance == null) {
            synchronized (ZookeeperClient.class) {
                if (instance == null) {
                    try {
                        instance = new ZookeeperClient();
                    } catch (Exception e) {
                        e.printStackTrace();
                        instance = null;
                    }
                }
            }
        }
        return instance;
    }

    private void init() {
        try {
            map = new ConcurrentHashMap<String, String>();
            zk = new ZooKeeper(HOST_PORTS, 5000, this);
            isAlive = zk.getState().isAlive();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 对外取znode值的接口 如果结点不存在,则返回null
     * 
     * @param zNodePath 如:/test
     * @return
     */
    public String getValue(final String zNodePath) {
        if (map.contains(zNodePath)) return map.get(zNodePath);
        if (isAlive) {
            byte[] bytes = null;
            try {
                bytes = zk.getData(zNodePath, true, null);
            } catch (KeeperException e) {
                return null;
            } catch (InterruptedException e) {
                e.printStackTrace();
                return null;
            }
            if (bytes != null && bytes.length != 0) {
                String data = new String(bytes);
                return data;
            }
        }
        return null;
    }

    /** 更新本地缓存 */
    private void update(final String zNodePath) {
        String zNodeValue = null;
        if (isAlive) {
            byte[] bytes = null;
            try {
                bytes = zk.getData(zNodePath, true, null);
            } catch (KeeperException e) {} catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (bytes != null && bytes.length != 0) {
                zNodeValue = new String(bytes);
            }
        }
        if (zNodeValue != null) {
            map.put(zNodePath, zNodeValue);
            notifyObservers(zNodePath, zNodeValue);
        }
    }

    /** 更新本地缓存 */
    private void delete(final String zNodePath) {
        map.remove(zNodePath);
    }

    /** 当zk server信息改变时的回调方法 */
    public void process(WatchedEvent event) {
        String zNodePath = event.getPath();
        EventType eventType = event.getType();
        KeeperState state = event.getState();
        if (state == KeeperState.SyncConnected) {
            switch (eventType.getIntValue()) {
                case -1: // None
                    break;
                case 1: // NodeCreated
                    update(zNodePath);
                    break;
                case 2: // NodeDeleted
                    delete(zNodePath);
                    break;
                case 3: // NodeDataChanged
                    update(zNodePath);
                    break;
                case 4: // NodeChildrenChanged
                    break;
                default:
                    break;
            }
        }

    }

    //    public static void main(String[] args) throws KeeperException, InterruptedException {
    //        BasicConfigurator.configure();
    //        ZookeeperClient client = ZookeeperClient.getInstance();
    //
    //        System.out.println(client.getValue("/test"));
    //
    //        TimeUnit.DAYS.sleep(1);
    //
    //    }
}
