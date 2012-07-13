package com.xiaonei.xce.usercache;

import java.util.List;
import java.util.Map;

public interface UserCacheInvoker<T> {
	public Map<Integer, T> getUserCacheMap(List<Integer> userIds);
	public Map<Integer, T> getUserCacheMap(List<Integer> userIds, List<Integer> types);
	public List<T> getUserCacheList(List<Integer> userIds);
	public void load(int id);
}
