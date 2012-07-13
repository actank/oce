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
import com.xiaonei.xce.userpassport.UserPassportAdapter;
import com.xiaonei.xce.userpassport.UserPassportFactory;
import com.xiaonei.xce.userpassport.UserPassportInfo;

@RunWith (Parameterized.class)
public class UserPassportTestCase {
	private int _id;
	private String _accountO;
	private String _domainO;
	private String _passwordMd5O;
	private int _statusO;
	private int _safeStatusO;
	
	private String _accountN;
	private String _domainN;
	private String _passwordMd5N;
	private int _statusN;
	private int _safeStatusN;
	
	public UserPassportTestCase(int id, String accountO, String domainO, String passwordMd5O, int statusO, int safeStatusO,String accountN, String domainN, String passwordMd5N, int statusN, int safeStatusN){
		_id=id;
		_accountO=accountO;
		_domainO=domainO;
		_passwordMd5O=passwordMd5O;
		_statusO=statusO;
		_safeStatusO=safeStatusO;
		_accountN=accountN;
		_domainN=domainN;
		_passwordMd5N=passwordMd5N;
		_statusN=statusN;
		_safeStatusN=safeStatusN;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"account1","domain1","passwordMd51",1,1,"account2","domain2","passwordMd52",2,2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_passport where id="+_id;
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
		UserPassportAdapter<MyUserPassportInfo> upa=new UserPassportAdapter<MyUserPassportInfo>(new MyUserPassportFactory());
		MyUserPassportInfo mupiw1=new MyUserPassportInfo();
		mupiw1.setId(_id);
		mupiw1.setAccount(_accountO);
		mupiw1.setDomain(_domainO);
		mupiw1.setPasswordMd5(_passwordMd5O);
		mupiw1.setSafeStatus(_safeStatusO);
		mupiw1.setStatus(_statusO);
		upa.createUserPassport(_id, mupiw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, status, safe_status, domain, password_md5, account FROM user_passport WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("domain"), _domainO);
						Assert.assertEquals(rs.getInt("status"), _statusO);
						Assert.assertEquals(rs.getInt("safe_status"), _safeStatusO);
						Assert.assertEquals(rs.getString("password_md5"), _passwordMd5O);
						Assert.assertEquals(rs.getString("account"), _accountO);
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
			MyUserPassportInfo mupir1=upa.getUserPassport(_id);
			Assert.assertEquals(mupir1.getId(), _id);
			Assert.assertEquals(mupir1.getDomain(), _domainO);
			Assert.assertEquals(mupir1.getStatus(), _statusO);
			Assert.assertEquals(mupir1.getSafeStatus(), _safeStatusO);
			Assert.assertEquals(mupir1.getPasswordMd5(), _passwordMd5O);
			Assert.assertEquals(mupir1.getAccount(), _accountO);
			//Assert.assertEquals(upa.getUserIdByEmail(_accountO),_id);
			//Assert.assertEquals(upa.getUserIdByEmailAndDomain(_accountO,_domainO),_id);
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserPassportInfo mupiw2=new MyUserPassportInfo();
		mupiw2.setAccount(_accountN);
		mupiw2.setDomain(_domainN);
		mupiw2.setPasswordMd5(_passwordMd5N);
		mupiw2.setSafeStatus(_safeStatusN);
		mupiw2.setStatus(_statusN);
		upa.setUserPassport(_id, mupiw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_passport");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, status, safe_status, domain, password_md5, account FROM user_passport WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("domain"), _domainN);
						
						Assert.assertEquals(rs.getInt("status"), _statusN);
						Assert.assertEquals(rs.getInt("safe_status"), _safeStatusN);
						Assert.assertEquals(rs.getString("password_md5"), _passwordMd5N);
						Assert.assertEquals(rs.getString("account"), _accountN);
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
			MyUserPassportInfo mupir2=upa.getUserPassport(_id);
			Assert.assertEquals(mupir2.getId(), _id);
			Assert.assertEquals(mupir2.getDomain(), _domainN);
			Assert.assertEquals(mupir2.getStatus(), _statusN);
			Assert.assertEquals(mupir2.getSafeStatus(), _safeStatusN);
			Assert.assertEquals(mupir2.getPasswordMd5(), _passwordMd5N);
			Assert.assertEquals(mupir2.getAccount(), _accountN);
			
			//Assert.assertEquals(upa.getUserIdByEmailAndDomain(_accountN,_domainN),_id);
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	
	private static class MyUserPassportInfo extends UserPassportInfo {

	}
	
	private static class MyUserPassportFactory implements UserPassportFactory<MyUserPassportInfo> {
		public MyUserPassportInfo create() {
			return new MyUserPassportInfo();
		}
	}	
}