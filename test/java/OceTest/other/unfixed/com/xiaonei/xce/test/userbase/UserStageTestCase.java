package com.xiaonei.xce.test.userbase;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userstage.UserStageAdapter;
import com.xiaonei.xce.userstage.UserStageFactory;
import com.xiaonei.xce.userstage.UserStageInfo;

@RunWith (Parameterized.class)
public class UserStageTestCase {
	private int _id;
	private int _stageO;
	private int _univO;
	private int _stageN;
	private int _univN;
	public UserStageTestCase(int id, int stageO,int univO,int stageN,int univN){
		_id=id;
		_stageO=stageO;
		_stageN=stageN;
		_univO=univO;
		_univN=univN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,1,1,2,2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_stage where id="+_id;
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
		UserStageAdapter<MyUserStageInfo> uda=new UserStageAdapter<MyUserStageInfo>(new MyUserStageFactory());
		MyUserStageInfo mudiw1=new MyUserStageInfo();
		mudiw1.setId(_id);
		mudiw1.setStage(_stageO);
		mudiw1.setUniv(_univO);
		uda.createUserStage(_id, mudiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, stage, univ FROM user_stage WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("stage"), _stageO);
						Assert.assertEquals(rs.getInt("univ"), _univO);
					}
					rs.close();
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
			MyUserStageInfo mudir1=uda.getUserStage(_id);
			Assert.assertEquals(_id, mudir1.getId());
			Assert.assertEquals(_stageO, mudir1.getStage());
			Assert.assertEquals(_univO, mudir1.getUniv());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserStageInfo mudiw2=new MyUserStageInfo();
		mudiw2.setId(_id);
		mudiw2.setStage(_stageN);
		mudiw2.setUniv(_univN);
		uda.setUserStage(_id, mudiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_stage");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, stage, univ FROM user_stage WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("stage"), _stageN);
						Assert.assertEquals(rs.getInt("univ"), _univN);
					}
					rs.close();
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
			MyUserStageInfo mudir1=uda.getUserStage(_id);
			Assert.assertEquals(_id, mudir1.getId());
			Assert.assertEquals(_stageN, mudir1.getStage());
			Assert.assertEquals(_univN, mudir1.getUniv());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserStageInfo extends UserStageInfo{
		
	}
	
	private static class MyUserStageFactory implements UserStageFactory<MyUserStageInfo>{

		public MyUserStageInfo create() {
			return new MyUserStageInfo();
		}
		
	}
}
