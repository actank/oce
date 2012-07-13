package com.renren.tripod.node.impl;

import java.util.List;
import java.util.Map;

import com.renren.tripod.TripodSchema;
import com.renren.tripod.TripodSchema.NodeCategoryEnum;
import com.renren.tripod.TripodSchema.NodeStatusEnum;
import com.renren.tripod.TripodSchema.NodeTypeEnum;
import com.renren.tripod.node.Nodeable;
import com.renren.tripod.util.TripodException;


/**
 * @author yuyang
 *
 */
public class Node implements Nodeable{

    protected TripodSchema.NodeCategoryEnum category_;
    protected TripodSchema.NodeTypeEnum type_;
    protected TripodSchema.NodeStatusEnum status_;
    protected String name_;
    protected String config_;
    
    @Override
    public void close() {
        // TODO Auto-generated method stub
        
    }

    @Override
    public String getConfig() {
        // TODO Auto-generated method stub
        return config_;
    }

    @Override
    public String getName() {
        // TODO Auto-generated method stub
        return name_;
    }

    @Override
    public NodeCategoryEnum getCategory(){
        return category_;      
    }
    
    @Override
    public NodeStatusEnum getStatus() {
        // TODO Auto-generated method stub
        return status_;
    }

    @Override
    public NodeTypeEnum getType() {
        // TODO Auto-generated method stub
        return type_;
    }

    @Override
    public void initialize(String name, String config, NodeCategoryEnum category,
            NodeTypeEnum type, NodeStatusEnum status) throws TripodException {
        // TODO Auto-generated method stub
        name_ = name;
        config_ = config;
        category_ = category;
        type_ = type;
        status_ = status;
    }

    /**
     * @param id
     * @return
     */
    public byte[] get(String key) throws TripodException{
        throw new TripodException("Node doesn't support get!!");
    }

    /**
     * @param id
     * @return
     */
    public Map<String, byte[]> get(List<String> keys) throws TripodException{
        throw new TripodException("Node doesn't support get batch!!");
    }

    /**
     * @param id
     * @param data
     * @throws TripodException
     */
    public void set(String key, byte[] data, boolean sync) throws TripodException {
        throw new TripodException("Node doesn't support set!!");
    }

    /**
     * @param data
     */
    public void set(Map<String, byte[]> data, boolean sync) throws TripodException {
        throw new TripodException("Node doesn't support get batch!!");
    }
}
