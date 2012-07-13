package com.renren.tripod.config.communicator;

import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.Observable;
import java.util.Observer;
import java.util.TreeMap;
import java.util.Map.Entry;
import java.util.concurrent.atomic.AtomicBoolean;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooKeeper;

import com.renren.tripod.util.Constant;
import com.renren.tripod.util.TripodException;

/**
 * 用ZooKeeper实现的通讯器，负责初始化连接和变化回推
 * 
 * Communicator与ZooKeeper一一对应
 * 
 */
public class Communicator {

    private static org.apache.commons.logging.Log logger = org.apache.commons.logging.LogFactory
            .getLog(Communicator.class);

    private String zkLocator_;

    private ZooKeeper zk_;

    private ZkWatcher watcher_;

    private AtomicBoolean connected = new AtomicBoolean(false);

    /**
     * @param locator
     * @param watch
     * @throws TripodException
     */
    public void initialize(String locator) throws TripodException {

        this.zkLocator_ = locator;
        if (null == this.watcher_) {
            this.watcher_ = new ZkWatcher();
        }
        this.zk_ = null;
        try {
            initZk(zkLocator_);
        } catch (TripodException e) {
            throw e;
        }
    }

    /**
     * @param observer
     * @throws TripodException
     */
    public void addObserver(Observer observer) throws TripodException {
        watcher_.addObserver(observer);
    }

    /**
     * 读取路径数据信息
     * 
     * @param path
     * @return
     * @throws TripodException
     */
    public byte[] getData(String path) throws TripodException {
        try {
            return this.zk_.getData(path, true, null);
        } catch (KeeperException e) {
            e.printStackTrace();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            throw new TripodException(e);
        }
    }

    /**
     * 读取路径下孩子信息
     * 
     * @param path
     * @return
     * @throws TripodException
     */
    public List<String> getChildren(String path) throws TripodException {
        try {
            return this.zk_.getChildren(path, false);
        } catch (KeeperException e) {
            e.printStackTrace();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            throw new TripodException(e);
        }
    }

    /**
     * 封装ZooKeeper读方法，读取该节点所有信息
     * 
     * @param path 对应ZooKeeper路径
     * @return 返回该路径所有节点信息
     * @throws TripodException
     */
    public Map<String, byte[]> getAll(String path) throws TripodException {

        try {
            return parseSubTree(path);
        } catch (KeeperException e) {
            e.printStackTrace();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            throw new TripodException(e);
        }
    }

    /**
     * 创建对应路径节点及信息
     * 
     * @param path
     * @param data
     * @throws TripodException
     */
    public void add(String path, byte[] data) throws TripodException {
        try {
            obtainUpgradingLock();
            zk_.create(path, data, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);

        } catch (TripodException e) {
            releaseUpgradingLock();
            throw new TripodException(e);
        } catch (KeeperException e) {
            e.printStackTrace();
            releaseUpgradingLock();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            releaseUpgradingLock();
            throw new TripodException(e);
        }
        releaseUpgradingLock();
    }

    /**
     * 批量创建对应路径节点及信息
     * 
     * @param config
     * @throws TripodException
     */
    public void add(List<Entry<String, byte[]>> config) throws TripodException {
        try {
            obtainUpgradingLock();
        } catch (TripodException e) {
            releaseUpgradingLock();
            throw new TripodException(e);
        }
        for (Entry<String, byte[]> entry : config) {
            try {
                zk_.create(entry.getKey(), entry.getValue(), ZooDefs.Ids.OPEN_ACL_UNSAFE,
                        CreateMode.PERSISTENT);
            } catch (KeeperException e) {
                e.printStackTrace();
                throw new TripodException(e);
            } catch (InterruptedException e) {
                e.printStackTrace();
                throw new TripodException(e);
            }
        }
        releaseUpgradingLock();
    }

