/**
 * 测试目的：测试UserBornAdapter的功能
 * 测试环境：在服务端UserBaseReader/UserBaseWriter服务各启动一份
 * 通过标准：Get/DB的数据一致；Create/Get/DB的数据一致；Set/Get/DB的数据一致
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userborn.UserBornAdapter;

import java.util.Arrays;
import java.util.Collection;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import junit.framework.Assert;

import com.xiaonei.xce.XceAdapter;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

@RunWith(Parameterized.class)
public class UserBornTestCase {
	private static int testCount = 0;
	private final static String DB_NAME = "user_born";
	private final static String TABLE_NAME = "user_born";
	private final MUserBornInfo _info;
	private MUserBornFactory _factory;
	private UserBornAdapter<MUserBornInfo> _adapter;

	public UserBornTestCase(int id, int birth_day, int birth_month, int birth_year, 
			String astrology, String gender, String home_province, String home_city, String city_code, boolean is_lunar_birth) {
		_factory = new MUserBornFactory();
		_adapter = new UserBornAdapter<MUserBornInfo>(_factory);
		_info = _factory.create();
		
		_info.setId(id);
		_info.setBirthDay(birth_day);
		_info.setBirthMonth(birth_month);
		_info.setBirthYear(birth_year);
		_info.setAstrology(astrology);
		_info.setGender(gender);
		_info.setHomeProvince(home_province);
		_info.setHomeCity(home_city);
		_info.setCityCode(city_code);
		_info.setIsLunarBirth(is_lunar_birth);
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param step 改变数据的增量，没实在意义，只为了得到不同的数据
	 */
	protected MUserBornInfo genInfoCopy(int intStep, String strStep) {
		MUserBornInfo result = _factory.create();

		result.setId(_info.getId());
		result.setBirthDay(_info.getBirthDay() + intStep);
		result.setBirthMonth(_info.getBirthMonth() + intStep);
		result.setBirthYear(_info.getBirthYear() + intStep);
		result.setAstrology(_info.getAstrology());
		result.setGender(_info.getGender() + strStep);
		result.setHomeProvince(_info.getHomeProvince() + strStep);
		result.setHomeCity(_info.getHomeCity() + strStep);
		result.setCityCode(_info.getCityCode());
		result.setIsLunarBirth(_info.isLunarBirth());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{ 800004444, 8, 8, 1988, "astrology", "gender", "home_province", "home_city", "city_code", false },		
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
		MUserBornInfo info = genInfoCopy(0, "");
		cleanDBByUserId(info.getId());	
		try {
			_adapter.createUserBorn(info.getId(), info);
		} catch (Ice.UnknownException e) {
			//测试环境没有UserLogic服务，会抛异常
			System.out.println(e.toString());
		}

		Assert.assertTrue(isCacheCorrect(info));
	}
	
	@Test
	public void testSetUserBorn() {
		MUserBornInfo info = genInfoCopy(0, "");
		cleanDBByUserId(info.getId());	
		try {
			_adapter.createUserBorn(info.getId(), info);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		
		MUserBornInfo infoSet = genInfoCopy(1, "-set");
		try {
			_adapter.setUserBorn(infoSet.getId(), infoSet);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		Assert.assertTrue(isDBCorrect(infoSet));
		Assert.assertTrue(isCacheCorrect(infoSet));
	}
	
	/**
	 * 以info作为标准，判断DB数据是否正确
	 * 
	 * @param info
	 * @return
	 */
	protected boolean isDBCorrect(MUserBornInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info);
		
		MUserBornInfo infoOfDB = readDBByUserId(info.getId());
		
		System.out.println("\ninfoOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infoOfDB);
		
		return isEqual(infoOfDB, info);
	}

	/**
	 * 以info作为标准，判断Cache数据是否正确
	 * 
	 * @param info
	 * @return
	 */
	protected boolean isCacheCorrect(MUserBornInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info);
		
		MUserBornInfo infoOfCache = null;		
		try {
			infoOfCache = _adapter.getUserBorn(info.getId());
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		System.out.println("\ninfoOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infoOfCache);
		
		return isEqual(infoOfCache, info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from " + TABLE_NAME + " where id=" + userId;
		writeDB(sql);
	}

	protected MUserBornInfo readDBByUserId(int userId) {
		String sql = "select * from " + TABLE_NAME + " where id=" + userId;
		return readDB(sql);
	}

	protected MUserBornInfo getInfo(ResultSet rs) {
		MUserBornInfo result = _factory.create();
		try {
			while (rs.next()) {
				result.setId(rs.getInt("id"));
				result.setBirthDay(rs.getInt("birth_day"));
				result.setBirthMonth(rs.getInt("birth_month"));
				result.setBirthYear(rs.getInt("birth_year"));
				result.setAstrology(rs.getString("astrology"));
				result.setGender(rs.getString("gender"));
				result.setHomeProvince(rs.getString("home_province"));
				result.setHomeCity(rs.getString("home_city"));
				result.setCityCode(rs.getString("city_code"));
				int isLunarBirth = rs.getInt("is_lunar_birth");
				if(isLunarBirth == 0){
					result.setIsLunarBirth(false);
				}else{
					result.setIsLunarBirth(true);
				}
				
				break; // 正常只有一个结果，如果有多个，只取其中的第一个
			}
		} catch (SQLException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		return result;
	}

	protected MUserBornInfo readDB(String sql) {
		MUserBornInfo result = _factory.create();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection(DB_NAME);
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					result = getInfo(rs);
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
		return result;
	}
	
	protected void writeDB(String sql) {
		System.out.println(sql);
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection(DB_NAME);
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
	
	protected boolean isEqual(MUserBornInfo info1, MUserBornInfo info2) {
		return info1.toString().equals(info2.toString());
	}
}