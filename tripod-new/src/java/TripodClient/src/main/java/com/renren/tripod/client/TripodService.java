package com.renren.tripod.client;

import java.util.List;
import java.util.Map;


/**
 * {@link TripodService}是对{@link ITripodClient}的一种简化封装，它的接口
 * 与{@link ITripodClient}的接口很类似，只不过是将{@link ITripodClient}中
 * 的方法的返回值中<code>byte[]</code>类型替换为泛型。而此接口的实现中应当自动
 * 完成从<code>byte[]</code>到泛型类型的转化。
 * 
 * @author Li Weibo (weibo.leo@gmail.com) //I believe spring-brother
 * @since 2012-1-4 下午05:46:38
 * @param <T> 当前{@link TripodService}所操作的数据类型
 */
public interface TripodService<T> {

    public Map<String, T> get(List<String> keys, List<String> missedKeys);
    
    public Map<String, T> getMissed(List<String> keys, int expiredTime, boolean useMaster);

    public Map<String, T> getMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout);

    public Map<String, T> getWithMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout);

    public boolean deleteCache(List<String> keys);

    public boolean deleteCache(List<String> keys, int timeout);
    
}
