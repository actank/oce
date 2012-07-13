package com.renren.tripod.node;

import java.util.List;
import java.util.Map;

import com.renren.tripod.TripodSchema;
import com.renren.tripod.util.TripodException;

/**
 * @author yuyang
 * 
 */
public interface Nodeable {

    //    public enum Type {
    //        Memcached, ICE;
    //    }
    //
    //    public enum Status {
    //        Offline, Upgrading, Online
    //    }
    //
    //    public static class Item {
    //
    //        public Item(int block, long id) {
    //            this.block = block;
    //            this.id = id;
    //        }
    //
    //        public final int block;
    //
    //        public final long id;
    //
    //        @Override
    //        public String toString() {
    //            return "Node.Item[" + block + "/" + id + "]";
    //        }
    //    }

    /**
     * @param name
     * @param config
     * @param category
     * @param type
     * @param status
     * @throws TripodException
     */
    public void initialize(String name, String config, TripodSchema.NodeCategoryEnum category,
            TripodSchema.NodeTypeEnum type, TripodSchema.NodeStatusEnum status)
            throws TripodException;

    /**
     * @return
     */
    public String getConfig();

    /**
     * @return
     */
    public String getName();

    /**
     * @return
     */
    public TripodSchema.NodeCategoryEnum getCategory();

    /**
     * @return
     */
    public TripodSchema.NodeStatusEnum getStatus();

    /**
     * @return
     */
    public TripodSchema.NodeTypeEnum getType();

    /**
     * 
     */
    public void close();

    
    /**
     * 读取数据接口
     * 
     * @param key
     * @return
     * @throws TripodException
     */
    public byte[] get(String key) throws TripodException;

    
    /**
     * 批量读取数据接口
     * 
     * @param keys
     * @return
     * @throws TripodException
     */
    public Map<String, byte[]> get(List<String> keys) throws TripodException;

    
    /**
     * 设置数据接口
     * 
     * @param key
     * @param data
     * @param sync
     * @throws TripodException
     */
    public void set(String key, byte[] data, boolean sync) throws TripodException;

    
    /**
     * 批量设置数据接口
     * 
     * @param data
     * @param sync
     * @throws TripodException
     */
    public void set(Map<String, byte[]> data, boolean sync) throws TripodException;

}
