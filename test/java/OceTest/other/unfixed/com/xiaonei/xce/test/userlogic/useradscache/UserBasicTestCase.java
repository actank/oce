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
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.clusterstate.ClusterStateSubscriber;
import xce.serverstate.ServerStateInfo;
import xce.useradscache.UserAdsCacheData;
import xce.useradscache.UserAdsCacheManagerPrx;
import xce.useradscache.UserAdsCacheManagerPrxHelper;
import xce.useradscache.UserAdsCacheUrlData;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userbasic.UserBasicAdapter;
import com.xiaonei.xce.userbasic.UserBasicFactory;
import com.xiaonei.xce.userbasic.UserBasicInfo;
@RunWith (Parameterized.class)
public class UserBasicTestCase {
	private int _id;
	private int _stage;
	private int _birth_year;
	private int _birth_month;
	private int _birth_day;
	private String _gender;
	private String _home_province;
	private String _home_city;
	public UserBasicTestCase(int id, int stage, int birth_year, int birth_month, int birth_day, String gender,
			String home_province, String home_city){
		_id=id;
		_stage=stage;
		_birth_year=birth_year;
		_birth_month=birth_month;
		_birth_day=birth_day;
		_gender=gender;
		_home_province=home_province;
		_home_city=home_city;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,1,1,1,1,"gender","home_province","home_city"},
			});
	}
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_basic(id,stage,status,birth_year,birth_month,birth_day,gender,home_province,home_city) values ("+_id+",-1,0,0,0,0,'','','')";
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
		UserBasicAdapter<MyUserBasicInfo> uba=new UserBasicAdapter<MyUserBasicInfo>(new MyUserBasicFactory());
		MyUserBasicInfo mubis1=new MyUserBasicInfo();
		mubis1.setBirthDay(_birth_day);
		mubis1.setStage(_stage);
		mubis1.setBirthMonth(_birth_month);
		mubis1.setBirthYear(_birth_year);
		mubis1.setGender(_gender);
		mubis1.setHomeCity(_home_city);
		mubis1.setHomeProvince(_home_province);
		uba.setUserBasic(_id, mubis1);
		try {
			Thread.sleep(2000);
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
		//Assert.assertEquals(data.tinyurl, "");
		Assert.assertEquals(_birth_year, data.birthyear);
		Assert.assertEquals(_birth_month, data.birthmonth);
		Assert.assertEquals(_birth_day, data.birthday);
		Assert.assertEquals(_stage, data.stage);
		Assert.assertEquals(data.sex, _gender);
		Assert.assertEquals(_home_province, data.homeprovince);
		Assert.assertEquals(_home_city, data.homecity);
		Assert.assertEquals(data.seatcity, "");
		Assert.assertEquals(data.seatprovince, "");
		Assert.assertEquals(0, data.networks.length);
		Assert.assertEquals(0, data.workplaces.length);
		Assert.assertEquals(0, data.schools.length);
	}
	
	private void check(UserAdsCacheData data){
		Assert.assertEquals(_id, data.id);
		Assert.assertEquals(_birth_year, data.birthyear);
		Assert.assertEquals(_birth_month, data.birthmonth);
		Assert.assertEquals(_birth_day, data.birthday);
		Assert.assertEquals(_stage, data.stage);
		Assert.assertEquals(data.sex, _gender);
		Assert.assertEquals(_home_province, data.homeprovince);
		Assert.assertEquals(_home_city, data.homecity);
		Assert.assertEquals(data.seatcity, "");
		Assert.assertEquals(data.seatprovince, "");
		Assert.assertEquals(0, data.networks.length);
		Assert.assertEquals(0, data.workplaces.length);
		Assert.assertEquals(0, data.schools.length);
	}
	
	private static class MyUserBasicInfo extends UserBasicInfo{
		private static final long serialVersionUID = 1L;
		
	}
	
	private static class MyUserBasicFactory implements UserBasicFactory<MyUserBasicInfo>{
		public MyUserBasicInfo create() {
			return new MyUserBasicInfo();
		}		
	}
}
