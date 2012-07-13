package com.renren.tripod.client;

import java.util.List;
import java.util.Map;

/**
 * TripodClient的接口。
 * 
 * 抽象这个接口的目的是方便替换实现，包括使用mock的实现来编写测试用例。
 * 
 * @author Li Weibo (weibo.leo@gmail.com) //I believe spring-brother
 * @since 2012-1-4 下午04:10:40
 */
public interface ITripodClient {

    public Map<String, byte[]> get(List<String> keys, List<String> missedKeys);

    public Map<String, byte[]> getMissed(List<String> keys, int expiredTime, boolean useMaster);

    public Map<String, byte[]> getMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout);

    public Map<String, byte[]> getWithMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout);

    public boolean deleteCache(List<String> keys);

    public boolean deleteCache(List<String> keys, int timeout);

}
