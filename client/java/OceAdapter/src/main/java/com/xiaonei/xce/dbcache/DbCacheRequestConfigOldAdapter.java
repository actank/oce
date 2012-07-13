package com.xiaonei.xce.dbcache;

import java.util.Vector;
import java.util.concurrent.atomic.AtomicBoolean;

import xce.generaldbcache.DbFields;
import xce.generaldbcache.DbRow;
import xce.generaldbcache.DbRows;
import xce.generaldbcache.GeneralDbCacheManagerPrx;
import xce.generaldbcache.GeneralDbCacheManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class DbCacheRequestConfigOldAdapter extends ServiceAdapter{
	private static DbCacheRequestConfigOldAdapter instance=new DbCacheRequestConfigOldAdapter();
	private static final String endpoints = "@DbCacheRequestConfig";
	private Vector<GeneralDbCacheManagerPrx> managers = new Vector<GeneralDbCacheManagerPrx>();
	private Vector<GeneralDbCacheManagerPrx> managersOneway = new Vector<GeneralDbCacheManagerPrx>();
	
	public static DbCacheRequestConfigOldAdapter getInstance(){
		return instance;
	}
	
	protected DbCacheRequestConfigOldAdapter() {
		super(endpoints, 1, Channel.newOceChannel(endpoints));
	}
	
	public GeneralDbCacheManagerPrx getGeneralDbCacheManager(int id) {
		return locateProxy(managers, "M", id, Channel.Invoke.Twoway,
				GeneralDbCacheManagerPrxHelper.class, 300);
	}

	public GeneralDbCacheManagerPrx getGeneralDbCacheManagerOneway(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Oneway,
				GeneralDbCacheManagerPrxHelper.class, 300);
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
