package com.xiaonei.xce.test.userdesc;

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
import com.xiaonei.xce.usertime.UserTimeAdapter;
import com.xiaonei.xce.usertime.UserTimeFactory;
import com.xiaonei.xce.usertime.UserTimeInfo;

@RunWith(Parameterized.class)
public class UserTimeTestCase {
	private int _id;
	private Date _registertimeO;
	private Date _activatetimeO;
	private Date _lastlogintimeO;
	private Date _lastlogouttimeO;
	private Date _registertimeN;
	private Date _activatetimeN;
	private Date _lastlogintimeN;
	private Date _lastlogouttimeN;

	public UserTimeTestCase(int id, Date registertimeO, Date activatetimeO,
			Date lastlogintimeO, Date lastlogouttimeO, Date registertimeN,
			Date activatetimeN, Date lastlogintimeN, Date lastlogouttimeN) {
		_id=id;
		_registertimeO = registertimeO;
		_activatetimeO = activatetimeO;
		_lastlogintimeO = lastlogintimeO;
		_lastlogouttimeO = lastlogouttimeO;
		_registertimeN = registertimeN;
		_activatetimeN = activatetimeN;
		_lastlogintimeN = lastlogintimeN;
		_lastlogouttimeN = lastlogouttimeN;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays
				.asList(new Object[][] { { 6813, new Date(1000), new Date(1000),new Date(1000),new Date(1000),new Date(2000),new Date(2000),new Date(2000),new Date(2000)}, });
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {

	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {

	}

	@Before
	public void setUpBefore() throws Exception {
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(
					"user_time_new");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "delete from user_time where id=" + _id;
					s.execute(sql);
				} catch (Exception e) {
					e.printStackTrace();
					Assert.assertFalse(true);
				} finally {
					s.close();
				}
			} catch (Exception e) {
				e.printStackTrace();
				Assert.assertFalse(true);
			} finally {
				c.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}

	@After
	public void tearDownAfter() throws Exception {

	}

	@Test
	public void test() {
		UserTimeAdapter<MyUserTimeInfo> uta = new UserTimeAdapter<MyUserTimeInfo>(
				new MyUserTimeFactory());
		MyUserTimeInfo mutiw1 = new MyUserTimeInfo();
		mutiw1.setId(_id);
		mutiw1.setActivateTime(_activatetimeO);
		mutiw1.setLastLoginTime(_lastlogintimeO);
		mutiw1.setLastLogoutTime(_lastlogouttimeO);
		mutiw1.setRegisterTime(_registertimeO);
		uta.createUserTime(_id, mutiw1);
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(
					"user_time_new");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "SELECT id, registertime, lastlogintime, lastlogouttime, activatetime FROM user_time WHERE id="
							+ _id;
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						Assert.assertEquals(rs.getTime("registertime")
								.getTime(), _registertimeO.getTime());
						Assert.assertEquals(rs.getTime("lastlogintime")
								.getTime(), _lastlogintimeO.getTime());
						Assert.assertEquals(rs.getTime("lastlogouttime")
								.getTime(), _lastlogouttimeO.getTime());
						Assert.assertEquals(rs.getTime("activatetime")
								.getTime(), _activatetimeO.getTime());
					}
					rs.close();
				} catch (Exception e) {
					e.printStackTrace();
					Assert.assertFalse(true);
				} finally {
					s.close();
				}
			} catch (Exception e) {
				e.printStackTrace();
				Assert.assertFalse(true);
			} finally {
				c.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try {
			MyUserTimeInfo mutir1 = uta.getUserTime(_id);
			Assert.assertEquals(_id, mutir1.getId());
			Assert.assertEquals(_registertimeO, mutir1.getRegisterTime());
			Assert.assertEquals(_activatetimeO, mutir1.getActivateTime());
			Assert.assertEquals(_lastlogintimeO, mutir1.getLastLoginTime());
			Assert.assertEquals(_lastlogouttimeO, mutir1.getLastLogoutTime());
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserTimeInfo mutiw2 = new MyUserTimeInfo();
		mutiw2.setActivateTime(_activatetimeN);
		mutiw2.setLastLoginTime(_lastlogintimeN);
		mutiw2.setLastLogoutTime(_lastlogouttimeN);
		mutiw2.setRegisterTime(_registertimeN);
		uta.setUserTime(_id, mutiw2);
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(
					"user_time_new");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "SELECT id, registertime, lastlogintime, lastlogouttime, activatetime FROM user_time WHERE id="
							+ _id;
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						Assert.assertEquals(rs.getTime("registertime")
								.getTime(), _registertimeN.getTime());
						Assert.assertEquals(rs.getTime("lastlogintime")
								.getTime(), _lastlogintimeN.getTime());
						Assert.assertEquals(rs.getTime("lastlogouttime")
								.getTime(), _lastlogouttimeN.getTime());
						Assert.assertEquals(rs.getTime("activatetime")
								.getTime(), _activatetimeN.getTime());
					}
					rs.close();
				} catch (Exception e) {
					e.printStackTrace();
					Assert.assertFalse(true);
				} finally {
					s.close();
				}
			} catch (Exception e) {
				e.printStackTrace();
				Assert.assertFalse(true);
			} finally {
				c.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try {
			MyUserTimeInfo mutir2 = uta.getUserTime(_id);
			Assert.assertEquals(_id, mutir2.getId());
			Assert.assertEquals(_registertimeN, mutir2.getRegisterTime());
			Assert.assertEquals(_activatetimeN, mutir2.getActivateTime());
			Assert.assertEquals(_lastlogintimeN, mutir2.getLastLoginTime());
			Assert.assertEquals(_lastlogouttimeN, mutir2.getLastLogoutTime());
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}

	private static class MyUserTimeInfo extends UserTimeInfo {

	}

	private static class MyUserTimeFactory implements
			UserTimeFactory<MyUserTimeInfo> {
		public MyUserTimeInfo create() {
			return new MyUserTimeInfo();
		}
	}
}
