package com.xiaonei.xce.test.usernetwork;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import junit.framework.Assert;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.NetworkAdapter;
import mop.hi.oce.domain.network.CollegeInfo;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

@RunWith (Parameterized.class)
public class CollegeInfoTestCase {
	private int userid_;
	private int college_idO_;
	private String college_nameO_;
	private String departmentO_;
	private int enroll_yearO_;
	private int useridN_;
	private int college_idN_;
	private String college_nameN_;
	private String departmentN_;
	private int enroll_yearN_;
	public CollegeInfoTestCase(int userid,int college_idO, String college_nameO,String departmentO, int enroll_yearO,
			int college_idN, String college_nameN, String departmentN,int enroll_yearN){
		userid_=userid;
		college_idO_ =college_idO;
		college_nameO_=college_nameO;
		departmentO_=departmentO;
		enroll_yearO_=enroll_yearO;
		college_idN_ =college_idN;
		college_nameN_=college_nameN;
		departmentN_=departmentN;
		enroll_yearN_=enroll_yearN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{6813,6813001,"jhsi1","department1",1,6813002,"jhsi2","department2",2},
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
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from college_info where userid="+userid_;
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
	
	@Test
	public void test() {
		NetworkAdapter _apt = new AdapterFactory().getNetworkAdapter();
		_apt.reloadCollegeInfoList(userid_);
		
		CollegeInfo collegew=new CollegeInfo();
		collegew.setCollegeId(college_idO_);
		collegew.setCollegeName(college_nameO_);
		collegew.setDepartment(departmentO_);
		collegew.setEnrollYear(enroll_yearO_);
		collegew.setUserId(userid_);
		_apt.addCollegeInfo(userid_, collegew);
		
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select * from college_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						while(rs.next()){
							Assert.assertEquals(userid_,rs.getInt("userid"));
							Assert.assertEquals(college_idO_,rs.getInt("college_id"));
							Assert.assertEquals(college_nameO_,rs.getString("college_name"));
							Assert.assertEquals(enroll_yearO_,rs.getInt("enroll_year"));
							Assert.assertEquals(departmentO_,rs.getString("department"));
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
		List<CollegeInfo> infos=_apt.getCollegeInfoList(userid_);
		Assert.assertEquals(1, infos.size());
		Assert.assertEquals(_apt.getCollegeInfoCount(userid_),1);
		for(CollegeInfo info:infos){
			if(info.getCollegeId()==college_idO_){
				checkO(info);
				checkO(_apt.getCollegeInfo(userid_, info.getId()));
				checkO(_apt.getCollegeInfoByCollegeId(userid_, info.getCollegeId()));
			}else if (info.getCollegeId()==college_idN_){
				checkN(info);
				checkN(_apt.getCollegeInfo(userid_, info.getId()));
				checkN(_apt.getCollegeInfoByCollegeId(userid_, info.getCollegeId()));
			}else {
				Assert.assertFalse(true);
			}
		}
		
		CollegeInfo collegew2=new CollegeInfo();
		collegew2.setCollegeId(college_idN_);
		collegew2.setCollegeName(college_nameN_);
		collegew2.setDepartment(departmentN_);
		collegew2.setEnrollYear(enroll_yearN_);
		collegew2.setUserId(userid_);
		_apt.addCollegeInfo(userid_, collegew2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select * from college_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						while(rs.next()){
							if(rs.getInt("college_id")==college_idO_){
								Assert.assertEquals(userid_,rs.getInt("userid"));
								Assert.assertEquals(college_idO_,rs.getInt("college_id"));
								Assert.assertEquals(college_nameO_,rs.getString("college_name"));
								Assert.assertEquals(enroll_yearO_,rs.getInt("enroll_year"));
								Assert.assertEquals(departmentO_,rs.getString("department"));
							}else if (rs.getInt("college_id")==college_idN_){
								Assert.assertEquals(userid_,rs.getInt("userid"));
								Assert.assertEquals(college_idN_,rs.getInt("college_id"));
								Assert.assertEquals(college_nameN_,rs.getString("college_name"));
								Assert.assertEquals(enroll_yearN_,rs.getInt("enroll_year"));
								Assert.assertEquals(departmentN_,rs.getString("department"));
							}else {
								Assert.assertFalse(true);
							}
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
		infos=_apt.getCollegeInfoList(userid_);
		Assert.assertEquals(2, infos.size());
		Assert.assertEquals(_apt.getCollegeInfoCount(userid_),2);
		for(CollegeInfo info:infos){
			if(info.getCollegeId()==college_idO_){
				checkO(info);
				checkO(_apt.getCollegeInfo(userid_, info.getId()));
				checkO(_apt.getCollegeInfoByCollegeId(userid_, info.getCollegeId()));
			}else if (info.getCollegeId()==college_idN_){
				checkN(info);
				checkN(_apt.getCollegeInfo(userid_, info.getId()));
				checkN(_apt.getCollegeInfoByCollegeId(userid_, info.getCollegeId()));
			}else {
				Assert.assertFalse(true);
			}
		}
		_apt.removeAllCollegeInfo(userid_);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("common");
			try{
				Statement s=c.createStatement();
				try{
					String sql="select count(*) as c from college_info where userid="+userid_;
					ResultSet rs=s.executeQuery(sql);
					try{
						if(rs.next()){
							Assert.assertEquals(rs.getInt("c"), 0);
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
		infos=_apt.getCollegeInfoList(userid_);
		Assert.assertEquals(0, infos.size());
		Assert.assertEquals(_apt.getCollegeInfoCount(userid_),0);
		
		_apt.addCollegeInfo(userid_, collegew);
		_apt.addCollegeInfo(userid_, collegew2);
		
		infos=_apt.getCollegeInfoList(userid_);
		boolean flag=false;
		for(CollegeInfo info:infos){
			if(info.getCollegeId()==college_idN_){
				_apt.removeCollegeInfo(userid_, info.getId());
				flag=true;
			}
		}
		//Assert.assertEquals(flag, true);
	}
	private void checkN(CollegeInfo info){
		Assert.assertEquals(info.getCollegeId(),college_idN_);
		Assert.assertEquals(info.getCollegeName(),college_nameN_);
		Assert.assertEquals(info.getDepartment(),departmentN_);
		Assert.assertEquals(info.getEnrollYear(),enroll_yearN_);
		Assert.assertEquals(info.getUserId(), userid_);
	}
	private void checkO(CollegeInfo info){
		Assert.assertEquals(info.getCollegeId(),college_idO_);
		Assert.assertEquals(info.getCollegeName(),college_nameO_);
		Assert.assertEquals(info.getDepartment(),departmentO_);
		Assert.assertEquals(info.getEnrollYear(),enroll_yearO_);
		Assert.assertEquals(info.getUserId(), userid_);
	}
}
