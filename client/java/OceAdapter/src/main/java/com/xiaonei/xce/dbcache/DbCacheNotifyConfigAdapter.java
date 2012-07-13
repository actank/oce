package com.xiaonei.xce.dbcache;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import com.xiaonei.xce.log.Oce;

import xce.generaldbcache.DbFields;
import xce.generaldbcache.DbRow;
import xce.generaldbcache.DbRows;
import xce.generaldbcache.GeneralDbCacheManagerPrx;
import xce.generaldbcache.GeneralDbCacheManagerPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;

public final class DbCacheNotifyConfigAdapter extends ReplicatedClusterAdapter {
	
	private static final String ctr_endpoints = "ControllerGeneralDbCacheNotify";
	private static int _interval = 120;	

	
	private static DbCacheNotifyConfigAdapter instance = new DbCacheNotifyConfigAdapter();

	public static DbCacheNotifyConfigAdapter getInstance() {
		
		return instance;
	}

	private DbCacheNotifyConfigAdapter() {
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
	
	public List<Integer> getAppIds(int owner){
		List<Integer> list=new ArrayList<Integer>();
		if(emptyTable.get()){
			return list;
		}
		if(appIdPos == -1){
			generateAppIdPos(0);
		}
		DbRows res=getGeneralDbCacheManager(owner).getDbRows(owner);
		for(DbRow row:res.rows){
			try{
				list.add(Integer.parseInt(row.values[appIdPos]));
			}catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getAppIds " + owner + " Ice.TimeoutException");
			}catch(NumberFormatException e){
				Oce.getLogger().error(this.getClass().getName() + ".getAppIds " + owner + " NumberFormatException");
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() + ".getAppIds " + owner, e);
			}
		}
		return list;
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
