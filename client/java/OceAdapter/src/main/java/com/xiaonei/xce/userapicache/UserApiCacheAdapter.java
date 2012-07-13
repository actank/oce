package com.xiaonei.xce.userapicache;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import xce.userapicache.UserApiCacheData1;
import com.xiaonei.xce.log.TimeCost;

public final class UserApiCacheAdapter {

	private UserApiCacheInvoker _invoker;
	private static  UserApiCacheAdapter _instance=new UserApiCacheAdapter();
	
	private UserApiCacheAdapter(){
	    String flag=System.getProperty("UserApiCacheInvoker","DC");
	    if(flag.equals("DC")) {
	    	_invoker = new DCUserApiCacheDAdapter();
	    } else {
	    	_invoker = new IceUserApiCacheAdapter();	
	    }
	}
	
	public static UserApiCacheAdapter getInstance(){
		return _instance;
	}
	
	public UserApiCache getUserApiCache(int userid){
		TimeCost cost = TimeCost.begin("UserApiCacheAdapter.getUserApiCache id: "+userid);
		try {
			if (userid <= 0) {
			    UserApiCache uac = new UserApiCache();
			    uac.setBase(new UserApiCacheData1());
			    return uac;
			}
			List<Integer> ids=new ArrayList<Integer>();
			ids.add(userid);
			return getUserApiCacheList(ids).get(userid);
		} finally {
			cost.endFinally();
		}
	}
	
	public Map<java.lang.Integer, UserApiCache> getUserApiCacheList(List<Integer> userids){
		return _invoker.getUserApiCacheList(userids);
	}
	
	public static void main(String[] args){
		UserApiCache cache=UserApiCacheAdapter.getInstance().getUserApiCache(68126);
		System.out.println(cache.getLevel());
		System.exit(0);
		/*try{
			int userId = 68126;
			Ice.ObjectPrx prx0 = _csClient.getValidProxy(userId);
			if ( prx0 == null){
				System.out.println("Prx is null");
			}
			else{
				UserApiCacheManagerPrx prx = getUserApiCacheManager(userId);
				System.out.println(": " + prx.toString());
				UserApiCacheData UData = prx.getUserApiCache(userId);
				System.out.println(": id: " + UData.id + " name: " + UData.name);
				System.out.println(" auth: " + UData.auth + "  basicconfig: " + UData.basicconfig);
				System.out.println("birthday: " + UData.birthday + "  birthmonth: " + UData.birthmonth);
				System.out.println("headurl: " + UData.headurl + "  homecity: " + UData.homecity);
				System.out.println("homeprovince: " + UData.homeprovince + "  mainurl: " + UData.mainurl);
				System.out.println("sex: " + UData.sex + "  statusconfig: " + UData.statusconfig);
				System.out.println("status: " + UData.status + "  univ: " + UData.univ);
				System.out.println("profileprivacy: "+prx.getUserApiCache(userId).profileprivacy);
			}
		}catch(Exception e){
			Oce.getLogger().error(this.getClass().getName() + " " + e);
		}
		System.exit(0);*/
	}

}
