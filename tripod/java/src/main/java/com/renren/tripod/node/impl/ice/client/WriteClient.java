package com.renren.tripod.node.impl.ice.client;

import java.util.Map;
import java.util.Map.Entry;

import com.renren.tripod.TripodData;
import com.renren.tripod.WriteServerPrx;
import com.renren.tripod.WriteServerPrxHelper;
import com.renren.tripod.TripodSchema.NodeCategoryEnum;
import com.renren.tripod.TripodSchema.NodeStatusEnum;
import com.renren.tripod.TripodSchema.NodeTypeEnum;
import com.renren.tripod.node.impl.Node;
import com.renren.tripod.util.Key;
import com.renren.tripod.util.TripodException;

/**
 * @author yuyang
 * 
 */
public class WriteClient extends Node {

    private static Ice.Communicator ic_ = Ice.Util.initialize();

    private WriteServerPrx prx_;

    @Override
    public void initialize(String name, String config, NodeCategoryEnum category,
            NodeTypeEnum type, NodeStatusEnum status) throws TripodException {
        // TODO Auto-generated method stub
        super.initialize(name, config, category, type, status);
        this.prx_ = WriteServerPrxHelper.uncheckedCast(ic_.stringToProxy(getConfig()));
    }

    @Override
    public void set(String key, byte[] data, boolean sync) throws TripodException {
        // TODO Auto-generated method stub
        Key k = new Key(key);
        TripodData[] entry = new TripodData[1];
        entry[0] = new TripodData(k.block, k.id, data);
        this.prx_.set(name_, entry, true);
    }

    @Override
    public void set(Map<String, byte[]> data, boolean sync) throws TripodException {
        // TODO Auto-generated method stub
        TripodData[] entries = new TripodData[data.size()];
        int i = 0;
        for (Entry<String, byte[]> entry : data.entrySet()) {
            Key k = new Key(entry.getKey());
            entries[i++] = new TripodData(k.block, k.id, entry.getValue());
        }
        this.prx_.set(name_, entries, true);
    }

    @Override
    public void close() {
        // TODO Auto-generated method stub
        ic_.destroy();
    }
}
