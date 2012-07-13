package com.renren.tripod.node;

import com.renren.tripod.TripodSchema;
import com.renren.tripod.node.Nodeable;
import com.renren.tripod.node.impl.memcached.MemcachedNode;
import com.renren.tripod.util.TripodException;

public class NodeFactory {

    private static org.apache.commons.logging.Log logger_ = org.apache.commons.logging.LogFactory
            .getLog(NodeFactory.class);

    public static Nodeable create(TripodSchema.NodeCategoryEnum category,
            TripodSchema.NodeTypeEnum type, String name, String config,
            TripodSchema.NodeStatusEnum status) {
        switch (category) {
            case READ:
                if (TripodSchema.NodeTypeEnum.MEMCACHED == type) {
                    MemcachedNode m = new MemcachedNode();
                    try {
                        m.initialize(name, config, category, type, status);
                    } catch (TripodException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                    return m;
                }
                break;
            case WRITE:
                break;
            default:

        }
        return null;

    }
}
