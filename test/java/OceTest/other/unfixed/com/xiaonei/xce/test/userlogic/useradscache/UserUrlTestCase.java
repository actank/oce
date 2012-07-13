package com.xiaonei.xce.test.userlogic.useradscache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runners.Parameterized.Parameters;

import xce.clusterstate.ClusterStateSubscriber;
import xce.serverstate.ServerStateInfo;
import xce.useradscache.UserAdsCacheData;
import xce.useradscache.UserAdsCacheManagerPrx;
import xce.useradscache.UserAdsCacheManagerPrxHelper;
import xce.useradscache.UserAdsCacheUrlData;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userurl.UserUrlAdapter;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;

public class UserUrlTestCase {
	private int _id;
	private String _tinyurl;
	public UserUrlTestCase(int id, String tinyurl){
		_id=id;
		_tinyurl=tinyurl;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,"tinyurl"},
			});
	}
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_basic(id,stage,status,birth_year,birth_month,birth_day,gender,home_province,home_city) values ("+_id+",0,0,0,0,0,'','','')";
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from region_info where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from network_history where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from workplace_info where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from elementary_school_info where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from junior_high_school_info where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from high_school_info where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from college_info where userid="+_id;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_network");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from university_info where userid="+_id;
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
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		
	}
	
	@After
	public void tearDownAfter() throws Exception {  

	}
	
	@Test
	public void test() {
		UserUrlAdapter<MyUserUrlInfo> uua=new UserUrlAdapter<MyUserUrlInfo>(new MyUserUrlFactory());
		MyUserUrlInfo muui=new MyUserUrlInfo();
		muui.setTinyUrl(_tinyurl);
		uua.setUserUrl(_id, muui);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		check();
	}
	
	private List<UserAdsCacheManagerPrx> getProxies(int userid){
		List<UserAdsCacheManagerPrx> managers=new ArrayList<UserAdsCacheManagerPrx>();
		ClusterStateSubscriber clusterState=ClusterStateSubscriber.getInstance("ControllerUserAdsCache",120);
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx!=null){
			for(ServerStateInfo ssi:nowPrx){
				managers.add(UserAdsCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("new");
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx!=null){
			for(ServerStateInfo ssi:oldPrx){
				managers.add(UserAdsCacheManagerPrxHelper.uncheckedCast(ssi.prx));
				System.out.println("old");
			}
		}
		return managers;
	}
	
	private void check(){
		List<UserAdsCacheManagerPrx> managers=getProxies(_id);
		for(UserAdsCacheManagerPrx uscmp:managers){
			int[] ids=new int[]{_id};
			Map<Integer,UserAdsCacheData> maps=uscmp.getUserAdsCacheList(ids);
			Assert.assertTrue(maps.containsKey(_id));
			Assert.assertEquals(ids.length, maps.size());
			check(maps.get(_id));
			Map<Integer,UserAdsCacheUrlData> urlmaps=uscmp.getUserAdsCacheUrlList(ids);
			Assert.assertTrue(urlmaps.containsKey(_id));
			Assert.assertEquals(ids.length, urlmaps.size());
			check(urlmaps.get(_id));
		}
	}
	
	private void check(UserAdsCacheUrlData data){
		Assert.assertEquals(data.id, _id);
		Assert.assertEquals(data.tinyurl, _tinyurl);
		Assert.assertEquals(0, data.birthyear);
		Assert.assertEquals(0, data.birthmonth);
		Assert.assertEquals(0, data.birthday);
		Assert.assertEquals(0, data.stage);
		Assert.assertEquals(data.sex, "");
		Assert.assertEquals("", data.homeprovince);
		Assert.assertEquals("", data.homecity);
		Assert.assertEquals(data.seatcity, "");
		Assert.assertEquals(data.seatprovince, "");
		Assert.assertEquals(0, data.networks.length);
		Assert.assertEquals(0, data.workplaces.length);
		Assert.assertEquals(0, data.schools.length);
	}
	
	private void check(UserAdsCacheData data){
		Assert.assertEquals(_id, data.id);
		Assert.assertEquals(0, data.birthyear);
		Assert.assertEquals(0, data.birthmonth);
		Assert.assertEquals(0, data.birthday);
		Assert.assertEquals(0, data.stage);
		Assert.assertEquals(data.sex, "");
		Assert.assertEquals("", data.homeprovince);
		Assert.assertEquals("", data.homecity);
		Assert.assertEquals(data.seatcity, "");
		Assert.assertEquals(data.seatprovince, "");
		Assert.assertEquals(0, data.networks.length);
		Assert.assertEquals(0, data.workplaces.length);
		Assert.assertEquals(0, data.schools.length);
	}
	
	private static class MyUserUrlInfo extends UserUrlInfo{
	}
	
	private static class MyUserUrlFactory implements UserUrlFactory<MyUserUrlInfo>{
		public MyUserUrlInfo create() {
			return new MyUserUrlInfo();
		}
	}
}
