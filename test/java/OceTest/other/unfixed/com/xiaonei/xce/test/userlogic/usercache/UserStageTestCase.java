package com.xiaonei.xce.test.userlogic.usercache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import junit.framework.Assert;

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
import xce.usercache.UserCacheLoaderPrx;
import xce.usercache.UserCacheLoaderPrxHelper;
import xce.usercache.UserCacheManagerPrx;
import xce.usercache.UserCacheManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.usercache.UserCache;
import com.xiaonei.xce.usercache.UserCacheAdapter;
import com.xiaonei.xce.usercache.UserCacheFactory;
import com.xiaonei.xce.userstage.UserStageAdapter;
import com.xiaonei.xce.userstage.UserStageFactory;
import com.xiaonei.xce.userstage.UserStageInfo;
@RunWith (Parameterized.class)
public class UserStageTestCase {
	private int _id;
	private int _univ;
	public UserStageTestCase(int id,int stage, int univ){
		_id=id;
		_univ=univ;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,2,1},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_state");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_state(id,level,state) values ("+_id+",-1,0)";
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
					String sql="replace into user_url(id,tinyurl) values ("+_id+",'')";
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
					String sql="replace into user_names(id,name) values ("+_id+",'')";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_born(id,gender) values ("+_id+",'')";
					System.out.println(sql);
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_stage(id,stage,univ) values ("+_id+",-1,-1)";
					System.out.println(sql);
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
		reload();
	}
	@After
	public void tearDownAfter() throws Exception {
		
	}
	
	@Test
	public void test() {
		UserStageAdapter<MyUserStageInfo> uba=new UserStageAdapter<MyUserStageInfo>(new MyUserStageFactory());
		MyUserStageInfo mubis1=new MyUserStageInfo();
		mubis1.setUniv(_univ);
		uba.setUserStage(_id, mubis1);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		check();
	}
	
	private void check(MyUserCache cache){
		Assert.assertEquals(cache.getId(), _id);
		Assert.assertEquals(cache.getStatus(), 0);
		Assert.assertEquals(cache.getState(), 0);
		Assert.assertEquals(cache.getLevel(), -1);
		Assert.assertEquals(cache.getTinyUrl(), "");
		Assert.assertEquals(cache.getUniv(), _univ);
		Assert.assertEquals(cache.getName(), "");
		Assert.assertEquals(cache.getGender(), "");
	}
	
	private void check(Map<String,String> map){
		Assert.assertEquals(Integer.parseInt(map.get("id")),_id);
		Assert.assertEquals(Integer.parseInt(map.get("status")),0);
		Assert.assertEquals(Integer.parseInt(map.get("state")),0);
		Assert.assertEquals(Integer.parseInt(map.get("level")),-1);
		Assert.assertEquals(map.get("tinyurl"),"");
		Assert.assertEquals(map.get("name"),"");
		Assert.assertEquals(Integer.parseInt(map.get("univ")),_univ);
		Assert.assertEquals(map.get("gender"), "");
	}
	
	private void check(){
		List<UserCacheManagerPrx> managers=getProxies(_id);
		for(UserCacheManagerPrx ucmp:managers){
			int[] ids=new int[]{_id};
			Map<String,String>[] maps=ucmp.getUserCacheList(ids);
			for(Map<String,String> map:maps){
				check(map);
			}
		}
		UserCacheAdapter<MyUserCache> adapter=new UserCacheAdapter<MyUserCache>(new MyUserCacheFactory());
		List<Integer> ids=new ArrayList<Integer>();
		ids.add(_id);
		Map<Integer,MyUserCache> map=adapter.getUserCacheMap(ids);
		for(Entry<Integer,MyUserCache> entry:map.entrySet()){
			check(entry.getValue());
		}
	}
	
	private void reload(){
		ObjectPrx op=Channel.newCoreChannel("core").getCommunicator().stringToProxy("M@UserCacheLoader");
		UserCacheLoaderPrx uclp=UserCacheLoaderPrxHelper.uncheckedCast(op);
		int[] ids=new int[]{_id};
		uclp.load(ids);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	private List<UserCacheManagerPrx> getProxies(int userid){
		List<UserCacheManagerPrx> managers=new ArrayList<UserCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("new");
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("old");
			}
		}
		return managers;
	}
	private static class MyUserStageInfo extends UserStageInfo{
		private static final long serialVersionUID = 1L;
		
	}
	
	private static class MyUserStageFactory implements UserStageFactory<MyUserStageInfo>{
		public MyUserStageInfo create() {
			return new MyUserStageInfo();
		}		
	}
	private static class MyUserCache extends UserCache{
		
	}
	
	private static class MyUserCacheFactory implements UserCacheFactory<MyUserCache>{

		public MyUserCache createUserCache() {
			return new MyUserCache();
		}
		
	}
}
