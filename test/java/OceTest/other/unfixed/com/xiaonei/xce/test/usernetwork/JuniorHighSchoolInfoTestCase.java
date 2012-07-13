package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.JuniorHighSchoolInfo;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.util.channel.Channel;
import Ice.ObjectPrx;
import MyUtil.ObjectCachePrx;
import MyUtil.ObjectCachePrxHelper;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class JuniorHighSchoolInfoTestCase {
	private int userid_;
	private int junior_high_school_idO_;
	private String junior_high_school_nameO_;
	private int junior_high_school_yearO_;
	private int junior_high_school_idN_;
	private String junior_high_school_nameN_;
	private int junior_high_school_yearN_;
	public JuniorHighSchoolInfoTestCase(int userid, int junior_high_school_idO, String junior_high_school_nameO, int junior_high_school_yearO,int junior_high_school_idN, String junior_high_school_nameN, int junior_high_school_yearN){
		userid_=userid;
		junior_high_school_idO_=junior_high_school_idO;
		junior_high_school_nameO_=junior_high_school_nameO;
		junior_high_school_yearO_=junior_high_school_yearO;
		junior_high_school_idN_=junior_high_school_idN;
		junior_high_school_nameN_=junior_high_school_nameN;
		junior_high_school_yearN_=junior_high_school_yearN;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,6813001,"jhsi1",1,6813002,"jhsi2",2},
			});
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
		NetworkAdapter _apt = new AdapterFactory().getNetworkAdapter();
		_apt.reloadJuniorHighSchoolInfo(userid_);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select * from junior_high_school_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						while(rs.next()){
							Assert.assertEquals(userid_,rs.getInt("userid"));
							Assert.assertEquals(junior_high_school_idO_,rs.getInt("junior_high_school_id"));
							Assert.assertEquals(junior_high_school_nameO_,rs.getString("junior_high_school_name"));
							Assert.assertEquals(junior_high_school_yearO_,rs.getInt("junior_high_school_year"));
						}
					}catch(Exception e){
						e.printStackTrace();
						Assert.assertFalse(true);
					}finally{
						rs.close();
					}
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
		JuniorHighSchoolInfo jhsir=_apt.getJuniorHighSchoolInfo(userid_);
		Assert.assertEquals(userid_,jhsir.getUserId());
		Assert.assertEquals(junior_high_school_idO_,jhsir.getJunHighentarySchoolId());
		Assert.assertEquals(junior_high_school_nameO_,jhsir.getJunHighentarySchoolName());
		Assert.assertEquals(junior_high_school_yearO_,jhsir.getJunHighentarySchoolYear());
		
		JuniorHighSchoolInfo jhsiw=new JuniorHighSchoolInfo();
		jhsiw.setUserId(userid_);
		jhsiw.setJunHighentarySchoolName(junior_high_school_nameN_);
		jhsiw.setJunHighentarySchoolId(junior_high_school_idN_);
		jhsiw.setJunHighentarySchoolYear(junior_high_school_yearN_);
		_apt.addJuniorHighSchoolInfo(userid_, jhsiw);
		
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select * from junior_high_school_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						while(rs.next()){
							Assert.assertEquals(userid_,rs.getInt("userid"));
							Assert.assertEquals(junior_high_school_idN_,rs.getInt("junior_high_school_id"));
							Assert.assertEquals(junior_high_school_nameN_,rs.getString("junior_high_school_name"));
							Assert.assertEquals(junior_high_school_yearN_,rs.getInt("junior_high_school_year"));
						}
					}catch(Exception e){
						e.printStackTrace();
						Assert.assertFalse(true);
					}finally{
						rs.close();
					}
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
		
		jhsir=_apt.getJuniorHighSchoolInfo(userid_);
		Assert.assertEquals(userid_,jhsir.getUserId());
		Assert.assertEquals(junior_high_school_idN_,jhsir.getJunHighentarySchoolId());
		Assert.assertEquals(junior_high_school_nameN_,jhsir.getJunHighentarySchoolName());
		Assert.assertEquals(junior_high_school_yearN_,jhsir.getJunHighentarySchoolYear());
		
		_apt.removeJuniorHighSchoolInfo(userid_);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select count(*) as c from junior_high_school_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						if(rs.next()){
							Assert.assertEquals(rs.getInt("c"),0);
						}else {
							Assert.assertFalse(true);
						}
					}catch(Exception e){
						e.printStackTrace();
						Assert.assertFalse(true);
					}finally{
						rs.close();
					}
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
		
		Assert.assertEquals(_apt.getJuniorHighSchoolInfo(userid_),null);
	}

	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into junior_high_school_info(userid,junior_high_school_id,junior_high_school_name,junior_high_school_year) values ("+userid_+","+junior_high_school_idO_+",'"+junior_high_school_nameO_+"',"+junior_high_school_yearO_+")";
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
}
