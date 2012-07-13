/**
 * 测试目的：测试UserRightAdapter的功能
 * 测试环境：在服务端UserBaseReader/UserBaseWriter服务各启动一份
 * 通过标准：Get/DB的数据一致；Create/Get/DB的数据一致；Set/Get/DB的数据一致
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userright.UserRightAdapter;

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
public class UserRightTestCase {
	private static int testCount = 0;
	private final static String DB_NAME = "user_right";
	private final static String TABLE_NAME = "user_right";
	private final MUserRightInfo _info;
	private MUserRightFactory _factory;
	private UserRightAdapter<MUserRightInfo> _adapter;

	public UserRightTestCase(int id, int auth, int common_rights) {
		_factory = new MUserRightFactory();
		_adapter = new UserRightAdapter<MUserRightInfo>(_factory);
		_info = _factory.create();
		
		_info.setId(id);
		_info.setAuth(auth);
		_info.setRights(common_rights);
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param step 改变数据的增量，没实在意义，只为了得到不同的数据
	 */
	protected MUserRightInfo genInfoCopy(int intStep, String strStep) {
		MUserRightInfo result = _factory.create();

		result.setId(_info.getId());
		result.setAuth(_info.getAuth() + intStep);
		result.setRights(_info.getRights() + intStep);

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{ 800004444, 0, 12 },		
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
		MUserRightInfo info = genInfoCopy(0, "");
		cleanDBByUserId(info.getId());	
		try {
			_adapter.createUserRight(info.getId(), info);
		} catch (Ice.UnknownException e) {
			//测试环境没有UserLogic服务，会抛异常
			System.out.println(e.toString());
		}

		Assert.assertTrue(isCacheCorrect(info));
	}
	
	@Test
	public void testSetUserRight() {
		MUserRightInfo info = genInfoCopy(0, "");
		cleanDBByUserId(info.getId());	
		try {
			_adapter.createUserRight(info.getId(), info);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		
		MUserRightInfo infoSet = genInfoCopy(1, "-set");
		try {
			_adapter.setUserRight(infoSet.getId(), infoSet);
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
	protected boolean isDBCorrect(MUserRightInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info);
		
		MUserRightInfo infoOfDB = readDBByUserId(info.getId());
		
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
	protected boolean isCacheCorrect(MUserRightInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info);
		
		MUserRightInfo infoOfCache = null;		
		try {
			infoOfCache = _adapter.getUserRight(info.getId());
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

	protected MUserRightInfo readDBByUserId(int userId) {
		String sql = "select * from " + TABLE_NAME + " where id=" + userId;
		return readDB(sql);
	}

	protected MUserRightInfo getInfo(ResultSet rs) {
		MUserRightInfo result = _factory.create();
		try {
			while (rs.next()) {
				result.setId(rs.getInt("id"));
				result.setAuth(rs.getInt("auth"));
				result.setRights(rs.getInt("common_rights"));
				
				break; // 正常只有一个结果，如果有多个，只取其中的第一个
			}
		} catch (SQLException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		return result;
	}

	protected MUserRightInfo readDB(String sql) {
		MUserRightInfo result = _factory.create();
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
	
	protected boolean isEqual(MUserRightInfo info1, MUserRightInfo info2) {
		return info1.toString().equals(info2.toString());
	}
}