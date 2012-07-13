package xce.distcache.node.impl;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import com.danga.MemCached.MemCachedClient;
import com.danga.MemCached.SockIOPool;

import xce.distcache.node.Node;
import xce.distcache.node.ReadNode;

public class MemcachedReadNode extends BaseNode implements ReadNode {
    private Logger l = Logger.getLogger("XceCache");
    private static final long EXPIRE = Integer.MAX_VALUE * 1000l;
    private MemCachedClient client;
    private String prefix;

    @Override
    public void initialize(String name, String config) {
	super.initialize(name, config);
	/*
	 * The config string format: <host>:<port>:<key_prefix>
	 */
	String[] part = config.split(":");
	if (part.length != 3) {
	    throw new IllegalArgumentException("The config string" + config
		    + " not match the format: <host>:<port>:<key_prefix>");
	}
	try {
	    String[] address = new String[] { part[0] + ":" + part[1] };
	    SockIOPool pool = SockIOPool.getInstance(part[0] + ":" + part[1]);
	    pool.setServers(address);
	    pool.setInitConn(1);
	    pool.setMinConn(1);
	    pool.setMaxConn(50);
	    pool.setMaxIdle(1000 * 60 * 60 * 6);
	    pool.setNagle(false);
	    pool.setSocketTO(300);
	    pool.setMaintSleep(30);
	    pool.setSocketConnectTO(0);
	    pool.initialize();

	    client = new MemCachedClient(part[0] + ":" + part[1]);
	    prefix = part[2];
	} catch (Throwable e) {
	    e.printStackTrace();
	    throw new IllegalArgumentException(
		    "Error while construct MemcachedClient, see stderr for stack detail. "
			    + e.getMessage());
	}
    }

    private String getKey(Node.Item item) {
	return prefix + "_" + item.block + "_" + item.id;
    }

    private Node.Item getKey(String str) {
	try {
	    if (str.startsWith(prefix)) {
		int pos_block_start = prefix.length() + 1;
		int pos_block_end = str.indexOf('_', pos_block_start);
		int pos_id_start = pos_block_end + 1;
		int pos_id_end = str.length();
		int block = Integer.valueOf(str.substring(pos_block_start,
			pos_block_end));
		long id = Long.valueOf(str.substring(pos_id_start, pos_id_end));
		return new Node.Item(block, id);
	    }
	} catch (Throwable e) {
	    e.printStackTrace();
	}
	return null;
    }

    @Override
    public byte[] get(Node.Item id) {
	if (l.isDebugEnabled()) {
	    l.debug(_name + ":" + _config + " get " + id);
	}
	return (byte[]) client.get(getKey(id));
    }

    @Override
    public boolean internalSet(Node.Item id, byte[] data) {
	if (l.isDebugEnabled()) {
	    l.debug(_name + ":" + _config + " set " + getKey(id)
		    + " data size: " + data.length);
	}
	if (data == null)
	    return false;
	// client.set(getKey(id), data, new Date(new Date().getTime()+EXPIRE));
	return client.set(getKey(id), data);
    }

    public boolean casSet(Node.Item id, byte[] data) {
	if (l.isDebugEnabled()) {
	    l.debug(_name + ":" + _config + " set " + getKey(id)
		    + " data size: " + data.length + " cas " + id.cas);
	}
	if (data == null)
	    return false;
	return client.cas(getKey(id), data, id.cas);
    }

    public void add(Node.Item id, byte[] data) {
	/*
	 * if (l.isDebugEnabled()) { l.debug(name_ + ":" + config_ + " add " +
	 * getKey(id) + " data size: " + data.length); }
	 */
	if (data == null)
	    return;
	client.set(getKey(id), data);
    }

    public static void main(String[] args) throws Throwable {
    }

    @Override
    public void close() {
	try {
	    client = null;
	} catch (Throwable e) {
	    e.printStackTrace();
	}
    }

    @Override
    public Map<Item, byte[]> get(List<Item> id) {
	List<String> keys = new ArrayList<String>(id.size());
	for (Item i : id) {
	    keys.add(getKey(i));
	}
	Map<Item, byte[]> result = new HashMap<Item, byte[]>();
	Map<String, Object> ret = client.getMulti(keys.toArray(new String[keys
		.size()]));
	for (Entry<String, Object> entry : ret.entrySet()) {
	    result.put(getKey(entry.getKey()), (byte[]) (entry.getValue()));
	}
	return result;
    }

    @Override
    protected void finalize() throws Throwable {
	close();
    }

    @Override
    public Map<SocketAddress, Map<String, String>> getStats() {
	Map<String, Map<String, String>> map = client.stats();
	Map<SocketAddress, Map<String, String>> result = new HashMap<SocketAddress, Map<String, String>>();
	for (Entry<String, Map<String, String>> entry : map.entrySet()) {
	    String[] address = entry.getKey().split(":");
	    result.put(new InetSocketAddress(address[0], Integer
		    .parseInt(address[1])), entry.getValue());
	}
	return result;
    }
}
