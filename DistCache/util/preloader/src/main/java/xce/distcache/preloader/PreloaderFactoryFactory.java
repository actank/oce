package xce.distcache.preloader;

import xce.distcache.preloader.impl.BuddyApplyCachePreloaderFactory;
import xce.distcache.preloader.impl.DistSearchCachePreloaderFactory;
import xce.distcache.preloader.impl.UserCachePreloaderFactory;

public class PreloaderFactoryFactory {
	
	
	public static PreloaderFactory create(String name) {
		if (name.equals("UserCache")) {
			return new UserCachePreloaderFactory();
		} else if (name.equals("DistSearchCache")) {
			return new DistSearchCachePreloaderFactory();
		} else if (name.equals("BuddyApplyCache")){
			return new BuddyApplyCachePreloaderFactory();
		} else {
			return null;
		}
	}
}