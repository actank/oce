package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.CollegeInfo;
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
public class CollegeTestCase {
	private static int testCount = 0;
	private List<CollegeInfo> _infos;
	private final CollegeInfo _info;

	public CollegeTestCase(int id, int userId, int collegeId, String collegeName, String department, int enrollYear) {
		_info = new CollegeInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setCollegeId(collegeId);
		_info.setCollegeName(collegeName);
		_info.setDepartment(department);
		_info.setEnrollYear(enrollYear);
		_infos = new ArrayList<CollegeInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected CollegeInfo genInfoCopy(int idStep) {
		CollegeInfo result = new CollegeInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setCollegeId(_info.getCollegeId() + 1);
		result.setCollegeName(_info.getCollegeName() + ".new");
		result.setDepartment(_info.getDepartment());
		result.setEnrollYear(_info.getEnrollYear());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "collegeName", "department", 2008 }, });
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
	 * reload和get是最关键的方法 测试中对缓存的控制依赖于reload方法 测试中对缓存的判断依赖于get方法
	 */
	@Test
	public void testBase() {
		init();

		// Debug info
		List<CollegeInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<CollegeInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getCollegeInfoList(_info.getUserId());
		List<CollegeInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getCollegeInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

	@Test
	public void testGetCollegeInfoCount() {
		init();

		int countOfDB = readDBByUserId(_info.getUserId()).size();
		int countOfCache = AdapterFactory.getNetworkAdapter().getCollegeInfoCount(_info.getUserId());
		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getCollegeInfoCount(_info.getUserId());
		int countOfOrigin = _infos.size();

		Assert.assertEquals(countOfOrigin, countOfDB);
		Assert.assertEquals(countOfOrigin, countOfCache);
		Assert.assertEquals(countOfOrigin, countOfReaderCache);
	}

	@Test
	public void testGetCollegeInfoList() {
		CollegeInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadCollegeInfoList(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetCollegeInfo() {
		init();

		CollegeInfo infoOfDB = readDBByRecordId(_info.getId()).get(0);
		CollegeInfo infoOfCache = AdapterFactory.getNetworkAdapter().getCollegeInfo(_info.getUserId(), _info.getId());
		CollegeInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getCollegeInfo(_info.getUserId(), _info.getId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testGetCollegeInfoByCollegeId() {
		init();

		CollegeInfo infoOfDB = readDBByUserIdAndInfoId(_info.getUserId(), _info.getCollegeId()).get(0);
		CollegeInfo infoOfCache = AdapterFactory.getNetworkAdapter().getCollegeInfoByCollegeId(_info.getUserId(), _info.getCollegeId());
		CollegeInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getCollegeInfoByCollegeId(_info.getUserId(), _info.getCollegeId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}
	
	@Test
	public void testReloadCollegeInfoList() {
		init();

		AdapterFactory.getNetworkAdapter().reloadCollegeInfoList(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadCollegeInfoList(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddCollegeInfo() {
		CollegeInfo info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addCollegeInfo(info.getUserId(), info);
		checkAll();

		// Writer
		init();
		
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addCollegeInfo(info.getUserId(), info);
		checkAll();
	}

	@Test
	public void testRemoveAllCollegeInfo() {
		CollegeInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadCollegeInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllCollegeInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadCollegeInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllCollegeInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testRemoveCollegeInfo() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeCollegeInfo(_info.getUserId(), _info.getId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeCollegeInfo(_info.getUserId(), _info.getId());
		checkAll();
	}

	@Test
	public void testSetCollegeInfo() {
		CollegeInfo info = genInfoCopy(0);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setCollegeInfo(info.getId(), info.getUserId(), info);
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setCollegeInfo(info.getId(), info.getUserId(), info);
		checkAll();
	}

	protected void checkAll() {
		System.out.println("\n_infos:");
		System.out.println("--------------------------------");
		System.out.println(_infos);
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<CollegeInfo> infosOfDB = readDBByUserId(_info.getUserId());
		System.out.println("\ninfosOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<CollegeInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getCollegeInfoList(_info.getUserId());
		System.out.println("\ninfosOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<CollegeInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getCollegeInfoList(_info.getUserId());
		System.out.println("\ninfosOfReaderCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, CollegeInfo info) {
		String sql = "replace into college_info(id,userid,college_id,college_name,department,enroll_year) values (" + recordId
				+ "," + info.getUserId() + "," + info.getCollegeId() + ",'" + info.getCollegeName() + "','"
				+ info.getDepartment() + "'," + info.getEnrollYear() + ")";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from college_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from college_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<CollegeInfo> readDBByUserId(int userId) {
		String sql = "select * from college_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<CollegeInfo> readDBByRecordId(int recordId) {
		String sql = "select * from college_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<CollegeInfo> readDBByUserIdAndInfoId(int userId, int infoId) {
		String sql = "select * from college_info where userid=" + userId + " and college_id=" + infoId;
		return readDB(sql);
	}

	protected List<CollegeInfo> readDB(String sql) {
		List<CollegeInfo> result = new ArrayList<CollegeInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						CollegeInfo info = new CollegeInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setCollegeId(rs.getInt("college_id"));
						info.setCollegeName(rs.getString("college_name"));
						info.setDepartment(rs.getString("department"));
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
		AdapterFactory.getNetworkAdapter().reloadCollegeInfoList(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadCollegeInfoList(_info.getUserId());
	}

	protected boolean isEqual(List<CollegeInfo> infos1, List<CollegeInfo> infos2) {
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

	protected boolean isEqual(CollegeInfo info1, CollegeInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}