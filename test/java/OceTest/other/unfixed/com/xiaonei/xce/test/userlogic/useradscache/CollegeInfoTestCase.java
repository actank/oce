package com.xiaonei.xce.test.userlogic.useradscache;

import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import junit.framework.Assert;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.network.CollegeInfo;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runners.Parameterized.Parameters;

import xce.clusterstate.ClusterStateSubscriber;
import xce.serverstate.ServerStateInfo;
import xce.useradscache.SchoolInfo;
import xce.useradscache.SchoolType;
import xce.useradscache.UserAdsCacheData;
import xce.useradscache.UserAdsCacheManagerPrx;
import xce.useradscache.UserAdsCacheManagerPrxHelper;
import xce.useradscache.UserAdsCacheUrlData;

import com.xiaonei.xce.XceAdapter;

public class CollegeInfoTestCase {
	private int _userid;
	private int _college_id1;
	private String _college_name1;
	private int _enroll_year1;
	private String _department1;
	private int _college_id2;
	private String _college_name2;
	private int _enroll_year2;
	private String _department2;
	public CollegeInfoTestCase(int userid,int college_id1,String college_name1,int enroll_year1,
			String department1,int college_id2,String college_name2,int enroll_year2,String department2){
		_userid=userid;
		_college_id1=college_id1;
		_college_name1=college_name1;
		_enroll_year1=enroll_year1;
		_department1=department1;
		_college_id2=college_id2;
		_college_name2=college_name2;
		_enroll_year2=enroll_year2;
		_department2=department2;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,0,"college_name1",0,"department1",1,"college_name2",
					1,"department2"},
			});
	}
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_basic");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into user_basic(id,stage,status,birth_year,birth_month,birth_day,gender,home_province,home_city) values ("+_userid+",0,0,0,0,0,'','','')";
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
					String sql="replace into user_url(id,tinyurl) values ("+_userid+",'')";
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
					String sql="delete from region_info where userid="+_userid;
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
					String sql="delete from network_history where userid="+_userid;
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
					String sql="delete from workplace_info where userid="+_userid;
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
					String sql="delete from elementary_school_info where userid="+_userid;
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
					String sql="delete from junior_high_school_info where userid="+_userid;
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
					String sql="delete from high_school_info where userid="+_userid;
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
					String sql="delete from college_info where userid="+_userid;
					s.execute(sql);
					sql="replace into college_info(userid,college_id,college_name,enroll_year,department) values("+_userid+","+_college_id1+",'"+_college_name1+"',"+_enroll_year1+","+_department1+")";
					s.execute(sql);
					sql="replace into college_info(userid,college_id,college_name,enroll_year,department) values("+_userid+","+_college_id2+",'"+_college_name2+"',"+_enroll_year2+","+_department2+")";
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
					String sql="delete from university_info where userid="+_userid;
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
		CollegeInfo ci=new CollegeInfo();
		ci.setCollegeId(_college_id1);
		ci.setCollegeName(_college_name1);
		ci.setDepartment(_department1);
		ci.setEnrollYear(_enroll_year1);
		ci.setUserId(_userid);
		AdapterFactory.getNetworkAdapter().setCollegeInfo(_userid,_college_id1,ci);
		ci=new CollegeInfo();
		ci.setCollegeId(_college_id2);
		ci.setCollegeName(_college_name2);
		ci.setDepartment(_department2);
		ci.setEnrollYear(_enroll_year2);
		ci.setUserId(_userid);
		AdapterFactory.getNetworkAdapter().setCollegeInfo(_userid, _college_id2, ci);
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
		List<UserAdsCacheManagerPrx> managers=getProxies(_userid);
		for(UserAdsCacheManagerPrx uscmp:managers){
			int[] ids=new int[]{_userid};
			Map<Integer,UserAdsCacheData> maps=uscmp.getUserAdsCacheList(ids);
			Assert.assertTrue(maps.containsKey(_userid));
			Assert.assertEquals(ids.length, maps.size());
			check(maps.get(_userid));
			Map<Integer,UserAdsCacheUrlData> urlmaps=uscmp.getUserAdsCacheUrlList(ids);
			Assert.assertTrue(urlmaps.containsKey(_userid));
			Assert.assertEquals(ids.length, urlmaps.size());
			check(urlmaps.get(_userid));
		}
	}
	
	private void check(UserAdsCacheUrlData data){
		Assert.assertEquals(data.id, _userid);
		Assert.assertEquals(data.tinyurl, "");
		Assert.assertEquals(0, data.birthyear);
		Assert.assertEquals(0, data.birthmonth);
		Assert.assertEquals(0, data.birthday);
		Assert.assertEquals(0, data.stage);
		Assert.assertEquals(data.sex, "");
		Assert.assertEquals("", data.homeprovince);
		Assert.assertEquals("", data.homecity);
		Assert.assertEquals(data.seatcity, "");
		Assert.assertEquals(data.seatprovince, "");
		Assert.assertEquals(0, data.workplaces.length);
		Assert.assertEquals(2, data.schools.length);
		Assert.assertEquals(0, data.networks.length);
		check1(data.schools[0]);
		check2(data.schools[1]);
	}
	
	private void check2(SchoolInfo ni){
		Assert.assertEquals(ni.type, SchoolType.College);
		Assert.assertEquals(ni.id, _college_id2);
		Assert.assertEquals(ni.name, _college_name2);
		Assert.assertEquals(ni.department, _department2);
		Assert.assertEquals(ni.enrollyear, _enroll_year2);
	}
	
	private void check1(SchoolInfo ni){
		Assert.assertEquals(ni.type, SchoolType.College);
		Assert.assertEquals(ni.id, _college_id1);
		Assert.assertEquals(ni.name, _college_name1);
		Assert.assertEquals(ni.department, _department1);
		Assert.assertEquals(ni.enrollyear, _enroll_year1);
	}
	
	private void check(UserAdsCacheData data){
		Assert.assertEquals(_userid, data.id);
		Assert.assertEquals(0, data.birthyear);
		Assert.assertEquals(0, data.birthmonth);
		Assert.assertEquals(0, data.birthday);
		Assert.assertEquals(0, data.stage);
		Assert.assertEquals(data.sex, "");
		Assert.assertEquals("", data.homeprovince);
		Assert.assertEquals("", data.homecity);
		Assert.assertEquals(data.seatcity, "");
		Assert.assertEquals(data.seatprovince, "");
		Assert.assertEquals(0, data.workplaces.length);
		Assert.assertEquals(2, data.schools.length);
		Assert.assertEquals(0, data.networks.length);
		check1(data.schools[0]);
		check2(data.schools[1]);
	}
}
