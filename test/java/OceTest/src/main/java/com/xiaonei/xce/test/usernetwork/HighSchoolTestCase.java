package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.HighSchoolInfo;
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
public class HighSchoolTestCase {
	private static int testCount = 0;
	private List<HighSchoolInfo> _infos;
	private final HighSchoolInfo _info;

	public HighSchoolTestCase(int id, int userId, int high_schoolId, String high_schoolName, int enrollYear, String h_class1, String h_class2, String h_class3) {
		_info = new HighSchoolInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setHighSchoolId(high_schoolId);
		_info.setHighSchoolName(high_schoolName);
		_info.setEnrollYear(enrollYear);
		_info.setHClass1(h_class1);
		_info.setHClass2(h_class2);
		_info.setHClass3(h_class3);
		_infos = new ArrayList<HighSchoolInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected HighSchoolInfo genInfoCopy(int idStep) {
		HighSchoolInfo result = new HighSchoolInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setHighSchoolId(_info.getHighSchoolId() + 1);
		result.setHighSchoolName(_info.getHighSchoolName() + ".new");
		result.setEnrollYear(_info.getEnrollYear());
		result.setHClass1(_info.getHClass1());
		result.setHClass2(_info.getHClass2());
		result.setHClass3(_info.getHClass3());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "high_schoolName", 2008 , "h_class1", "h_class2", "h_class3" }, });
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
		List<HighSchoolInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<HighSchoolInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getHighSchoolInfoList(_info.getUserId());
		List<HighSchoolInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getHighSchoolInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

	@Test
	public void testGetHighSchoolInfoCount() {
		init();

		int countOfDB = readDBByUserId(_info.getUserId()).size();
		int countOfCache = AdapterFactory.getNetworkAdapter().getHighSchoolInfoCount(_info.getUserId());
		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getHighSchoolInfoCount(_info.getUserId());
		int countOfOrigin = _infos.size();

		Assert.assertEquals(countOfOrigin, countOfDB);
		Assert.assertEquals(countOfOrigin, countOfCache);
		Assert.assertEquals(countOfOrigin, countOfReaderCache);
	}

	@Test
	public void testGetHighSchoolInfoList() {
		HighSchoolInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetHighSchoolInfoBySchoolId() {
		init();

		HighSchoolInfo infoOfDB = readDBByUserIdAndInfoId(_info.getUserId(), _info.getHighSchoolId()).get(0);
		HighSchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getHighSchoolInfoBySchoolId(_info.getUserId(), _info.getHighSchoolId());
		HighSchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getHighSchoolInfoBySchoolId(_info.getUserId(), _info.getHighSchoolId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testReloadHighSchoolInfoList() {
		init();

		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadHighSchoolInfoList(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddHighSchoolInfo() {
		HighSchoolInfo info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addHighSchoolInfo(info);
		checkAll();

		// Writer
		init();
		
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addHighSchoolInfo(info);
		checkAll();
	}

	@Test
	public void testRemoveAllHighSchoolInfo() {
		HighSchoolInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllHighSchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllHighSchoolInfo(_info.getUserId());
		checkAll();
	}

//	@Test
//	public void testRemoveHighSchoolInfoBySchoolId() {
//		init();
//
//		_infos.remove(_info);
//		AdapterFactory.getNetworkAdapter().removeHighSchoolInfoBySchoolId(_info.getUserId(), _info.getHighSchoolId());
//		checkAll();
//
//		// Writer
//		init();
//
//		_infos.remove(_info);
//		AdapterFactory.getNetworkWriterAdapter().removeHighSchoolInfoBySchoolId(_info.getUserId(), _info.getHighSchoolId());
//		checkAll();
//	}

	@Test
	public void testSetHighSchoolInfo() {
		HighSchoolInfo info = genInfoCopy(0);

		init();

		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());
		
		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setHighSchoolInfo(info);
		checkAll();

		// Writer
		init();
		
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());
		
		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setHighSchoolInfo(info);
		checkAll();
	}

	protected void checkAll() {
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<HighSchoolInfo> infosOfDB = readDBByUserId(_info.getUserId());
		// System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<HighSchoolInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getHighSchoolInfoList(_info.getUserId());
		// System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<HighSchoolInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getHighSchoolInfoList(_info.getUserId());
		// System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, HighSchoolInfo info) {
		String sql = "replace into high_school_info(id,userid,high_school_id,high_school_name,enroll_year,h_class1,h_class2,h_class3) values (" + recordId
				+ "," + info.getUserId() + "," + info.getHighSchoolId() + ",'" + info.getHighSchoolName() + "'," + info.getEnrollYear() + ",'"
				+ info.getHClass1() + "','" + info.getHClass2() + "','" + info.getHClass3() + "')";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from high_school_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from high_school_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<HighSchoolInfo> readDBByUserId(int userId) {
		String sql = "select * from high_school_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<HighSchoolInfo> readDBByRecordId(int recordId) {
		String sql = "select * from high_school_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<HighSchoolInfo> readDBByUserIdAndInfoId(int userId, int infoId) {
		String sql = "select * from high_school_info where userid=" + userId + " and high_school_id=" + infoId;
		return readDB(sql);
	}

	protected List<HighSchoolInfo> readDB(String sql) {
		List<HighSchoolInfo> result = new ArrayList<HighSchoolInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						HighSchoolInfo info = new HighSchoolInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setHighSchoolId(rs.getInt("high_school_id"));
						info.setHighSchoolName(rs.getString("high_school_name"));
						info.setHClass1(rs.getString("h_class1"));
						info.setHClass2(rs.getString("h_class2"));
						info.setHClass3(rs.getString("h_class3"));
						info.setEnrollYear(rs.getInt("enroll_year"));
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
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadHighSchoolInfoList(_info.getUserId());
	}

	protected boolean isEqual(List<HighSchoolInfo> infos1, List<HighSchoolInfo> infos2) {
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

	protected boolean isEqual(HighSchoolInfo info1, HighSchoolInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}