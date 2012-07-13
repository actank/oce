package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.JuniorHighSchoolInfo;
import mop.hi.oce.adapter.AdapterFactory;
import com.xiaonei.xce.XceAdapter;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import junit.framework.Assert;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

@RunWith(Parameterized.class)
public class JuniorHighSchoolTestCase {
	private static int testCount = 0;
	private List<JuniorHighSchoolInfo> _infos;
	private final JuniorHighSchoolInfo _info;

	public JuniorHighSchoolTestCase(int id, int userId, int juniorHighSchoolId, String juniorHighSchoolName, int juniorHighSchoolYear) {
		_info = new JuniorHighSchoolInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setJunHighentarySchoolId(juniorHighSchoolId);
		_info.setJunHighentarySchoolName(juniorHighSchoolName);
		_info.setJunHighentarySchoolYear(juniorHighSchoolYear);
		_infos = new ArrayList<JuniorHighSchoolInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected JuniorHighSchoolInfo genInfoCopy(int idStep) {
		JuniorHighSchoolInfo result = new JuniorHighSchoolInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setJunHighentarySchoolId(_info.getJunHighentarySchoolId());
		result.setJunHighentarySchoolName(_info.getJunHighentarySchoolName());
		result.setJunHighentarySchoolYear(_info.getJunHighentarySchoolYear());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "juniorHighSchoolName", 2008 }, });
	}

	@Before
	public void setUpBefore() throws Exception {
		System.out.println(++testCount + " : setUpBefore() ");
	}

	@After
	public void tearDownAfter() throws Exception {

	}

	/**
	 * reload和get是最关键的方法 测试中对缓存的控制依赖于reload方法 测试中对缓存的判断依赖于get方法
	 */
	@Test
	public void testBase() {
		init();

		// Debug info
		List<JuniorHighSchoolInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<JuniorHighSchoolInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getJuniorHighSchoolInfoList(_info.getUserId());
		List<JuniorHighSchoolInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getJuniorHighSchoolInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

//	@Test
//	public void testGetJuniorHighSchoolInfoCount() {
//		init();
//
//		int countOfDB = readDBByUserId(_info.getUserId()).size();
//		int countOfCache = AdapterFactory.getNetworkAdapter().getJuniorHighSchoolInfoCount(_info.getUserId());
//		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getJuniorHighSchoolInfoCount(_info.getUserId());
//		int countOfOrigin = _infos.size();
//
//		Assert.assertEquals(countOfOrigin, countOfDB);
//		Assert.assertEquals(countOfOrigin, countOfCache);
//		Assert.assertEquals(countOfOrigin, countOfReaderCache);
//	}

	@Test
	public void testGetJuniorHighSchoolInfoList() {
		JuniorHighSchoolInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetJuniorHighSchoolInfoInt() {
		init();

		JuniorHighSchoolInfo infoOfDB = readDBByUserId(_info.getUserId()).get(0);
		JuniorHighSchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getJuniorHighSchoolInfo(_info.getUserId());
		JuniorHighSchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getJuniorHighSchoolInfo(_info.getUserId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}
	
	@Test
	public void testGetJuniorHighSchoolInfoIntInt() {
		init();

		JuniorHighSchoolInfo infoOfDB = readDBByRecordId(_info.getId()).get(0);
		JuniorHighSchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getJuniorHighSchoolInfo(_info.getUserId(), _info.getId());
		JuniorHighSchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getJuniorHighSchoolInfo(_info.getUserId(), _info.getId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}
	
	@Test
	public void testGetJuniorHighSchoolInfoBySchoolId() {
		init();

		JuniorHighSchoolInfo infoOfDB = readDBByUserIdAndInfoId(_info.getUserId(), _info.getJunHighentarySchoolId()).get(0);
		JuniorHighSchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getJuniorHighSchoolInfoBySchoolId(_info.getUserId(), _info.getJunHighentarySchoolId());
		JuniorHighSchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getJuniorHighSchoolInfoBySchoolId(_info.getUserId(), _info.getJunHighentarySchoolId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testReloadJuniorHighSchoolInfo() {
		init();

		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddJuniorHighSchoolInfo() {
		JuniorHighSchoolInfo info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addJuniorHighSchoolInfo(info.getUserId(), info);
		checkAll();

		// Writer
		init();
		
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addJuniorHighSchoolInfo(info.getUserId(), info);
		checkAll();
	}

	@Test
	public void testRemoveAllJuniorHighSchoolInfo() {
		JuniorHighSchoolInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllJuniorHighSchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllJuniorHighSchoolInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testRemoveJuniorHighSchoolInfoInt() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeJuniorHighSchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeJuniorHighSchoolInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testRemoveJuniorHighSchoolInfoIntInt() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeJuniorHighSchoolInfo(_info.getUserId(), _info.getId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeJuniorHighSchoolInfo(_info.getUserId(), _info.getId());
		checkAll();
	}

	@Test
	public void testSetJuniorHighSchoolInfo() {
		JuniorHighSchoolInfo info = genInfoCopy(0);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setJuniorHighSchoolInfo(info);
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setJuniorHighSchoolInfo(info);
		checkAll();
	}

	protected void checkAll() {
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<JuniorHighSchoolInfo> infosOfDB = readDBByUserId(_info.getUserId());
		// System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<JuniorHighSchoolInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getJuniorHighSchoolInfoList(_info.getUserId());
		// System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<JuniorHighSchoolInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getJuniorHighSchoolInfoList(_info.getUserId());
		// System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, JuniorHighSchoolInfo info) {
		String sql = "replace into junior_high_school_info(id,userid,junior_high_school_id,junior_high_school_name,junior_high_school_year) values (" + recordId
					+ "," + info.getUserId() + "," + info.getJunHighentarySchoolId() + ",'" + info.getJunHighentarySchoolName() + "'," + info.getJunHighentarySchoolYear() + ")";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from junior_high_school_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from junior_high_school_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<JuniorHighSchoolInfo> readDBByUserId(int userId) {
		String sql = "select * from junior_high_school_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<JuniorHighSchoolInfo> readDBByRecordId(int recordId) {
		String sql = "select * from junior_high_school_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<JuniorHighSchoolInfo> readDBByUserIdAndInfoId(int userId, int infoId) {
		String sql = "select * from junior_high_school_info where userid=" + userId + " and junior_high_school_id=" + infoId;
		return readDB(sql);
	}

	protected List<JuniorHighSchoolInfo> readDB(String sql) {
		List<JuniorHighSchoolInfo> result = new ArrayList<JuniorHighSchoolInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						JuniorHighSchoolInfo info = new JuniorHighSchoolInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setJunHighentarySchoolId(rs.getInt("junior_high_school_id"));
						info.setJunHighentarySchoolName(rs.getString("junior_high_school_name"));
						info.setJunHighentarySchoolYear(rs.getInt("junior_high_school_year"));
						result.add(info);
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
		return result;
	}

	protected void writeDB(String sql) {
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection("common");
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

	protected void cleanBbAndCache() {
		cleanDBByUserId(_info.getUserId());
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadJuniorHighSchoolInfo(_info.getUserId());
	}

	protected boolean isEqual(List<JuniorHighSchoolInfo> infos1, List<JuniorHighSchoolInfo> infos2) {
		boolean result = true;

		if (infos1.size() != infos2.size()) {
			result = false;
		} else {
			for (int i = 0; i < infos1.size(); ++i) {
				if (!isEqual(infos1.get(i), infos2.get(i))) {
					result = false;
					break;
				}
			}
		}
		return result;
	}

	protected boolean isEqual(JuniorHighSchoolInfo info1, JuniorHighSchoolInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}