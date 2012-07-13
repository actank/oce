package com.renren.tripod.client.impl;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.google.protobuf.GeneratedMessage;
import com.renren.tripod.client.ITripodClient;
import com.renren.tripod.client.TripodService;


/**
 * @author Li Weibo (weibo.leo@gmail.com) //I believe spring-brother
 * @since 2012-1-5 下午04:49:34
 * @param <T>
 */
public class TripodServiceImpl<T> implements TripodService<T> {

    protected final Log logger = LogFactory.getLog(this.getClass());
    
    private Class<T> dataClass;
    
    private Method parseFromBytesMethod;
    
    private ITripodClient tripodClient;
    
    public void setDataClass(Class<T> dataClass) {
        if (!isGeneratedMessage(dataClass)) {
            throw new IllegalArgumentException(dataClass.getName() + " must be subclass of " + GeneratedMessage.class.getName());
        }
        this.dataClass = dataClass;
        try {
            parseFromBytesMethod = dataClass.getMethod("parseFrom", byte[].class);
        } catch (Exception e) {
            throw new IllegalArgumentException(e);
        } 
    }

    private boolean isGeneratedMessage(Class<?> klass) {
        Class<?> superClass;
        while ((superClass = klass.getSuperclass()) != null) {
            if (superClass.equals(GeneratedMessage.class)) {
                return true;
            }
            klass = superClass;
        }
        return false;
    }
    
    public void setTripodClient(ITripodClient tripodClient) {
        this.tripodClient = tripodClient;
    }

    @Override
    public Map<String, T> get(List<String> keys, List<String> missedKeys) {
        return parseMap(tripodClient.get(keys, missedKeys));
    }

    @Override
    public Map<String, T> getMissed(List<String> keys, int expiredTime, boolean useMaster) {
        return parseMap(tripodClient.getMissed(keys, expiredTime, useMaster));
    }

    @Override
    public Map<String, T> getMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout) {
        return parseMap(tripodClient.getMissed(keys, expiredTime, useMaster, timeout));
    }

    @Override
    public Map<String, T> getWithMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout) {
        return parseMap(tripodClient.getWithMissed(keys, expiredTime, useMaster, timeout));
    }

    @Override
    public boolean deleteCache(List<String> keys) {
        return tripodClient.deleteCache(keys);
    }

    @Override
    public boolean deleteCache(List<String> keys, int timeout) {
        return tripodClient.deleteCache(keys, timeout);
    }
    
    private Map<String, T> parseMap(Map<String, byte[]> rawMap) {
        Map<String, T> ret = new HashMap<String, T>(rawMap.size());
        for (Entry<String, byte[]> entry : rawMap.entrySet()) {
            T value = parseFromBytes(entry.getValue());
            ret.put(entry.getKey(), value);
        }
        return ret;
    }
    
    @SuppressWarnings("unchecked")
    private T parseFromBytes(byte[] bytes) {
        try {
            return (T)parseFromBytesMethod.invoke(dataClass, new Object[]{bytes});
        } catch (Exception e) {
            throw new RuntimeException("Error while parsing object from byte[]");
        }
    }

}
