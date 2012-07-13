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
import com.xiaonei.xce.username.UserNameAdapter;
import com.xiaonei.xce.username.UserNameFactory;
import com.xiaonei.xce.username.UserNameInfo;

@RunWith (Parameterized.class)
public class UserNameTestCase {
	private int _id;
	private String _nameO;
	private String _nicknameO;
	private String _nameN;
	private String _nicknameN;
	
	public UserNameTestCase(int id, String nameO, String nicknameO, String nameN, String nicknameN){
		_id=id;
		_nameO=nameO;
		_nicknameO=nicknameO;
		_nameN=nameN;
		_nicknameN=nicknameN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"name1","nickname1","name2","nickname2"},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_names");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_names where id="+_id;
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
		UserNameAdapter<MyUserNameInfo> una=new UserNameAdapter<MyUserNameInfo>(new MyUserNameFactory());
		MyUserNameInfo muniw1=new MyUserNameInfo();
		muniw1.setId(_id);
		muniw1.setName(_nameO);
		muniw1.setNickname(_nicknameO);
		una.createUserName(_id, muniw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_names");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, name, nickname FROM user_names WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("name"), _nameO);
						Assert.assertEquals(rs.getString("nickname"), _nicknameO);
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
			MyUserNameInfo munir1=una.getUserName(_id);
			Assert.assertEquals(_id,munir1.getId());
			Assert.assertEquals(_nameO,munir1.getName());
			Assert.assertEquals(_nicknameO,munir1.getNickname());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserNameInfo muniw2=new MyUserNameInfo();
		muniw2.setName(_nameN);
		muniw2.setNickname(_nicknameN);
		una.setUserName(_id, muniw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_names");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, name, nickname FROM user_names WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("name"), _nameN);
						Assert.assertEquals(rs.getString("nickname"), _nicknameN);
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
			MyUserNameInfo munir2=una.getUserName(_id);
			Assert.assertEquals(_id,munir2.getId());
			Assert.assertEquals(_nameN,munir2.getName());
			Assert.assertEquals(_nicknameN,munir2.getNickname());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserNameInfo extends UserNameInfo{
		
	}
	
	private static class MyUserNameFactory implements UserNameFactory<MyUserNameInfo>{

		public MyUserNameInfo create() {
			// TODO Auto-generated method stub
			return new MyUserNameInfo();
		}
		
	}
}
