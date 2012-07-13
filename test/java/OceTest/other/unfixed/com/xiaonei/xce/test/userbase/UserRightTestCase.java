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
import com.xiaonei.xce.userbasic.UserBasicAdapter;
import com.xiaonei.xce.userright.UserRightAdapter;
import com.xiaonei.xce.userright.UserRightFactory;
import com.xiaonei.xce.userright.UserRightInfo;

@RunWith (Parameterized.class)
public class UserRightTestCase {
	private int _id;
	private int _authO;
	private int _common_rightsO;
	private int _authN;
	private int _common_rightsN;
	
	public UserRightTestCase(int id, int authO, int common_rightsO, int authN, int common_rightsN){
		_id=id;
		_authO=authO;
		_common_rightsO=common_rightsO;
		_authN=authN;
		_common_rightsN=common_rightsN;
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_right");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_right where id="+_id;
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
		UserRightAdapter<MyUserRightInfo> ura=new UserRightAdapter<MyUserRightInfo>(new MyUserRightFactory());
		MyUserRightInfo muriw1=new MyUserRightInfo();
		muriw1.setId(_id);
		muriw1.setAuth(_authO);
		muriw1.setRights(_common_rightsO);
		ura.createUserRight(_id, muriw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_right");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, auth, common_rights FROM user_right WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("auth"), _authO);
						Assert.assertEquals(rs.getInt("common_rights"), _common_rightsO);
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
			MyUserRightInfo murir1=ura.getUserRight(_id);
			Assert.assertEquals(_id,murir1.getId());
			Assert.assertEquals(_authO,murir1.getAuth());
			Assert.assertEquals(_common_rightsO,murir1.getRights());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserRightInfo muriw2=new MyUserRightInfo();
		muriw2.setAuth(_authN);
		muriw2.setRights(_common_rightsN);
		ura.setUserRight(_id, muriw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_right");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, auth, common_rights FROM user_right WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("auth"), _authN);
						Assert.assertEquals(rs.getInt("common_rights"), _common_rightsN);
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
			MyUserRightInfo murir2=ura.getUserRight(_id);
			Assert.assertEquals(_id,murir2.getId());
			Assert.assertEquals(_authN,murir2.getAuth());
			Assert.assertEquals(_common_rightsN,murir2.getRights());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		
	}
	
	private static class MyUserRightInfo extends UserRightInfo{
		
	}
	
	private static class MyUserRightFactory implements UserRightFactory<MyUserRightInfo>{
		public MyUserRightInfo create() {
			return new MyUserRightInfo();
		}
	}
}
