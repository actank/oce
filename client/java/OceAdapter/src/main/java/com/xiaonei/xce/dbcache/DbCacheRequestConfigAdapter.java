package com.xiaonei.xce.dbcache;

import java.util.concurrent.atomic.AtomicBoolean;

import xce.generaldbcache.DbFields;
import xce.generaldbcache.DbRow;
import xce.generaldbcache.DbRows;
import xce.generaldbcache.GeneralDbCacheManagerPrx;
import xce.generaldbcache.GeneralDbCacheManagerPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;

public class DbCacheRequestConfigAdapter extends ReplicatedClusterAdapter{
	
	private static final String ctr_endpoints = "ControllerGeneralDbCacheRequest";
	private static int _interval = 120;	
	
	private static DbCacheRequestConfigAdapter instance=new DbCacheRequestConfigAdapter();

	public static DbCacheRequestConfigAdapter getInstance(){
		return instance;
	}
	
	protected DbCacheRequestConfigAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	public GeneralDbCacheManagerPrx getGeneralDbCacheManager(int id) {
		Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if ( prx0 != null ) {
			GeneralDbCacheManagerPrx prx = GeneralDbCacheManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public GeneralDbCacheManagerPrx getGeneralDbCacheManagerOneway(int id) {
		Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if ( prx0 != null ) {
			GeneralDbCacheManagerPrx prx = GeneralDbCacheManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	private volatile int appIdPos = -1;
	private AtomicBoolean emptyTable = new AtomicBoolean(false);
	
	public synchronized void generateAppIdPos(int num) {
		if (emptyTable.get()) {
			return;
		}
		if (appIdPos != -1) {
			return;
		}
		DbFields res = getGeneralDbCacheManager(num).getDbFields();
		if (res == null) {
			emptyTable.set(true);
			return;
		}
		String[] fields = res.fields;
		for (int i = 0; i < fields.length; ++i) {
			if (fields[i].equals("app_id")) {
				appIdPos = i;
				break;
			}
		}
	}

	public boolean has(int owner, int app_id) {
		if (emptyTable.get()) {
			return false;
		}
		if (appIdPos == -1) {
			generateAppIdPos(0);
		}
		DbRows res = getGeneralDbCacheManager(owner).getDbRows(owner);
		for (DbRow row : res.rows) {
			if (row.values[appIdPos].equals(String.valueOf(app_id))) {
				return true;
			}
		}
		return false;
	}

	public void reload(int id) {
		emptyTable.set(false);
		getGeneralDbCacheManager(id).load(id);
	}
}
