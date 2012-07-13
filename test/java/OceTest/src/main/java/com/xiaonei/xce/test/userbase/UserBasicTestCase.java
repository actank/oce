/**
 * 测试目的：测试UserBasicAdapter的功能
 * 测试环境：在服务端UserBaseReader/UserBaseWriter/BatchUpdater服务各启动一份
 * 通过标准：Get的数据和参数给定的数据一致
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userbasic.UserBasicAdapter;
import com.xiaonei.xce.userpassport.UserPassportAdapter;
import com.xiaonei.xce.userstate.UserStateAdapter;
import com.xiaonei.xce.userurl.UserUrlAdapter;
import com.xiaonei.xce.username.UserNameAdapter;
import com.xiaonei.xce.userborn.UserBornAdapter;
import com.xiaonei.xce.userstage.UserStageAdapter;
import com.xiaonei.xce.userdoing.UserDoingAdapter;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.util.tools.DateFormatter;
import junit.framework.Assert;

import com.xiaonei.xce.XceAdapter;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

@RunWith(Parameterized.class)
public class UserBasicTestCase {
	private static int testCount = 0;
	private final MUserBasicInfo _userBasicInfo;
	private final MUserPassportInfo _userPassportInfo;
	private final MUserStateInfo _userStateInfo;
	private final MUserUrlInfo _userUrlInfo;
	private final MUserNameInfo _userNameInfo;
	private final MUserBornInfo _userBornInfo;
	private final MUserStageInfo _userStageInfo;
	private final MUserDoingInfo _userDoingInfo;
	
	private MUserBasicFactory _userBasicFactory;
	private MUserPassportFactory _userPassportFactory;
	private MUserStateFactory _userStateFactory;
	private MUserUrlFactory _userUrlFactory;
	private MUserNameFactory _userNameFactory;
	private MUserBornFactory _userBornFactory;
	private MUserStageFactory _userStageFactory;
	private MUserDoingFactory _userDoingFactory;
	
	private UserBasicAdapter<MUserBasicInfo> _userBasicAdapter;
	private UserPassportAdapter<MUserPassportInfo> _userPassportAdapter;
	private UserStateAdapter<MUserStateInfo> _userStateAdapter;
	private UserUrlAdapter<MUserUrlInfo> _userUrlAdapter;
	private UserNameAdapter<MUserNameInfo> _userNameAdapter;
	private UserBornAdapter<MUserBornInfo> _userBornAdapter;
	private UserStageAdapter<MUserStageInfo> _userStageAdapter;
	private UserDoingAdapter<MUserDoingInfo> _userDoingAdapter;
	
	private final static String DB_USER_PASSPORT      = "user_passport";
	private final static String DB_USER_STATE	      = "user_state";
	private final static String DB_USER_URL           = "user_url";
	private final static String DB_USER_NAME          = "user_names";
	private final static String DB_USER_BORN          = "user_born";
	private final static String DB_USER_STAGE         = "user_stage";
	private final static String DB_USER_DOING         = "user_doing";

	private final static String TABLE_USER_PASSPORT   = "user_passport";
	private final static String TABLE_USER_STATE      = "user_state";
	private final static String TABLE_USER_URL        = "user_url";
	private final static String TABLE_USER_NAME       = "user_names";
	private final static String TABLE_USER_BORN       = "user_born";
	private final static String TABLE_USER_STAGE      = "user_stage";
	private final static String TABLE_USER_DOING      = "user_doing";

	public UserBasicTestCase(int id, String name, int univ, int status, String gender, 
			String homeProvince, String homeCity, String cityCode, int birthYear, int birthMonth, 
			int birthDay, boolean isLunarBirth, String astrology, int star, String headUrl, 
			String tinyUrl, String doing, String doingTime, int stage) {
		//初始化InfoFactory/Adapter
		//-----------------------------------------------
		_userBasicFactory = new MUserBasicFactory();
		_userBasicAdapter = new UserBasicAdapter<MUserBasicInfo>(_userBasicFactory);
		_userPassportFactory = new MUserPassportFactory();
		_userPassportAdapter = new UserPassportAdapter<MUserPassportInfo>(_userPassportFactory);
		_userStateFactory = new MUserStateFactory();
		_userStateAdapter = new UserStateAdapter<MUserStateInfo>(_userStateFactory);
		_userUrlFactory = new MUserUrlFactory();
		_userUrlAdapter = new UserUrlAdapter<MUserUrlInfo>(_userUrlFactory);
		_userNameFactory = new MUserNameFactory();
		_userNameAdapter = new UserNameAdapter<MUserNameInfo>(_userNameFactory);
		_userBornFactory = new MUserBornFactory();
		_userBornAdapter = new UserBornAdapter<MUserBornInfo>(_userBornFactory);
		_userStageFactory = new MUserStageFactory();
		_userStageAdapter = new UserStageAdapter<MUserStageInfo>(_userStageFactory);
		_userDoingFactory = new MUserDoingFactory();
		_userDoingAdapter = new UserDoingAdapter<MUserDoingInfo>(_userDoingFactory);
		
		//根据参数初始化_userBasicInfo
		//-----------------------------------------------
		_userBasicInfo = _userBasicFactory.create();
		_userBasicInfo.setId(id);
		_userBasicInfo.setName(name);
		_userBasicInfo.setUniv(univ);
		_userBasicInfo.setStatus(status);
		_userBasicInfo.setGender(gender);
		_userBasicInfo.setHomeProvince(homeProvince);
		_userBasicInfo.setHomeCity(homeCity);
		_userBasicInfo.setCityCode(cityCode);
		_userBasicInfo.setBirthYear(birthYear);
		_userBasicInfo.setBirthMonth(birthMonth);
		_userBasicInfo.setBirthDay(birthDay);
		_userBasicInfo.setIsLunarBirth(isLunarBirth);
		_userBasicInfo.setAstrology(astrology);
		_userBasicInfo.setStar(star);
		_userBasicInfo.setHeadUrl(headUrl);
		_userBasicInfo.setTinyUrl(tinyUrl);
		_userBasicInfo.setDoing(doing);
		_userBasicInfo.setDoingTime(DateFormatter.parse(doingTime));
		_userBasicInfo.setStage(stage);

		//根据参数初始化其他用来初始化缓存的的数据
		//-----------------------------------------------
		_userPassportInfo = _userPassportFactory.create();
		_userPassportInfo.setId(id);
		_userPassportInfo.setStatus(status);
		
		_userStateInfo = _userStateFactory.create();
		_userStateInfo.setId(id);
		_userStateInfo.setStar(star);
		
		_userUrlInfo = _userUrlFactory.create();
		_userUrlInfo.setId(id);
		_userUrlInfo.setHeadUrl(headUrl);
		_userUrlInfo.setTinyUrl(tinyUrl);
		
		_userNameInfo = _userNameFactory.create();
		_userNameInfo.setId(id);
		_userNameInfo.setName(name);
		
		_userBornInfo = _userBornFactory.create();
		_userBornInfo.setId(id);
		_userBornInfo.setGender(gender);
		_userBornInfo.setBirthDay(birthDay);
		_userBornInfo.setBirthMonth(birthMonth);
		_userBornInfo.setBirthYear(birthYear);
		_userBornInfo.setIsLunarBirth(isLunarBirth);
		_userBornInfo.setHomeProvince(homeProvince);
		_userBornInfo.setHomeCity(homeCity);
		_userBornInfo.setCityCode(cityCode);
		_userBornInfo.setAstrology(astrology);	
		
		_userStageInfo = _userStageFactory.create();
		_userStageInfo.setId(id);
		_userStageInfo.setStage(stage);
		_userStageInfo.setUniv(univ);
		
		_userDoingInfo = _userDoingFactory.create();
		_userDoingInfo.setId(id);
		_userDoingInfo.setDoing(doing);
		_userDoingInfo.setDoingTime(DateFormatter.parse(doingTime));

	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				/*
					int id, String name, int univ, int status, String gender, 
					String homeProvince, String homeCity, String cityCode, int birthYear, int birthMonth, 
					int birthDay, boolean isLunarBirth, String astrology, int star, String headUrl, 
					String tinyUrl, String doing, Date doingTime, int stage	
				*/
				{
					800004444, "name", 8, 8, "gender", 
					"homeProvince", "homeCity", "cityCode", 1988,  8,
					 8, true, "astrology", 8,  "headUrl",
					 "tinyUrl", "doing", "2011-09-06 18:00:00", 8
				},		
			});
	}

	@Before
	public void setUpBefore() throws Exception {
		System.out.println(++testCount + " : setUpBefore() ");
		System.out.println("================================");

	}

	@After
	public void tearDownAfter() throws Exception {
		
	}
	
	/**
	 * 本条测试同时测试get和create方法
	 * get和create方法是测试缓存是否正确的依赖条件
	 * 由于没有reload方法，所以用create方法来控制
	 */
	@Test
	public void testBase() {
		cleanDBByUserId(_userBasicInfo.getId());
		createViaCache();

		try {
			Thread.sleep(10000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		Assert.assertTrue(isCacheCorrect(_userBasicInfo));
	}
	
	protected void createViaCache(){
		try {
			_userPassportAdapter.createUserPassport(_userPassportInfo.getId(), _userPassportInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		try {
			_userStateAdapter.createUserState(_userStateInfo.getId(), _userStateInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		try {
			_userUrlAdapter.createUserUrl(_userUrlInfo.getId(), _userUrlInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		try {
			_userNameAdapter.createUserName(_userNameInfo.getId(), _userNameInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		try {
			_userBornAdapter.createUserBorn(_userBornInfo.getId(), _userBornInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		try {
			_userStageAdapter.createUserStage(_userStageInfo.getId(), _userStageInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		try {
			_userDoingAdapter.createUserDoing(_userDoingInfo.getId(), _userDoingInfo);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
	}

	/**
	 * 以info作为标准，判断Cache数据是否正确
	 * 
	 * @param info
	 * @return
	 */
	protected boolean isCacheCorrect(MUserBasicInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info.toString());
		
		MUserBasicInfo infoOfCache = null;		
		try {
			infoOfCache = _userBasicAdapter.getUserBasic(info.getId());
		} catch (Exception e) {
			System.out.println(e.toString());
		}
		
		System.out.println("\ninfoOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infoOfCache.toString());
		
		return isEqual(infoOfCache, info);
	}

	protected void cleanDBByUserId(int userId) {
		cleanDBByTable(userId, DB_USER_PASSPORT, TABLE_USER_PASSPORT);
		cleanDBByTable(userId, DB_USER_STATE, TABLE_USER_STATE);
		cleanDBByTable(userId, DB_USER_URL, TABLE_USER_URL);
		cleanDBByTable(userId, DB_USER_NAME, TABLE_USER_NAME);
		cleanDBByTable(userId, DB_USER_BORN, TABLE_USER_BORN);
		cleanDBByTable(userId, DB_USER_STAGE, TABLE_USER_STAGE);
		cleanDBByTable(userId, DB_USER_DOING, TABLE_USER_DOING);
	}

	protected void cleanDBByTable(int userId, String dbName, String tableName) {
		String sql = "delete from " + tableName + " where id=" + userId;
		writeDB(sql, dbName);
	}	
	
	protected MUserBasicInfo readDBByUserId(int userId) {
		MUserBasicInfo result = _userBasicFactory.create();
		readDBByTable(userId, DB_USER_PASSPORT, TABLE_USER_PASSPORT, result);
		readDBByTable(userId, DB_USER_STATE, TABLE_USER_STATE, result);
		readDBByTable(userId, DB_USER_URL, TABLE_USER_URL, result);
		readDBByTable(userId, DB_USER_NAME, TABLE_USER_NAME, result);
		readDBByTable(userId, DB_USER_BORN, TABLE_USER_BORN, result);
		readDBByTable(userId, DB_USER_STAGE, TABLE_USER_STAGE, result);
		readDBByTable(userId, DB_USER_DOING, TABLE_USER_DOING, result);
		return result;
	}

	protected void readDBByTable(int userId, String dbName, String tableName, MUserBasicInfo info) {
		String sql = "select * from " + tableName + " where id=" + userId;
		readDB(sql, dbName, tableName, info);
	}

	protected void getInfo(ResultSet rs, MUserBasicInfo info, String tableName) {
		try {
			if(tableName.equals(TABLE_USER_PASSPORT)){
				while (rs.next()) {
					info.setStatus(rs.getInt("status"));
					break;
				}
			}else if(tableName.equals(TABLE_USER_STATE)){
				while (rs.next()) {
					info.setStar(rs.getInt("star"));
					break;
				}
			}else if(tableName.equals(TABLE_USER_URL)){
				while (rs.next()) {
					info.setHeadUrl(rs.getString("headurl"));
					info.setTinyUrl(rs.getString("tinyurl"));
					break;
				}
			}else if(tableName.equals(TABLE_USER_NAME)){
				while (rs.next()) {
					info.setName(rs.getString("name"));
					break;
				}
			}else if(tableName.equals(TABLE_USER_BORN)){
				while (rs.next()) {
					info.setId(rs.getInt("id"));
					info.setGender(rs.getString("gender"));
					info.setBirthDay(rs.getInt("birth_day"));
					info.setBirthMonth(rs.getInt("birth_month"));
					info.setBirthYear(rs.getInt("birth_year"));
					info.setHomeProvince(rs.getString("home_province"));
					info.setHomeCity(rs.getString("home_city"));
					info.setCityCode(rs.getString("city_code"));
					info.setAstrology(rs.getString("astrology"));					
					break;
				}
			}else if(tableName.equals(TABLE_USER_STAGE)){
				while (rs.next()) {
					info.setUniv(rs.getInt("univ"));
					info.setStage(rs.getInt("stage"));
					break;
				}
			}else if(tableName.equals(TABLE_USER_DOING)){
				while (rs.next()) {
					info.setDoing(rs.getString("doing"));
					info.setDoingTime(DateFormatter.parse(rs.getString("doingtime")));
					break;
				}
			}
		} catch (SQLException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}

	protected void readDB(String sql, String dbName, String tableName, MUserBasicInfo info) {
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(dbName);
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					getInfo(rs, info, tableName);
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
	}
	
	protected void writeDB(String sql, String dbName) {
		System.out.println(sql);
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(dbName);
			try {
				Statement s = c.createStatement();
				try {
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
	
	protected boolean isEqual(MUserBasicInfo info1, MUserBasicInfo info2) {
		return info1.toString().equals(info2.toString());
	}
}