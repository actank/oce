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
import xce.useradscache.WorkplaceInfo;

import com.xiaonei.xce.XceAdapter;

public class WorkplaceInfoTestCase {
	private int _userid;
	private int _workplace_id1;
	private int _job_title_id1;
	private int _position_id1;
	private String _workplace_name1;
	private int _workplace_id2;
	private int _job_title_id2;
	private int _position_id2;
	private String _workplace_name2;
	public WorkplaceInfoTestCase(int userid, int workplace_id1, int job_title_id1, int position_id1, String workplace_name1, int workplace_id2, int job_title_id2, int position_id2,String workplace_name2){
		_userid=userid;
		_workplace_id1=workplace_id1;
		_job_title_id1=job_title_id1;
		_position_id1=position_id1;
		_workplace_name1=workplace_name1;
		_workplace_id2=workplace_id2;
		_job_title_id2=job_title_id2;
		_position_id2=position_id2;
		_workplace_name2=workplace_name2;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,0,0,0,"workplace_name1",1,1,1,"workplace_name2"},
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
					sql ="replace into workplace_info(userid,workplace_name, job_title_id, position_id) values("+_userid+",'"+_workplace_name1+"',"+_job_title_id1+","+_position_id1+")";
					s.execute(sql);
					sql ="replace into workplace_info(userid,workplace_name, job_title_id, position_id) values("+_userid+",'"+_workplace_name2+"',"+_job_title_id2+","+_position_id2+")";
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
		 mop.hi.oce.domain.network.WorkplaceInfo workplace=new  mop.hi.oce.domain.network.WorkplaceInfo();
		 workplace.setJobTitleId(_job_title_id1);
		 workplace.setWorkplaceId(_workplace_id1);
		 workplace.setPositionId(_position_id1);
		 workplace.setUserId(_userid);
		 workplace.setWorkplaceName(_workplace_name1);
		 AdapterFactory.getNetworkAdapter().setWorkplaceInfo(workplace);
		 workplace.setWorkplaceId(_workplace_id2);
		 workplace.setJobTitleId(_job_title_id2);
		 workplace.setPositionId(_position_id2);
		 workplace.setUserId(_userid);
		 workplace.setWorkplaceName(_workplace_name2);		 
		 AdapterFactory.getNetworkAdapter().setWorkplaceInfo(workplace);
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
		Assert.assertEquals(2, data.workplaces.length);
		Assert.assertEquals(0, data.schools.length);
		Assert.assertEquals(0, data.networks.length);
		check1(data.workplaces[0]);
		check2(data.workplaces[1]);
	}
	
	private void check2(WorkplaceInfo wi){
		Assert.assertEquals(wi.jobtitleid, _job_title_id2);
		Assert.assertEquals(wi.positionid, _position_id2);
		Assert.assertEquals(wi.workplacename, _workplace_name2);
	}
	
	private void check1(WorkplaceInfo wi){
		Assert.assertEquals(wi.jobtitleid, _job_title_id1);
		Assert.assertEquals(wi.positionid, _position_id1);
		Assert.assertEquals(wi.workplacename, _workplace_name1);
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
		Assert.assertEquals(2, data.workplaces.length);
		Assert.assertEquals(0, data.schools.length);
		Assert.assertEquals(0, data.networks.length);
		check1(data.workplaces[0]);
		check2(data.workplaces[1]);
	}
}
