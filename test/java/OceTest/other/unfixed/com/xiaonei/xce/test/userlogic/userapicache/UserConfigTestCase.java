package com.xiaonei.xce.test.userlogic.userapicache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import junit.framework.Assert;

import mop.hi.oce.adapter.impl.UserConfigInfoAdapterImpl;
import mop.hi.oce.domain.user.UserConfigInfo;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.clusterstate.ClusterStateSubscriber;
import xce.serverstate.ServerStateInfo;
import xce.userapicache.UserApiCacheData;
import xce.userapicache.UserApiCacheManagerPrx;
import xce.userapicache.UserApiCacheManagerPrxHelper;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userapicache.UserApiCache;
import com.xiaonei.xce.userapicache.UserApiCacheAdapter;

@RunWith (Parameterized.class)
public class UserConfigTestCase {
	private int _id;
	private int _statusconfig;
	private int _basicconfig;
	private String _profileprivacy;
	
	public UserConfigTestCase (int id, int statusconfig, int basicconfig, String profileprivacy){
		_id = id;
		_statusconfig = statusconfig;
		_basicconfig = basicconfig;
		_profileprivacy = profileprivacy;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,2,3,"profileprivacy1"},
				});
	}
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		
	}
	
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_passport(id,account,domain,status) values ("+_id+",'account"+_id+"','domain"+_id+"', 0)";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_basic(id,status) values ("+_id+",0)";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_state(id) values ("+_id+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_url(id) values ("+_id+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_names(id,name) values ("+_id+",'name"+_id+"')";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_right(id) values ("+_id+")";
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	@After
	public void tearDownAfter() throws Exception {
		/*
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_passport where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_basic where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_config where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_url where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_names where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_right where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}*/
	}
	
	@Test
	public void test() {	
		UserConfigInfoAdapterImpl uca = new UserConfigInfoAdapterImpl();
		
		MyUserConfigInfo muconf1=new MyUserConfigInfo();
		muconf1.setStatusConfig(_statusconfig);
		muconf1.setBasicConfig(_basicconfig);
		muconf1.setProfilePrivacy(_profileprivacy);
		uca.setUserConfig(_id, muconf1);
		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		
		checkO();
		
	}
	
	private List<UserApiCacheManagerPrx> getProxies(int userid){
		List<UserApiCacheManagerPrx> managers=new ArrayList<UserApiCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserApiCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserApiCacheManagerPrxHelper.uncheckedCast(ssi.prx));
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserApiCacheManagerPrxHelper.uncheckedCast(ssi.prx));
			}
		}
		return managers;
	}
	
	private void checkO(UserApiCacheData cache){
		Assert.assertEquals(cache.id, _id);
		Assert.assertEquals(cache.statusconfig, _statusconfig);
		Assert.assertEquals(cache.basicconfig, _basicconfig);
		Assert.assertEquals(cache.profileprivacy, _profileprivacy);
	}
	
	private void checkO(UserApiCache cache){
		Assert.assertEquals(cache.id, _id);
		Assert.assertEquals(cache.statusconfig, _statusconfig);
		Assert.assertEquals(cache.basicconfig, _basicconfig);
		Assert.assertEquals(cache.profileprivacy, _profileprivacy);
	}
	
	private void checkO(){
		
		List<UserApiCacheManagerPrx> managers=getProxies(_id);
		for(UserApiCacheManagerPrx ucmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer, UserApiCacheData> map=ucmp.getUserApiCacheList(ids);
			for(Entry<Integer,UserApiCacheData> entry:map.entrySet()){
				checkO(entry.getValue());
			}
		}
		
		UserApiCacheAdapter adapter=UserApiCacheAdapter.getInstance();
		UserApiCache data = adapter.getUserApiCache(_id);
		checkO(data);
		
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer, UserApiCache> map=adapter.getUserApiCacheList(ids);
		for(Entry<Integer,UserApiCache> entry:map.entrySet()){
			checkO(entry.getValue());
		}
	}
	
	private static class MyUserConfigInfo extends UserConfigInfo{
		
	}
}
