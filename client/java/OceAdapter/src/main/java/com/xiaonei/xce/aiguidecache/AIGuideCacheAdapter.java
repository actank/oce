package com.xiaonei.xce.aiguidecache;

import java.util.Vector;
import xce.aiguidecache.AIGuideCacheManagerPrx;
import xce.aiguidecache.AIGuideCacheManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import xce.aiguidecache.FARMGAME;
import xce.aiguidecache.DOG;
import xce.aiguidecache.CARPARK;
import xce.aiguidecache.GODFATHER;
import xce.aiguidecache.TYCOON;

public final class AIGuideCacheAdapter extends ServiceAdapter {

	private static AIGuideCacheAdapter _adapter = new AIGuideCacheAdapter();	
	private static final String endpoints = "@AIGuideCache";
	
	public enum AppType { 
		APP_FARMGAME(FARMGAME.value),
		APP_DOG(DOG.value),
		APP_CARPARK(CARPARK.value),
		APP_GODFATHER(GODFATHER.value),
		APP_TYCOON(TYCOON.value);		
		
		int value;
		AppType(int value){
			this.value = value;
		}
	};

	private Vector<AIGuideCacheManagerPrx> managers = new Vector<AIGuideCacheManagerPrx>();
	private Vector<AIGuideCacheManagerPrx> managersOneway = new Vector<AIGuideCacheManagerPrx>();

	protected AIGuideCacheAdapter() {
		super(endpoints, 0, Channel.newActiveTrackChannel(endpoints));
	}

	public static AIGuideCacheAdapter getInstance() {
		return _adapter;
	}

	protected AIGuideCacheManagerPrx getAIGuideCacheManager(int userId) {
		return AIGuideCacheManagerPrxHelper.uncheckedCast(locateProxy(managers,
				"M", userId, Channel.Invoke.Twoway,
				AIGuideCacheManagerPrxHelper.class, 300));
	}

	protected AIGuideCacheManagerPrx getAIGuideCacheManagerOneway(int userId) {
		return AIGuideCacheManagerPrxHelper.uncheckedCast(locateProxy(
				managersOneway, "M", userId, Channel.Invoke.Oneway,
				AIGuideCacheManagerPrxHelper.class, 300));
	}

	public void set(int userId, AppType appType, boolean status) {
		getAIGuideCacheManagerOneway(userId).set(userId, appType.value, status);
	}

	public boolean get(int userId, AppType appType) {
		return getAIGuideCacheManager(userId).get(userId, appType.value);
	}
	
	public boolean getAndClear(int userId, AppType appType) {
		boolean result = getAIGuideCacheManager(userId).get(userId, appType.value);
		if ( result ) {
			set(userId, appType, false);
		}
		return result;		
	}	
}
