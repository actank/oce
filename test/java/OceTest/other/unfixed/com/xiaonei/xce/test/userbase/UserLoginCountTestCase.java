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

import xce.userbase.UserLoginCountManagerPrx;
import xce.userbase.UserLoginCountManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userlogincount.UserLoginCountAdapter;

@RunWith (Parameterized.class)
public class UserLoginCountTestCase {
	private int _id;
	private int _logincountO;
	private int _logincountN;

	public UserLoginCountTestCase(int id, int logincountO, int logincountN){
		_id=id;
		_logincountO=logincountO;
		_logincountN=logincountN;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,1,2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_logincount");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_logincount where id="+_id;
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
		ObjectPrx op=Channel.newUserChannel("user").getCommunicator().stringToProxy("ULCM@UserBase"+_id%100);
		UserLoginCountManagerPrx ulcmp=UserLoginCountManagerPrxHelper.uncheckedCast(op);
		ulcmp.reload(_id);
	}
	
	@After
	public void tearDownAfter() throws Exception {  

	}
	
	@Test
	public void test() {
		UserLoginCountAdapter ulca=new UserLoginCountAdapter();
		ulca.incUserLoginCount(_id);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_logincount");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, logincount FROM user_logincount WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						System.out.println(rs.getInt("logincount")+" "+ _logincountO);
						Assert.assertEquals(rs.getInt("logincount"), _logincountO);
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
		try {
			System.out.println(_logincountO+" "+ ulca.getUserLoginCount(_id));
			Assert.assertEquals(_logincountO,ulca.getUserLoginCount(_id));
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		ulca.incUserLoginCount(_id);
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_logincount");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, logincount FROM user_logincount WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						System.out.println(rs.getInt("logincount")+" "+ _logincountN);
						Assert.assertEquals(rs.getInt("logincount"), _logincountN);
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
		try {
			System.out.println(_logincountN+" "+ ulca.getUserLoginCount(_id));
			Assert.assertEquals(_logincountN,ulca.getUserLoginCount(_id));
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
}
