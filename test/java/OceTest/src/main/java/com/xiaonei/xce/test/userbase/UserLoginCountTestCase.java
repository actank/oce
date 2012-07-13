/**
 * 测试目的：测试UserLoginCountAdapter的功能
 * 测试环境：在服务端UserBaseReader/UserBaseWriter服务各启动一份；
 * 通过标准：Get/DB的数据一致；Create/Get/DB的数据一致；Set/Get/DB的数据一致。
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userlogincount.UserLoginCountAdapter;

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
public class UserLoginCountTestCase {
	private static int testCount = 0;
	private final static String DB_NAME = "user_logincount";
	private final static String TABLE_NAME = "user_logincount";
	private final int _id;
	private final int _logincount;
	private UserLoginCountAdapter _adapter;

	public UserLoginCountTestCase(int id) {
		_adapter = new UserLoginCountAdapter();
		_id = id;
		_logincount = 0;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{ 800004444 },		
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
		cleanDBByUserId(_id);	
		try {
			_adapter.createUserLoginCount(_id);
		} catch (Ice.UnknownException e) {
			//测试环境没有UserLogic服务，会抛异常
			System.out.println(e.toString());
		}

		Assert.assertTrue(isCacheCorrect(_logincount));
	}
	
	@Test
	public void testIncUserLoginCount() {
		cleanDBByUserId(_id);	
		try {
			_adapter.createUserLoginCount(_id);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		
		try {
			_adapter.incUserLoginCount(_id);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}		
		
		Assert.assertTrue(isCacheCorrect(_logincount + 1));
		Assert.assertTrue(isDBCorrect(_logincount + 1));
	}
	
	/**
	 * 以info作为标准，判断DB数据是否正确
	 * 
	 * @param info
	 * @return
	 */
	protected boolean isDBCorrect(int info) {
		System.out.println("\ninfo:" + info);
		
		//由于是异步update数据库，所以等待10s
		try {
			Thread.sleep(10000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		int infoOfDB = readDBByUserId(_id);
		
		System.out.println("\ninfoOfDB:" + infoOfDB);
		
		return (infoOfDB == info);
	}

	/**
	 * 以info作为标准，判断Cache数据是否正确
	 * 
	 * @param info
	 * @return
	 */
	protected boolean isCacheCorrect(int info) {
		System.out.println("\ninfo:" + info);
		
		int infoOfCache = 0;
		try {
			infoOfCache = _adapter.getUserLoginCount(_id);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		System.out.println("\ninfoOfCache:" + infoOfCache);
		
		return (infoOfCache == info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from " + TABLE_NAME + " where id=" + userId;
		writeDB(sql);
	}

	protected int readDBByUserId(int userId) {
		String sql = "select logincount from " + TABLE_NAME + " where id=" + userId;
		return readDB(sql);
	}

	protected int getInfo(ResultSet rs) {
		int result = 0;
		try {
			while (rs.next()) {
				result = rs.getInt("logincount");
				
				break; // 正常只有一个结果，如果有多个，只取其中的第一个
			}
		} catch (SQLException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		return result;
	}

	protected int readDB(String sql) {
		int result = 0;
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
	
}