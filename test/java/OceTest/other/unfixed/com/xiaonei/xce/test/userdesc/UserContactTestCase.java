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
import com.xiaonei.xce.usercontact.UserContactAdapter;
import com.xiaonei.xce.usercontact.UserContactFactory;
import com.xiaonei.xce.usercontact.UserContactInfo;

@RunWith (Parameterized.class)
public class UserContactTestCase {
	private int _id;
	private String _msnO;
	private String _mobilephoneO;
	private String _fixphoneO;
	private String _homepageO;
	private int _qqO;
	private String _msnN;
	private String _mobilephoneN;
	private String _fixphoneN;
	private String _homepageN;
	private int _qqN;
	public UserContactTestCase(int id, String msnO, String mobilephoneO, String fixphoneO, String homepageO,
			int qqO, String msnN, String mobilephoneN, String fixphoneN, String homepageN, int qqN){
		_id=id;
		_msnO=msnO;
		_mobilephoneO=mobilephoneO;
		_fixphoneO=fixphoneO;
		_homepageO=homepageO;
		_qqO=qqO;
		_msnN=msnN;
		_mobilephoneN=mobilephoneN;
		_fixphoneN=fixphoneN;
		_homepageN=homepageN;
		_qqN=qqN;
	}
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,"msn1","mibolephone1","fixphone1","homepage1",1,"msn2","mibolephone2","fixphone2","homepage2",2},
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
			Connection c=XceAdapter.getInstance().getWriteConnection("user_contact_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_contact where id="+_id;
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
		UserContactAdapter<MyUserContactInfo> uca=new UserContactAdapter<MyUserContactInfo>(new MyUserContactFactory());
		MyUserContactInfo muciw1=new MyUserContactInfo();
		muciw1.setId(_id);
		muciw1.setHomepage(_homepageO);
		muciw1.setMobilePhone(_mobilephoneO);
		muciw1.setFixPhone(_fixphoneO);
		muciw1.setMsn(_msnO);
		muciw1.setQq(_qqO);
		uca.createUserContact(_id, muciw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_contact_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, qq, msn, homepage, mobilephone, fixphone FROM user_contact WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("qq"), _qqO);
						Assert.assertEquals(rs.getString("msn"), _msnO);
						Assert.assertEquals(rs.getString("mobilephone"), _mobilephoneO);
						Assert.assertEquals(rs.getString("fixphone"), _fixphoneO);
						Assert.assertEquals(rs.getString("homepage"), _homepageO);
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
			MyUserContactInfo mucir1=uca.getUserContact(_id);
			Assert.assertEquals(_id,mucir1.getId(),_id);
			Assert.assertEquals(_qqO,mucir1.getQq());
			Assert.assertEquals(_mobilephoneO,mucir1.getMobilePhone());
			Assert.assertEquals(_fixphoneO,mucir1.getFixPhone());
			Assert.assertEquals(_msnO,mucir1.getMsn());
			Assert.assertEquals(_homepageO,mucir1.getHomepage());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserContactInfo muciw2=new MyUserContactInfo();
		muciw2.setHomepage(_homepageN);
		muciw2.setMobilePhone(_mobilephoneN);
		muciw2.setFixPhone(_fixphoneN);
		muciw2.setMsn(_msnN);
		muciw2.setQq(_qqN);
		uca.setUserContact(_id, muciw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_contact_new");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT id, qq, msn, homepage, mobilephone, fixphone FROM user_contact WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getInt("qq"), _qqN);
						Assert.assertEquals(rs.getString("msn"), _msnN);
						Assert.assertEquals(rs.getString("mobilephone"), _mobilephoneN);
						Assert.assertEquals(rs.getString("fixphone"), _fixphoneN);
						Assert.assertEquals(rs.getString("homepage"), _homepageN);
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
			MyUserContactInfo mucir2=uca.getUserContact(_id);
			Assert.assertEquals(_id,mucir2.getId(),_id);
			Assert.assertEquals(_qqN,mucir2.getQq());
			Assert.assertEquals(_mobilephoneN,mucir2.getMobilePhone());
			Assert.assertEquals(_fixphoneN,mucir2.getFixPhone());
			Assert.assertEquals(_msnN,mucir2.getMsn());
			Assert.assertEquals(_homepageN,mucir2.getHomepage());
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}		
	}
	
	private static class MyUserContactInfo extends UserContactInfo{
		
	}
	
	private static class MyUserContactFactory implements UserContactFactory<MyUserContactInfo>{
		public MyUserContactInfo create() {
			return new MyUserContactInfo();
		}
	}
}
