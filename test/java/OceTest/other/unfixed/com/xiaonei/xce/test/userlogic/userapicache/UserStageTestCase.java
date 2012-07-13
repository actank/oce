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
import com.xiaonei.xce.userstage.UserStageAdapter;
import com.xiaonei.xce.userstage.UserStageFactory;
import com.xiaonei.xce.userstage.UserStageInfo;


@RunWith (Parameterized.class)
public class UserStageTestCase {
	private int _id;
	private int _stage;
	private int _univ;
	
	public UserStageTestCase (int id, int stage, int univ){
		_id = id;
		_stage = stage;
		_univ = univ;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,1,2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_born");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_born(id) values ("+_id+")";
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
					String sql="replace into user_stage(id) values ("+_id+")";
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
	
	}
	
	@Test
	public void test() {
		UserStageAdapter<MyUserStageInfo> uba=new UserStageAdapter<MyUserStageInfo>(new MyUserStageFactory());
		
		MyUserStageInfo mubis1=new MyUserStageInfo();
		mubis1.setStage(_stage);
		mubis1.setUniv(_univ);
		uba.setUserStage(_id, mubis1);
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
		Assert.assertEquals(cache.stage, _stage);
		Assert.assertEquals(cache.univ, _univ);
	}
	
	private void checkO(UserApiCache cache){
		Assert.assertEquals(cache.id, _id);
		Assert.assertEquals(cache.stage, _stage);
		Assert.assertEquals(cache.univ, _univ);
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
	
	private static class MyUserStageInfo extends UserStageInfo{
		private static final long serialVersionUID = 1L;
		
	}
	
	private static class MyUserStageFactory implements UserStageFactory<MyUserStageInfo>{
		public MyUserStageInfo create() {
			return new MyUserStageInfo();
		}		
	}
	
	
}
