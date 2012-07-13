package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.ElementarySchoolInfo;

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
public class ElementarySchoolInfoTestCase {
	private int userid_;
	private int elementary_school_idO_;
	private String elementary_school_nameO_;
	private int elementary_school_yearO_;
	private int elementary_school_idN_;
	private String elementary_school_nameN_;
	private int elementary_school_yearN_;
	public ElementarySchoolInfoTestCase(int userid, int elementary_school_idO, String elementary_school_nameO, int elementary_school_yearO,int elementary_school_idN, String elementary_school_nameN, int elementary_school_yearN){
		userid_=userid;
		elementary_school_idO_=elementary_school_idO;
		elementary_school_nameO_=elementary_school_nameO;
		elementary_school_yearO_=elementary_school_yearO;
		elementary_school_idN_=elementary_school_idN;
		elementary_school_nameN_=elementary_school_nameN;
		elementary_school_yearN_=elementary_school_yearN;
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
		_apt.reloadElementarySchoolInfo(userid_);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select * from elementary_school_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						while(rs.next()){
							Assert.assertEquals(userid_,rs.getInt("userid"));
							Assert.assertEquals(elementary_school_idO_,rs.getInt("elementary_school_id"));
							Assert.assertEquals(elementary_school_nameO_,rs.getString("elementary_school_name"));
							Assert.assertEquals(elementary_school_yearO_,rs.getInt("elementary_school_year"));
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
		ElementarySchoolInfo jhsir=_apt.getElementarySchoolInfo(userid_);
		Assert.assertEquals(userid_,jhsir.getUserId());
		Assert.assertEquals(elementary_school_idO_,jhsir.getElementarySchoolId());
		Assert.assertEquals(elementary_school_nameO_,jhsir.getElementarySchoolName());
		Assert.assertEquals(elementary_school_yearO_,jhsir.getElementarySchoolYear());
		
		ElementarySchoolInfo jhsiw=new ElementarySchoolInfo();
		jhsiw.setUserId(userid_);
		jhsiw.setElementarySchoolName(elementary_school_nameN_);
		jhsiw.setElementarySchoolId(elementary_school_idN_);
		jhsiw.setElementarySchoolYear(elementary_school_yearN_);
		_apt.addElementarySchoolInfo(userid_, jhsiw);
		
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select * from elementary_school_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						while(rs.next()){
							Assert.assertEquals(userid_,rs.getInt("userid"));
							Assert.assertEquals(elementary_school_idN_,rs.getInt("elementary_school_id"));
							Assert.assertEquals(elementary_school_nameN_,rs.getString("elementary_school_name"));
							Assert.assertEquals(elementary_school_yearN_,rs.getInt("elementary_school_year"));
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
		
		jhsir=_apt.getElementarySchoolInfo(userid_);
		Assert.assertEquals(userid_,jhsir.getUserId());
		Assert.assertEquals(elementary_school_idN_,jhsir.getElementarySchoolId());
		Assert.assertEquals(elementary_school_nameN_,jhsir.getElementarySchoolName());
		Assert.assertEquals(elementary_school_yearN_,jhsir.getElementarySchoolYear());
		
		_apt.removeElementarySchoolInfo(userid_);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select count(*) as c from elementary_school_info where userid="+userid_;
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
		
		Assert.assertEquals(_apt.getElementarySchoolInfo(userid_),null);
	}

	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="replace into elementary_school_info(userid,elementary_school_id,elementary_school_name,elementary_school_year) values ("+userid_+","+elementary_school_idO_+",'"+elementary_school_nameO_+"',"+elementary_school_yearO_+")";
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
