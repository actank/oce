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
import com.xiaonei.xce.userurl.UserUrlAdapter;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;

@RunWith (Parameterized.class)
public class UserUrlTestCase {
	private int _id;
	private String _largeurlO;
	private String _headurlO;
	private String _tinyurlO;
	private String _mainurlO;
	private String _largeurlN;
	private String _headurlN;
	private String _mainurlN;
	private String _tinyurlN;
	
	public UserUrlTestCase(int id, String largeurlO, String headurlO, String tinyurlO, String mainurlO, String largeurlN, String headurlN, String tinyurlN, String mainurlN){
		_id=id;
		_largeurlO=largeurlO;
		_headurlO=headurlO;
		_tinyurlO=tinyurlO;
		_mainurlO=mainurlO;
		_largeurlN=largeurlN;
		_headurlN=headurlN;
		_tinyurlN=tinyurlN;
		_mainurlN=mainurlN;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"largeurl1","headurl1","tinyurl1","mainurl1","largeurl2","headurl2","tinyurl2","mainurl2"},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_url where id="+_id;
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
		UserUrlAdapter<MyUserUrlInfo> uua=new UserUrlAdapter<MyUserUrlInfo>(new MyUserUrlFactory());
		MyUserUrlInfo muuiw1=new MyUserUrlInfo();
		muuiw1.setId(_id);
		muuiw1.setHeadUrl(_headurlO);
		muuiw1.setLargeUrl(_largeurlO);
		muuiw1.setMainUrl(_mainurlO);
		muuiw1.setTinyUrl(_tinyurlO);
		uua.createUserUrl(_id, muuiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, largeurl, headurl, mainurl, tinyurl FROM user_url WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("largeurl"), _largeurlO);
						Assert.assertEquals(rs.getString("headurl"), _headurlO);
						Assert.assertEquals(rs.getString("mainurl"), _mainurlO);
						Assert.assertEquals(rs.getString("tinyurl"), _tinyurlO);
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
			MyUserUrlInfo muuir1=uua.getUserUrl(_id);
			Assert.assertEquals(_largeurlO,muuir1.getLargeUrl());
			Assert.assertEquals(_tinyurlO,muuir1.getTinyUrl());
			Assert.assertEquals(_mainurlO,muuir1.getMainUrl());
			Assert.assertEquals(_headurlO, muuir1.getHeadUrl());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserUrlInfo muuiw2=new MyUserUrlInfo();
		muuiw2.setId(_id);
		muuiw2.setHeadUrl(_headurlN);
		muuiw2.setLargeUrl(_largeurlN);
		muuiw2.setMainUrl(_mainurlN);
		muuiw2.setTinyUrl(_tinyurlN);
		uua.setUserUrl(_id, muuiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_url");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, largeurl, headurl, mainurl, tinyurl FROM user_url WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						System.out.println(rs.getString("largeurl")+" "+_largeurlN);
						Assert.assertEquals(rs.getString("largeurl"), _largeurlN);
						Assert.assertEquals(rs.getString("headurl"), _headurlN);
						Assert.assertEquals(rs.getString("mainurl"), _mainurlN);
						Assert.assertEquals(rs.getString("tinyurl"), _tinyurlN);
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
			MyUserUrlInfo muuir2=uua.getUserUrl(_id);
			Assert.assertEquals(_id,muuir2.getId());
			Assert.assertEquals(_largeurlN,muuir2.getLargeUrl());
			Assert.assertEquals(_tinyurlN,muuir2.getTinyUrl());
			Assert.assertEquals(_mainurlN,muuir2.getMainUrl());
			Assert.assertEquals(_headurlN, muuir2.getHeadUrl());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserUrlInfo extends UserUrlInfo{
	}
	
	private static class MyUserUrlFactory implements UserUrlFactory<MyUserUrlInfo>{
		public MyUserUrlInfo create() {
			return new MyUserUrlInfo();
		}
	}
}
