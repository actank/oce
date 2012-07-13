package com.xiaonei.xce.usercache;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.usercache.impl.DCUserCacheAdapter;
import com.xiaonei.xce.usercache.impl.IceUserCacheAdapter;

public final class UserCacheAdapter<T extends UserCache> {
	private UserCacheInvoker<T> _invoker;
	
	public UserCacheAdapter(UserCacheFactory<T> factory) {
		String flag=System.getProperty("UserCacheInvoker","DC");
		if(flag.equals("DC")) {
			_invoker = new DCUserCacheAdapter<T>(factory);
		}else {
			_invoker = new IceUserCacheAdapter<T>(factory);
		}
	}

	/*
	 *userIds为用户id， types为需要取得vip类型，在VipMemberInfo中的TYPE_VIP,DISNEY,GIFT,WIRELESS
	 *return value: T中有各个成员为vipIconInfoList, 存储要取的vip的各种类型信息
	 */
	public Map<Integer, T> getUserCacheMapWithVipIconInfo(List<Integer> userIds, List<Integer> types) {
		return _invoker.getUserCacheMap(userIds, types);
	}
	
	public Map<Integer, T> getUserCacheMap(List<Integer> userIds) {
		return _invoker.getUserCacheMap(userIds);
	}

	@Deprecated
	public List<T> getUserCacheList(List<Integer> userIds) {
		return _invoker.getUserCacheList(userIds);
	}

	public void load(int id) {
		_invoker.load(id);
	}

	public static void main(String[] args) {
		System.setProperty("UserCacheInvoker", "ICE");
		try {
			UserCacheAdapter<DefaultUserCache> adapter = new UserCacheAdapter<DefaultUserCache>(
					new DefaultUserCacheFactory());
			List<Integer> ids=new ArrayList<Integer>();
			ids.add(68126);
			Map<Integer, DefaultUserCache> map=adapter.getUserCacheMap(ids);
			for(Entry<Integer,DefaultUserCache> entry:map.entrySet()){
				System.out.println(entry.getKey()+" "+entry.getValue().getName());
				System.out.println("gender: " + entry.getValue().getGender());
			}
		} catch (Exception e) {
			Oce.getLogger().error(e);
		}
		System.exit(0);
	}
}
