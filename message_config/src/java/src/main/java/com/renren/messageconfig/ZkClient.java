package com.renren.messageconfig;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.log4j.BasicConfigurator;

/**
 * message conf ZkClient
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class ZkClient {

    private static Log logger = LogFactory.getLog(ZkClient.class);

    private static String DEFAULT_ZK_ADDRESS = "10.22.206.32:2181";
    // private static String DEFAULT_ZK_ADDRESS = "10.2.78.253:2181";

    private static String DEFAULT_PREFIX = "/MessagePipe";

    static final String UPDATE_ALL_NAMESPACE = "HOLY_SHIT_UPDATE_ALL_NAMESPACE";

    private static ZkClient defaultInstance = null;

    private static Map<String, ZkClient> zkClientMap = new HashMap<String, ZkClient>();
    private static Object zkClientMapGate = new Object();
    //
    private String zkAddress;

    private ZkManager zkManager;

    private boolean valid;

    private Map<String, List<NamespaceListener>> listeners;

    private ReentrantReadWriteLock listenerRWLock;

    private Map<String, String> znodeVersionMap;

    private Object znodeVersionLock;

    private ZkClient(String zkAddress) throws MessageConfigException {
	this.zkAddress = zkAddress;
	initialize();
    }

    public static ZkClient getInstance(String zkAddress)
	    throws MessageConfigException {
	ZkClient zkClient = null;
	synchronized (zkClientMapGate) {
	    zkClient = zkClientMap.get(zkAddress);
	    if (zkClient != null) {
		return zkClient;
	    } else {
		try {
		    zkClient = new ZkClient(zkAddress);
		} catch (Exception e) {
		    logger.error("Zkclient.getInstance(String) zkAddress:"
			    + zkAddress);
		}
	    }
	}
	return zkClient;
    }

    private boolean initialize() throws MessageConfigException {
	zkManager = ZkManagerFactory.get(zkAddress);
	valid = zkManager != null ? true : false;
	if (!isValid()) {
	    throw new MessageConfigException(
		    "ZkManagerFactory.get return null\t zkAddress:" + zkAddress);
	}
	zkManager.setZkClient(this);
	listeners = new HashMap<String, List<NamespaceListener>>();
	listenerRWLock = new ReentrantReadWriteLock();
	znodeVersionMap = new HashMap<String, String>();
	znodeVersionLock = new Object();
	return isValid();
    }

    public boolean isValid() {
	return valid == true;
    }

    /**
     * 被外面回调的更新方法
     * 
     * @param znodePath
     * @return
     */
    public boolean update(String znodePath) {
	if (UPDATE_ALL_NAMESPACE.equals(znodePath)) {
	    return updateAllNamespaces();
	}
	boolean flag = updateNamespace(znodePath);
	logger.info("ZkClient.update() flag:" + flag);
	return flag;
    }

    /**
     * 取出namespace下,node name list
     * 
     * @param namespace
     * @return
     */
    public List<String> getNodes(String namespace) {
	if ("".equals(namespace)) {
	    return null;
	}
	String znodePath = getNamespaceZnodePath(namespace);
	return getConfigDataFromZkManager(znodePath).getChildrenNames();
    }

    /**
     * 增加namespace listener
     * 
     * @param listener
     */
    public void addNamespaceListener(NamespaceListener listener) {
	try {
	    listenerRWLock.writeLock().lock();
	    List<NamespaceListener> listenerList = listeners.get(listener
		    .getNamespace());
	    if (listenerList != null) {
		listenerList.add(listener);
	    } else {
		listenerList = new ArrayList<NamespaceListener>();
		listenerList.add(listener);
		listeners.put(listener.getNamespace(), listenerList);
	    }
	} finally {
	    listenerRWLock.writeLock().unlock();
	}
    }

    private ConfigData getConfigDataFromZkManager(String znodePath) {
	String namespace = znodePath.substring(znodePath.lastIndexOf("/") + 1,
		znodePath.length());
	ConfigData msg = null;
	String version = zkManager.getData(znodePath);
	msg = new ConfigData(namespace, version);
	List<String> childrenNames = zkManager.getChildren(znodePath);
	if (childrenNames != null && !childrenNames.isEmpty()) {
	    for (String childName : childrenNames) {
		msg.addChild(childName, ConfigData.DEFAULT_VERSION);
	    }
	}
	return msg;
    }

    /**
     * when holy shit happens!!
     * 
     * @return
     */
    private boolean updateAllNamespaces() {
	boolean flag = true;
	try {
	    listenerRWLock.readLock().lock();
	    for (String namespace : listeners.keySet()) {
		boolean result = updateNamespace(getNamespaceZnodePath(namespace));
		if (!result) {
		    flag = false;
		}
	    }
	} finally {
	    listenerRWLock.readLock().unlock();
	}
	logger.info("ZkClient.updateAllNamespaces() flag:" + flag);
	return flag;
    }

    private boolean updateNamespace(String znodePath) {
	boolean flag = true;
	String newVersion = getNamespaceVersion(znodePath);
	synchronized (znodeVersionLock) {
	    String oldVersion = znodeVersionMap.get(znodePath);
	    if (newVersion.equals(oldVersion)) {
		return true;
	    }
	    znodeVersionMap.put(znodePath, newVersion);
	}
	ConfigData configData = getConfigDataFromZkManager(znodePath);
	logger.info("ZkClient.updateNamespace() znodePath:" + znodePath + "\t"
		+ configData.toString());
	try {
	    listenerRWLock.readLock().lock();
	    List<NamespaceListener> listenerList = listeners.get(configData
		    .getName());
	    if (listenerList == null) {
		logger.info("namespace:" + configData.getName()
			+ " listenerList is null.");
		return true;
	    }
	    for (NamespaceListener listener : listenerList) {
		boolean ret = listener.update(configData.getChildrenNames());
		if (!ret) {
		    flag = ret;
		}
	    }
	} finally {
	    listenerRWLock.readLock().unlock();
	}
	return flag;
    }

    private String getNamespaceVersion(String znodePath) {
	return zkManager.getData(znodePath);
    }

    private static String getNamespaceZnodePath(String namespace) {
	return DEFAULT_PREFIX + "/" + namespace;
    }

    /*
     * public static void main(String[] args) { BasicConfigurator.configure();
     * ZkClient zc = null; try { zc = ZkClient.getInstance("10.2.78.253:2181");
     * } catch (MessageConfigException e1) { e1.printStackTrace(); }
     * 
     * zc.addNamespaceListener(new NamespaceListener("test2") {
     * 
     * @Override public boolean update(List<String> childrenNameList) {
     * System.out.println("update... ns:" + getNamespace() + "\tchildren:" +
     * childrenNameList); return true; } });
     * 
     * System.out.println(zc.getNodes("test2"));
     * 
     * try { TimeUnit.DAYS.sleep(1); } catch (InterruptedException e) {
     * logger.error(e); }
     * 
     * System.exit(0); }
     */
}
