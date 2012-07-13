package com.xiaonei.xce.test.userbase;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;

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
import com.xiaonei.xce.userdoing.UserDoingAdapter;
import com.xiaonei.xce.userdoing.UserDoingFactory;
import com.xiaonei.xce.userdoing.UserDoingInfo;

@RunWith (Parameterized.class)
public class UserDoingTestCase {
	private int _id;
	private String _doingO;
	private String _doingN;
	private Date _doingTimeO;
	private Date _doingTimeN;
	public UserDoingTestCase(int id, String doingO, Date doingTimeO, String doingN, Date doingTimeN){
		_id=id;
		_doingO=doingO;
		_doingN=doingN;
		_doingTimeO=doingTimeO;
		_doingTimeN=doingTimeN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"doing1",new Date(1000),"doing2",new Date(2000)},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_doing");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_doing where id="+_id;
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
		UserDoingAdapter<MyUserDoingInfo> uda=new UserDoingAdapter<MyUserDoingInfo>(new MyUserDoingFactory());
		MyUserDoingInfo mudiw1=new MyUserDoingInfo();
		mudiw1.setId(_id);
		mudiw1.setDoing(_doingO);
		mudiw1.setDoingTime(_doingTimeO);
		uda.createUserDoing(_id, mudiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_doing");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, doing, doingtime FROM user_doing WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("doing"), _doingO);
						Assert.assertEquals(rs.getTime("doingtime").getTime(), _doingTimeO.getTime());
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
			MyUserDoingInfo mudir1=uda.getUserDoing(_id);
			Assert.assertEquals(_id, mudir1.getId());
			Assert.assertEquals(_doingO, mudir1.getDoing());
			Assert.assertEquals(_doingTimeO.getTime(), mudir1.getDoingTime().getTime());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserDoingInfo mudiw2=new MyUserDoingInfo();
		mudiw2.setId(_id);
		mudiw2.setDoing(_doingN);
		mudiw2.setDoingTime(_doingTimeN);
		uda.setUserDoing(_id, mudiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_doing");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, doing, doingtime FROM user_doing WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("doing"), _doingN);
						Assert.assertEquals(rs.getTime("doingtime").getTime(), _doingTimeN.getTime());
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
			MyUserDoingInfo mudir1=uda.getUserDoing(_id);
			Assert.assertEquals(_id, mudir1.getId());
			Assert.assertEquals(_doingN, mudir1.getDoing());
			Assert.assertEquals(_doingTimeN.getTime(), mudir1.getDoingTime().getTime());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserDoingInfo extends UserDoingInfo{
		
	}
	
	private static class MyUserDoingFactory implements UserDoingFactory<MyUserDoingInfo>{

		public MyUserDoingInfo create() {
			return new MyUserDoingInfo();
		}
		
	}
}