    /**
     * 设置对应路径节点的数据信息
     * 
     * @param path
     * @param data
     * @throws TripodException
     */
    public void set(String path, byte[] data) throws TripodException {
        try {
            obtainUpgradingLock();
            zk_.setData(path, data, -1);
        } catch (TripodException e) {
            releaseUpgradingLock();
            throw new TripodException(e);
        } catch (KeeperException e) {
            e.printStackTrace();
            releaseUpgradingLock();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            releaseUpgradingLock();
            throw new TripodException(e);
        }

        releaseUpgradingLock();
    }

    /**
     * 批量设置对应路径节点的数据信息
     * 
     * @param config
     * @throws TripodException
     */
    public void set(List<Entry<String, byte[]>> config) throws TripodException {
        try {
            obtainUpgradingLock();
        } catch (TripodException e) {
            releaseUpgradingLock();
            throw new TripodException(e);
        }
        for (Entry<String, byte[]> entry : config) {
            try {
                zk_.setData(entry.getKey(), entry.getValue(), -1);
            } catch (KeeperException e) {
                e.printStackTrace();
                throw new TripodException(e);
            } catch (InterruptedException e) {
                e.printStackTrace();
                throw new TripodException(e);
            }
        }
        releaseUpgradingLock();
    }

    /**
     * 删除对应路径节点
     * 
     * @param path
     * @throws TripodException
     */
    public void delete(String path) throws TripodException {
        try {
            obtainUpgradingLock();
            zk_.delete(path, -1);
        } catch (TripodException e) {
            releaseUpgradingLock();
            throw new TripodException(e);
        } catch (KeeperException e) {
            e.printStackTrace();
            releaseUpgradingLock();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            releaseUpgradingLock();
            throw new TripodException(e);
        }

        releaseUpgradingLock();
    }

    /**
     * 批量删除对应路径节点
     * 
     * @param config
     * @throws TripodException
     */
    public void delete(List<Entry<String, byte[]>> config) throws TripodException {
        try {
            obtainUpgradingLock();
        } catch (TripodException e) {
            releaseUpgradingLock();
            throw new TripodException(e);
        }
        for (Entry<String, byte[]> entry : config) {
            try {
                zk_.delete(entry.getKey(), -1);
            } catch (KeeperException e) {
                e.printStackTrace();
                throw new TripodException(e);
            } catch (InterruptedException e) {
                e.printStackTrace();
                throw new TripodException(e);
            }
        }
        releaseUpgradingLock();
    }

