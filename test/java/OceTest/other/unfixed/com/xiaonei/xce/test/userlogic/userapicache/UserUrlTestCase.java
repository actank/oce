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
import com.xiaonei.xce.userurl.UserUrlAdapter;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;

@RunWith (Parameterized.class)
public class UserUrlTestCase {
	private int _id;
	private String _mainurl;
	private String _headurl;
	private String _tinyurl;
	
	public UserUrlTestCase (int id, String mainurl, String headurl,String tinyurl){
		_id = id;
		_mainurl = mainurl;
		_headurl = headurl;
		_tinyurl = tinyurl;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"mainhead8","headurl9","tinyurl7"},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport_new");
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic_new");
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url_new");
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names_new");
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right_new");
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
		UserUrlAdapter<MyUserUrlInfo> uua=new UserUrlAdapter<MyUserUrlInfo>(new MyUserUrlFactory());
		
		MyUserUrlInfo muuiws1=new MyUserUrlInfo();
		muuiws1.setMainUrl(_mainurl);
		muuiws1.setHeadUrl(_headurl);
		muuiws1.setTinyUrl(_tinyurl);
		uua.setUserUrl(_id, muuiws1);	
		
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
		Assert.assertEquals(cache.mainurl, _mainurl);
		System.out.println("mainurl: " + cache.mainurl);
		Assert.assertEquals(cache.headurl, _headurl);
		System.out.println("headurl: " + cache.headurl);
		Assert.assertEquals(cache.tinyurl, _tinyurl);
	}
	
	private void checkO(UserApiCache cache){
		Assert.assertEquals(cache.id, _id);
		Assert.assertEquals(cache.mainurl, _mainurl);
		Assert.assertEquals(cache.headurl, _headurl);
		Assert.assertEquals(cache.tinyurl, _tinyurl);
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
	
	private static class MyUserUrlInfo extends UserUrlInfo{
	}
	
	private static class MyUserUrlFactory implements UserUrlFactory<MyUserUrlInfo>{
		public MyUserUrlInfo create() {
			return new MyUserUrlInfo();
		}
	}
}
