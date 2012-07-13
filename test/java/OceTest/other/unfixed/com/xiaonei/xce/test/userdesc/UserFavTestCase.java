package com.xiaonei.xce.test.userdesc;

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
import com.xiaonei.xce.userfav.UserFavAdapter;
import com.xiaonei.xce.userfav.UserFavFactory;
import com.xiaonei.xce.userfav.UserFavInfo;

@RunWith (Parameterized.class)
public class UserFavTestCase {
	private int _id;
	private String _favO;
	private int _fav_flagO;
	private String _favN;
	private int _fav_flagN;
	public UserFavTestCase(int id, String favO, int fav_flagO, String favN, int fav_flagN){
		_id=id;
		_favO=favO;
		_fav_flagO=fav_flagO;
		_favN=favN;
		_fav_flagN=fav_flagN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"fav1",1,"fav2",2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_fav_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_fav where id="+_id;
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
		UserFavAdapter<MyUserFavInfo> ufa=new UserFavAdapter<MyUserFavInfo>(new MyUserFavFactory());
		MyUserFavInfo mufiw1=new MyUserFavInfo();
		mufiw1.setId(_id);
		mufiw1.setFav(_favO);
		mufiw1.setFavFlag(_fav_flagO);
		ufa.createUserFav(_id, mufiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_fav_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, fav, fav_flag FROM user_fav WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("fav_flag"), _fav_flagO);
						Assert.assertEquals(rs.getString("fav"), _favO);
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
			MyUserFavInfo mufir1=ufa.getUserFav(_id);
			Assert.assertEquals(_id,mufir1.getId());
			Assert.assertEquals(_favO,mufir1.getFav());
			Assert.assertEquals(_fav_flagO,mufir1.getFavFlag());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserFavInfo mufiw2=new MyUserFavInfo();
		mufiw2.setId(_id);
		mufiw2.setFav(_favN);
		mufiw2.setFavFlag(_fav_flagN);
		ufa.setUserFav(_id, mufiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_fav_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, fav, fav_flag FROM user_fav WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("fav_flag"), _fav_flagN);
						Assert.assertEquals(rs.getString("fav"), _favN);
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
			MyUserFavInfo mufir2=ufa.getUserFav(_id);
			Assert.assertEquals(_id,mufir2.getId());
			Assert.assertEquals(_favN,mufir2.getFav());
			Assert.assertEquals(_fav_flagN,mufir2.getFavFlag());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserFavInfo extends UserFavInfo{
		
	}
	
	private static class MyUserFavFactory implements UserFavFactory<MyUserFavInfo>{
		public MyUserFavInfo create() {
			return new MyUserFavInfo();
		}
	}
}