    /**
     * 初始化ZooKeeper,创建新链接
     * 
     * @param zkLocater
     * @param watch
     * @throws TripodException
     */
    private void initZk(String zkLocater) throws TripodException {

        if (null != this.zk_) {
            if (ZooKeeper.States.CONNECTED == this.zk_.getState()) {
                logger
                        .info("ZooKeeperCommunicator.initZk is already connected, initZk do nothing.");
                return;
            }
            try {
                this.zk_.close();
                connected.set(false);
                this.zk_ = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        if (null == this.zk_) {
            synchronized (this) {
                if (null == this.zk_) {
                    try {

                        this.zk_ = new ZooKeeper(zkLocater, Constant.ZOOKEEPER_SESSION_TIMEOUT,
                                this.watcher_);
                        this.watcher_.wait();
                        logger.info("ZooKeeperCommunicator.initZk succeed.");

                    } catch (IllegalArgumentException e) {
                        logger.error("ZooKeeperCommunicator.initZk " + e);
                        throw new TripodException(
                                "ZooKeeperCommunicator.initZk IllegalArgumentException", e);
                    } catch (IOException e) {
                        logger.error("ZooKeeperCommunicator.initZk " + e);
                        throw new TripodException("ZooKeeperCommunicator.initZk IOException", e);
                    } catch (Throwable e) {
                        logger.error("ZooKeeperCommunicator.initZk " + e);
                        throw new TripodException("ZooKeeperCommunicator.initZk UnknownException",
                                e);
                    }
                }
            }
        }
    }

    /**
     * Watcher实现类 负责捕获ZooKeeper变更，同时将变更通知其他观察者 观察"/Tripod"的变化
     */
    private class ZkWatcher extends Observable implements Watcher {

        @Override
        public void process(WatchedEvent event) {
            String path = event.getPath();
            switch (event.getState()) {
                case Expired:
                case Disconnected:
                    logger
                            .warn(" [ZooKeeperCommunicator::ZkWatcher::process] connection expired or disconnected.");
                    //重建ZooKeeper连接
                    while (true) {
                        try {
                            initZk(zkLocator_);
                            logger
                                    .warn(" [ZooKeeperCommunicator::ZkWatcher::process] reConnecting...");
                            break;
                        } catch (TripodException e) {
                            logger
                                    .warn(" [ZooKeeperCommunicator::ZkWatcher::process] reConnecting... TripodException"
                                            + e);
                        }
                        try {
                            Thread.sleep(Constant.ZOOKEEPER_RECONNECT_INTERVAL);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    break;
                case SyncConnected:

                    this.notifyAll();

                    switch (event.getType()) {
                        case NodeChildrenChanged:
                        case NodeCreated:
                        case NodeDataChanged:
                        case NodeDeleted:
                            this.setChanged();
                            this.notifyObservers(path);
                            try {
                                zk_.getChildren("/", watcher_);
                            } catch (KeeperException e) {
                                e.printStackTrace();
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            break;
                        default:// 

                    }

                    break;
                default:
                    break;
            }

        }

    }

    /**
     * 递归分析子树
     * 
     * @param subPath
     * @return
     * @throws KeeperException
     * @throws InterruptedException
     */
    private Map<String, byte[]> parseSubTree(String subPath) throws KeeperException,
            InterruptedException {
        Map<String, byte[]> result = new TreeMap<String, byte[]>();

        if (this.zk_.exists(subPath, true) == null) {
            return result;
        }

        List<String> children = this.zk_.getChildren(subPath, false);
        if (children.isEmpty()) {
            result.put(subPath, this.zk_.getData(subPath, true, null));
        } else {
            for (String child : children) {
                String childPath = subPath.equals("/") ? subPath + child : subPath + "/" + child;
                result.putAll(parseSubTree(childPath));
            }
        }
        return result;
    }

    /**
     * 获取分布式锁
     * 
     * @throws TripodException
     */
    private void obtainUpgradingLock() throws TripodException {
        try {
            zk_.create("/upgrading_lock", new byte[0], ZooDefs.Ids.OPEN_ACL_UNSAFE,
                    CreateMode.EPHEMERAL);

        } catch (KeeperException e) {
            e.printStackTrace();
            throw new TripodException(" obtainUpgradingLock KeeperException", e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            throw new TripodException(" obtainUpgradingLock InterruptedException", e);
        }
    }

    /**
     * 释放分布式锁
     * 
     * @throws TripodException
     */
    private void releaseUpgradingLock() throws TripodException {
        try {
            zk_.delete("/upgrading_lock", -1);
        } catch (KeeperException e) {
            e.printStackTrace();
            throw new TripodException(" releaseUpgradingLock KeeperException", e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            throw new TripodException(" releaseUpgradingLock InterruptedException", e);
        }
    }

    /**
     * 判断对应路径节点是否存在
     * 
     * @param path
     * @throws TripodException
     */
    private boolean exists(String path) throws TripodException {
        try {
            if (zk_.exists(path, null) == null) {
                return false;
            }
        } catch (KeeperException e) {
            e.printStackTrace();
            throw new TripodException(e);
        } catch (InterruptedException e) {
            e.printStackTrace();
            throw new TripodException(e);
        }
        return true;
    }

    /**
     * 批量判断对应路径节点是否存在
     * 
     * @param config
     * @throws TripodException
     */
    private boolean exists(List<Entry<String, byte[]>> config) throws TripodException {
        for (Entry<String, byte[]> entry : config) {
            try {
                if (zk_.exists(entry.getKey(), null) == null) {
                    return false;
                }
            } catch (KeeperException e) {
                e.printStackTrace();
                throw new TripodException(e);
            } catch (InterruptedException e) {
                e.printStackTrace();
                throw new TripodException(e);
            }
        }
        return true;
    }

}
