/**
 * 测试目的：测试UserPassportAdapter的功能
 * 测试环境：在服务端UserBaseReader/UserBaseWriter服务各启动一份
 * 通过标准：Get/DB的数据一致；Create/Get/DB的数据一致；Set/Get/DB的数据一致
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userpassport.UserPassportAdapter;

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
public class UserPassportTestCase {
	private static int testCount = 0;
	private final static String DB_NAME = "user_passport";
	private final static String TABLE_NAME = "user_passport";
	private final MUserPassportInfo _info;
	private MUserPassportFactory _factory;
	private UserPassportAdapter<MUserPassportInfo> _adapter;

	public UserPassportTestCase(int id, String account, String domain, String password_md5, int status, int safe_status) {
		_factory = new MUserPassportFactory();
		_adapter = new UserPassportAdapter<MUserPassportInfo>(_factory);
		_info = _factory.create();
		
		_info.setId(id);
		_info.setAccount(account);
		_info.setDomain(domain);
		_info.setPasswordMd5(password_md5);
		_info.setStatus(status);
		_info.setSafeStatus(safe_status);
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param step 改变数据的增量，没实在意义，只为了得到不同的数据
	 */
	protected MUserPassportInfo genInfoCopy(int intStep, String strStep) {
		MUserPassportInfo result = _factory.create();

		result.setId(_info.getId());
		result.setAccount(_info.getAccount() + strStep);
		result.setDomain(_info.getDomain() + strStep);
		result.setPasswordMd5(_info.getPasswordMd5()  + strStep);
		result.setStatus(_info.getStatus() + intStep);
		result.setSafeStatus(_info.getSafeStatus() + intStep);

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
				{ 800004444, "account", "domain", "password_md5", 0, 0 },		
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
		MUserPassportInfo info = genInfoCopy(0, "");
		cleanDBByUserId(info.getId());	
		try {
			_adapter.createUserPassport(info.getId(), info);
		} catch (Ice.UnknownException e) {
			//测试环境没有UserLogic服务，会抛异常
			System.out.println(e.toString());
		}

		Assert.assertTrue(isCacheCorrect(info));
	}
	
	@Test
	public void testSetUserPassport() {
		MUserPassportInfo info = genInfoCopy(0, "");
		cleanDBByUserId(info.getId());	
		try {
			_adapter.createUserPassport(info.getId(), info);
		} catch (Ice.UnknownException e) {
			System.out.println(e.toString());
		}
		
		MUserPassportInfo infoSet = genInfoCopy(1, "-set");
		try {
			_adapter.setUserPassport(infoSet.getId(), infoSet);
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
	protected boolean isDBCorrect(MUserPassportInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info);
		
		MUserPassportInfo infoOfDB = readDBByUserId(info.getId());
		
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
	protected boolean isCacheCorrect(MUserPassportInfo info) {
		System.out.println("\ninfo:");
		System.out.println("--------------------------------");
		System.out.println(info);
		
		MUserPassportInfo infoOfCache = null;		
		try {
			infoOfCache = _adapter.getUserPassport(info.getId());
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

	protected MUserPassportInfo readDBByUserId(int userId) {
		String sql = "select * from " + TABLE_NAME + " where id=" + userId;
		return readDB(sql);
	}

	protected MUserPassportInfo getInfo(ResultSet rs) {
		MUserPassportInfo result = _factory.create();
		try {
			while (rs.next()) {
				result.setId(rs.getInt("id"));
				result.setAccount(rs.getString("account"));
				result.setDomain(rs.getString("domain"));
				result.setPasswordMd5(rs.getString("password_md5"));
				result.setStatus(rs.getInt("status"));
				result.setSafeStatus(rs.getInt("safe_status"));
				
				break; // 正常只有一个结果，如果有多个，只取其中的第一个
			}
		} catch (SQLException e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		return result;
	}

	protected MUserPassportInfo readDB(String sql) {
		MUserPassportInfo result = _factory.create();
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
	
	protected boolean isEqual(MUserPassportInfo info1, MUserPassportInfo info2) {
		return info1.toString().equals(info2.toString());
	}
}