package com.xiaonei.xce.usercache;

public class DefaultUserCacheFactory implements UserCacheFactory<DefaultUserCache> {

	@Override
	public DefaultUserCache createUserCache() {
		return new DefaultUserCache();
	}
	
}