package xce.distcache.admin;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.dom4j.Element;

public abstract class ConfigData {
    public abstract void Add(ZooKeeper zk) throws KeeperException,
	    InterruptedException;

    public abstract void Remove(ZooKeeper zk) throws InterruptedException,
	    KeeperException;

    public abstract void Update(ZooKeeper zk) throws InterruptedException,
	    KeeperException;

    public abstract void Parse(Element e) throws ParseConfigException;
}
