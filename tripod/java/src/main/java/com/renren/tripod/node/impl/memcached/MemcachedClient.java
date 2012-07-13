package com.renren.tripod.node.impl.memcached;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.danga.MemCached.MemCachedClient;
import com.danga.MemCached.SockIOPool;
import com.renren.tripod.TripodSchema.NodeCategoryEnum;
import com.renren.tripod.TripodSchema.NodeStatusEnum;
import com.renren.tripod.TripodSchema.NodeTypeEnum;
import com.renren.tripod.node.impl.Node;
import com.renren.tripod.util.Constant;
import com.renren.tripod.util.TripodException;

/**
 * @author yuyang
 * 
 */
public class MemcachedClient extends Node {

    private static org.apache.commons.logging.Log logger = org.apache.commons.logging.LogFactory
            .getLog(MemcachedClient.class);

    private MemCachedClient client_;

    @Override
    public void initialize(String name, String config, NodeCategoryEnum category,
            NodeTypeEnum type, NodeStatusEnum status) throws TripodException {
        super.initialize(name, config, category, type, status);
        /*
         * The config string format: <host>:<port>:<key_prefix>
         */
        String[] part = config.split(":");
        if (part.length != 3) {
            throw new TripodException("The config string" + config
                    + " not match the format: <host>:<port>:<key_prefix>");
        }
        try {
            String[] address = new String[] { part[0] + ":" + part[1] };
            SockIOPool pool = SockIOPool.getInstance(part[0] + ":" + part[1]);
            if (!pool.isInitialized()) {
                pool.setServers(address);
                pool.setInitConn(Constant.MEMCACHE_INIT_CONNECTION);
                pool.setMinConn(Constant.MEMCACHE_MIN_CONNECTION);
                pool.setMaxConn(Constant.MEMCACHE_MAX_CONNECTION);
                pool.setMaxIdle(Constant.MEMCACHE_MAX_IDLE);
                pool.setNagle(Constant.MEMCACHE_USE_NAGLE);
                pool.setSocketTO(Constant.MEMCACHE_SOCKET_TIMEOUT);
                pool.setMaintSleep(Constant.MEMCACHE_MAINT_SLEEP);
                pool.setSocketConnectTO(Constant.MEMCACHE_SOCKET_CONNECTION_TIMEOUT);
                pool.initialize();
            }

            client_ = new MemCachedClient(part[0] + ":" + part[1]);

        } catch (Throwable e) {
            e.printStackTrace();
            throw new TripodException(
                    " [MemcachedNode.initialize] Error while construct MemcachedClient, see stderr for stack detail. ",
                    e);
        }
    }

    public byte[] get(String key) throws TripodException {
        if (logger.isDebugEnabled()) {
            logger.debug(" [MemcachedNode.get] " + key );
        }
        return (byte[]) client_.get(key);
    }

    public Map<String, byte[]> get(List<String> keys) throws TripodException {

        Map<String, byte[]> result = new HashMap<String, byte[]>();
        Map<String, Object> ret = client_.getMulti(keys.toArray(new String[keys.size()]));
        for (Entry<String, Object> entry : ret.entrySet()) {
            result.put(entry.getKey(), (byte[]) (entry.getValue()));
        }
        return result;
    }

    public void set(String key, byte[] data, boolean sync) throws TripodException {

        if (logger.isDebugEnabled()) {
            logger.debug(" [MemcachedNode.set] " + name_ + ":" + config_ + " set " + key
                    + " data size: " + data.length);
        }
        if (data == null) throw new TripodException(" [MemcachedNode.set] set error, data is null.");

        if (!client_.set(key, data)) {
            throw new TripodException(" [MemcachedNode.set] set error, " + name_ + ":" + config_
                    + " set " + key + " data size: " + data.length + " failed.");
        }
    }

    public void set(Map<String, byte[]> data, boolean sync) throws TripodException {
        throw new TripodException("MemcachedNode doesn't support set batch!!");
    }
}
