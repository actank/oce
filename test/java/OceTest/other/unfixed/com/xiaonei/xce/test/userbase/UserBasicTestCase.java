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

import xce.userbase.UserBornManagerPrx;
import xce.userbase.UserBornManagerPrxHelper;
import xce.userbase.UserDoingManagerPrx;
import xce.userbase.UserDoingManagerPrxHelper;
import xce.userbase.UserStageManagerPrx;
import xce.userbase.UserStageManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectPrx;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.userbasic.UserBasicAdapter;
import com.xiaonei.xce.userbasic.UserBasicFactory;
import com.xiaonei.xce.userbasic.UserBasicInfo;
import com.xiaonei.xce.username.UserNameAdapter;
import com.xiaonei.xce.username.UserNameFactory;
import com.xiaonei.xce.username.UserNameInfo;
import com.xiaonei.xce.userpassport.UserPassportAdapter;
import com.xiaonei.xce.userpassport.UserPassportFactory;
import com.xiaonei.xce.userpassport.UserPassportInfo;
import com.xiaonei.xce.userstate.UserStateAdapter;
import com.xiaonei.xce.userstate.UserStateFactory;
import com.xiaonei.xce.userstate.UserStateInfo;
import com.xiaonei.xce.userurl.UserUrlAdapter;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;

@RunWith(Parameterized.class)
public class UserBasicTestCase {

	private int _id;
	private int _univO;
	private String _nameO;
	private int _statusO;
	private String _genderO;
	private int _birth_dayO;
	private int _birth_monthO;
	private int _birth_yearO;
	private String _home_provinceO;
	private String _home_cityO;
	private String _city_codeO;
	private String _astrologyO;
	private String _headurlO;
	private String _tinyurlO;
	private int _starO;
	private String _doingO;
	private Date _doingtimeO;
	private int _stageO;
	private int _univN;
	private String _nameN;
	private int _statusN;
	private String _genderN;
	private int _birth_dayN;
	private int _birth_monthN;
	private int _birth_yearN;
	private String _home_provinceN;
	private String _home_cityN;
	private String _city_codeN;
	private String _astrologyN;
	private String _headurlN;
	private String _tinyurlN;
	private int _starN;
	private String _doingN;
	private Date _doingtimeN;
	private int _stageN;
	private int _updateId;

