package com.xiaonei.xce.userapicache;

import java.util.List;
import java.util.Map;

public interface UserApiCacheInvoker {
	public Map<java.lang.Integer, UserApiCache> getUserApiCacheList(List<Integer> userIds);
}
