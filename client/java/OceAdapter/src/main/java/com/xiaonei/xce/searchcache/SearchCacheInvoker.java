package com.xiaonei.xce.searchcache;

import java.util.List;
import java.util.Map;

public interface SearchCacheInvoker {
	public Map<Integer, byte[]> getSearchCacheMap2(List<Integer> userIds);
}