	public UserBasicTestCase(int id, int univO, int selectedO, String nameO,
			int statusO, String genderO, int univ_yearO, String departmentO,
			String dormO, int birth_dayO, int birth_monthO, int birth_yearO,
			String home_provinceO, String home_cityO, String city_codeO,
			String high_schoolO, String high_school_codeO, String astrologyO,
			String headurlO, String tinyurlO, int basiccontrolO, int starO,
			String doingO, Date doingtimeO, int stageO, String roomO,
			int univN, int selectedN, String nameN, int statusN,
			String genderN, int univ_yearN, String departmentN, String dormN,
			int birth_dayN, int birth_monthN, int birth_yearN,
			String home_provinceN, String home_cityN, String city_codeN,
			String high_schoolN, String high_school_codeN, String astrologyN,
			String headurlN, String tinyurlN, int basiccontrolN, int starN,
			String doingN, Date doingtimeN, int stageN, String roomN,
			int updateId) {
		_id = id;
		_univO = univO;
		_nameO = nameO;
		_statusO = statusO;
		_genderO = genderO;
		_birth_dayO = birth_dayO;
		_birth_monthO = birth_monthO;
		_birth_yearO = birth_yearO;
		_home_provinceO = home_provinceO;
		_home_cityO = home_cityO;
		_city_codeO = city_codeO;
		_astrologyO = astrologyO;
		_headurlO = headurlO;
		_tinyurlO = tinyurlO;
		_starO = starO;
		_doingO = doingO;
		_doingtimeO = doingtimeO;
		_stageO = stageO;
		_univN = univN;
		_nameN = nameN;
		_statusN = statusN;
		_genderN = genderN;
		_birth_dayN = birth_dayN;
		_birth_monthN = birth_monthN;
		_birth_yearN = birth_yearN;
		_home_provinceN = home_provinceN;
		_home_cityN = home_cityN;
		_city_codeN = city_codeN;
		_astrologyN = astrologyN;
		_headurlN = headurlN;
		_tinyurlN = tinyurlN;
		_starN = starN;
		_doingN = doingN;
		_doingtimeN = doingtimeN;
		_stageN = stageN;
		_updateId = updateId;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 6813, 1, 1, "name1", 1,
				"gender1", 1, "department1", "dorm1", 1, 1, 1,
				"home_province1", "home_city1", "city_code1", "high_school1",
				"hsc1", "astrology1", "headurl1", "tinyur1", 1, 1, "doing1",
				new Date(1000), 1, "room1", 2, 2, "name2", 2, "gender2", 2,
				"department2", "dorm2", 2, 2, 2, "home_province2",
				"home_city2", "city_code2", "high_school2", "hsc2",
				"astrology2", "headurl2", "tinyur2", 2, 2, "doing2",
				new Date(2000), 2, "room2", 6814 }, });
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
					"user_passport");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "delete from user_passport where id=" + _id;
					s.execute(sql);
					sql = "delete from user_passport where id=" + _updateId;
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
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(
					"user_state");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "delete from user_state where id=" + _id;
					s.execute(sql);
					sql = "delete from user_state where id=" + _updateId;
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
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(
					"user_url");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "delete from user_url where id=" + _id;
					s.execute(sql);
					sql = "delete from user_url where id=" + _updateId;
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
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(
					"user_names");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "delete from user_names where id=" + _id;
					s.execute(sql);
					sql = "delete from user_names where id=" + _updateId;
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
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(
					"user_basic");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "delete from user_basic where id=" + _id;
					s.execute(sql);
					sql = "delete from user_basic where id=" + _updateId;
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

	void checkUpdateId() {
		UserBasicAdapter<MyUserBasicInfo> uba = new UserBasicAdapter<MyUserBasicInfo>(
				new MyUserBasicFactory());
		MyUserBasicInfo mubiw1 = new MyUserBasicInfo();
		mubiw1.setId(_updateId);
		uba.setUserBasic(_updateId, mubiw1);
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(
					"user_basic");
			try {
				Statement s = c.createStatement();
				try {
					int id = -1;
					String sql = "SELECT id FROM user_basic WHERE id="
							+ _updateId;
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						Assert.assertEquals(rs.getInt("id"), id);
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
			MyUserBasicInfo mubir = uba.getUserBasic(_id);
			Assert.assertEquals(mubir.getId(), _id);
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try {
			uba.getUserBasic(_updateId);
			Assert.assertFalse(true);
		} catch (Exception e) {
			Assert.assertTrue(true);
		}
	}

	@Test
	public void test() {
		UserBasicAdapter<MyUserBasicInfo> uba = new UserBasicAdapter<MyUserBasicInfo>(
				new MyUserBasicFactory());
		UserUrlAdapter<MyUserUrlInfo> uua=new UserUrlAdapter<MyUserUrlInfo>(new MyUserUrlFactory());
		UserPassportAdapter<MyUserPassportInfo> upa=new UserPassportAdapter<MyUserPassportInfo>(new MyUserPassportFactory());
		UserStateAdapter<MyUserStateInfo> usa=new UserStateAdapter<MyUserStateInfo>(new MyUserStateFactory());
		UserNameAdapter<MyUserNameInfo> una=new UserNameAdapter<MyUserNameInfo>(new MyUserNameFactory());
		MyUserBasicInfo mubiw1 = new MyUserBasicInfo();
		mubiw1.setId(_id);
		mubiw1.setAstrology(_astrologyO);
		mubiw1.setBirthDay(_birth_dayO);
		mubiw1.setBirthMonth(_birth_monthO);
		mubiw1.setBirthYear(_birth_yearO);
		mubiw1.setCityCode(_city_codeO);
		mubiw1.setDoing(_doingO);
		mubiw1.setDoingTime(_doingtimeO);
		mubiw1.setGender(_genderO);
		mubiw1.setHeadUrl(_headurlO);
		mubiw1.setHomeCity(_home_cityO);
		mubiw1.setHomeProvince(_home_provinceO);
		mubiw1.setName(_nameO);
		mubiw1.setStage(_stageO);
		mubiw1.setStar(_starO);
		mubiw1.setStatus(_statusO);
		mubiw1.setTinyUrl(_tinyurlO);
		mubiw1.setUniv(_univO);
		uba.createUserBasic(_id, mubiw1);
		MyUserPassportInfo mupiw1 = new MyUserPassportInfo();
		mupiw1.setId(_id);
		mupiw1.setStatus(_statusO);
		mupiw1.setDomain("domain"+_id);
		mupiw1.setAccount("account"+_id);
		upa.createUserPassport(_id, mupiw1);
		MyUserUrlInfo muuiw1=new MyUserUrlInfo();
		muuiw1.setId(_id);
		muuiw1.setTinyUrl(_tinyurlO);
		muuiw1.setHeadUrl(_headurlO);
		uua.createUserUrl(_id, muuiw1);
		MyUserStateInfo musiw1=new MyUserStateInfo();
		musiw1.setId(_id);
		musiw1.setStar(_starO);
		usa.createUserState(_id, musiw1);
		MyUserNameInfo muniw1=new MyUserNameInfo();
		muniw1.setId(_id);
		muniw1.setName(_nameO);
		una.createUserName(_id, muniw1);
		
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(
					"user_basic");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "SELECT * FROM user_basic WHERE id=" + _id;
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						Assert.assertEquals(rs.getInt("univ"), _univO);
						Assert.assertEquals(rs.getString("name"), _nameO);
						Assert.assertEquals(rs.getInt("status"), _statusO);
						Assert.assertEquals(rs.getString("gender"), _genderO);
						Assert
								.assertEquals(rs.getInt("birth_day"),
										_birth_dayO);
						Assert.assertEquals(rs.getInt("birth_month"),
								_birth_monthO);
						Assert.assertEquals(rs.getInt("birth_year"),
								_birth_yearO);
						Assert.assertEquals(rs.getString("home_province"),
								_home_provinceO);
						Assert.assertEquals(rs.getString("home_city"),
								_home_cityO);
						Assert.assertEquals(rs.getString("city_code"),
								_city_codeO);
						Assert.assertEquals(rs.getString("astrology"),
								_astrologyO);
						Assert.assertEquals(rs.getString("headurl"), _headurlO);
						Assert.assertEquals(rs.getString("tinyurl"), _tinyurlO);
						Assert.assertEquals(rs.getInt("star"), _starO);
						Assert.assertEquals(rs.getString("doing"), _doingO);
						Assert.assertEquals(rs.getTime("doingtime").getTime(),
								_doingtimeO.getTime());
						Assert.assertEquals(rs.getInt("stage"), _stageO);
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
			MyUserBasicInfo mubir1 = uba.getUserBasic(_id);
			Assert.assertEquals(mubir1.getAstrology(), _astrologyO);
			Assert.assertEquals(mubir1.getBirthDay(), _birth_dayO);
			Assert.assertEquals(mubir1.getBirthMonth(), _birth_monthO);
			Assert.assertEquals(mubir1.getBirthYear(), _birth_yearO);
			Assert.assertEquals(mubir1.getCityCode(), _city_codeO);
			Assert.assertEquals(mubir1.getDoing(), _doingO);
			Assert.assertEquals(mubir1.getGender(), _genderO);
			Assert.assertEquals(mubir1.getHeadUrl(), _headurlO);
			Assert.assertEquals(mubir1.getDoingTime().getTime(),_doingtimeO.getTime());
			Assert.assertEquals(mubir1.getHomeCity(), _home_cityO);
			Assert.assertEquals(mubir1.getHomeProvince(), _home_provinceO);
			Assert.assertEquals(mubir1.getName(), _nameO);
			Assert.assertEquals(mubir1.getStage(), _stageO);
			Assert.assertEquals(mubir1.getStar(), _starO);
			Assert.assertEquals(mubir1.getStatus(), _statusO);
			Assert.assertEquals(mubir1.getTinyUrl(), _tinyurlO);
			Assert.assertEquals(mubir1.getUniv(), _univO);
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		MyUserBasicInfo mubiw2 = new MyUserBasicInfo();
		mubiw2.setAstrology(_astrologyN);
		mubiw2.setBirthDay(_birth_dayN);
		mubiw2.setBirthMonth(_birth_monthN);
		mubiw2.setBirthYear(_birth_yearN);
		mubiw2.setCityCode(_city_codeN);
		mubiw2.setDoing(_doingN);
		mubiw2.setDoingTime(_doingtimeN);
		mubiw2.setGender(_genderN);
		mubiw2.setHeadUrl(_headurlN);
		mubiw2.setHomeCity(_home_cityN);
		mubiw2.setHomeProvince(_home_provinceN);
		mubiw2.setName(_nameN);
		mubiw2.setStage(_stageN);
		mubiw2.setStar(_starN);
		mubiw2.setStatus(_statusN);
		mubiw2.setTinyUrl(_tinyurlN);
		mubiw2.setUniv(_univN);
		uba.setUserBasic(_id, mubiw2);
		
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(
					"user_basic");
			try {
				Statement s = c.createStatement();
				try {
					String sql = "SELECT * FROM user_basic WHERE id=" + _id;
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						Assert.assertEquals(rs.getInt("univ"), _univN);
						Assert.assertEquals(rs.getString("name"), _nameN);
						Assert.assertEquals(rs.getInt("status"), _statusN);
						Assert.assertEquals(rs.getString("gender"), _genderN);
						Assert
								.assertEquals(rs.getInt("birth_day"),
										_birth_dayN);
						Assert.assertEquals(rs.getInt("birth_month"),
								_birth_monthN);
						Assert.assertEquals(rs.getInt("birth_year"),
								_birth_yearN);
						Assert.assertEquals(rs.getString("home_province"),
								_home_provinceN);
						Assert.assertEquals(rs.getString("home_city"),
								_home_cityN);
						Assert.assertEquals(rs.getString("city_code"),
								_city_codeN);
						Assert.assertEquals(rs.getString("astrology"),
								_astrologyN);
						Assert.assertEquals(rs.getString("headurl"), _headurlN);
						Assert.assertEquals(rs.getString("tinyurl"), _tinyurlN);
						Assert.assertEquals(rs.getInt("star"), _starN);
						Assert.assertEquals(rs.getString("doing"), _doingN);
						Assert.assertEquals(rs.getTime("doingtime").getTime(),
								_doingtimeN.getTime());
						Assert.assertEquals(rs.getInt("stage"), _stageN);
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
			MyUserBasicInfo mubir1 = uba.getUserBasic(_id);
			Assert.assertEquals(mubir1.getAstrology(), _astrologyN);
			Assert.assertEquals(mubir1.getBirthDay(), _birth_dayN);
			Assert.assertEquals(mubir1.getBirthMonth(), _birth_monthN);
			Assert.assertEquals(mubir1.getBirthYear(), _birth_yearN);
			Assert.assertEquals(mubir1.getCityCode(), _city_codeN);
			Assert.assertEquals(mubir1.getDoing(), _doingN);
			Assert.assertEquals(mubir1.getGender(), _genderN);
			Assert.assertEquals(mubir1.getHeadUrl(), _headurlO);
			Assert.assertEquals(mubir1.getHomeCity(), _home_cityN);
			Assert.assertEquals(mubir1.getHomeProvince(), _home_provinceN);
			Assert.assertEquals(mubir1.getName(), _nameO);
			Assert.assertEquals(mubir1.getStage(), _stageN);
			Assert.assertEquals(mubir1.getStar(), _starO);
			Assert.assertEquals(mubir1.getStatus(), _statusO);
			Assert.assertEquals(mubir1.getTinyUrl(), _tinyurlO);
			Assert.assertEquals(mubir1.getUniv(), _univN);
			Assert.assertEquals(mubir1.getDoingTime().getTime(), _doingtimeN.getTime());
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}

		checkUpdateId();
	}

	private static class MyUserBasicInfo extends UserBasicInfo {

		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;

	}

	private static class MyUserPassportInfo extends UserPassportInfo {

	}

	private static class MyUserNameInfo extends UserNameInfo {

	}

	private static class MyUserStateInfo extends UserStateInfo {

	}

	private static class MyUserUrlInfo extends UserUrlInfo {

	}

	private static class MyUserPassportFactory implements
			UserPassportFactory<MyUserPassportInfo> {

		public MyUserPassportInfo create() {
			// TODO Auto-generated method stub
			return new MyUserPassportInfo();
		}

	}

	private static class MyUserNameFactory implements
			UserNameFactory<MyUserNameInfo> {

		public MyUserNameInfo create() {
			// TODO Auto-generated method stub
			return new MyUserNameInfo();
		}

	}

	private static class MyUserStateFactory implements
			UserStateFactory<MyUserStateInfo> {

		public MyUserStateInfo create() {
			// TODO Auto-generated method stub
			return new MyUserStateInfo();
		}

	}

	private static class MyUserUrlFactory implements
			UserUrlFactory<MyUserUrlInfo> {

		public MyUserUrlInfo create() {
			// TODO Auto-generated method stub
			return new MyUserUrlInfo();
		}

	}

	private static class MyUserBasicFactory implements
			UserBasicFactory<MyUserBasicInfo> {
		public MyUserBasicInfo create() {
			return new MyUserBasicInfo();
		}
	}
}
