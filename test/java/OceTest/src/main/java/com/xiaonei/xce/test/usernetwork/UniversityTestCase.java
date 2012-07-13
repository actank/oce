package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.UniversityInfo;
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
public class UniversityTestCase {
	private static int testCount = 0;
	private List<UniversityInfo> _infos;
	private final UniversityInfo _info;

	public UniversityTestCase(int id, int userId, int universityId, String universityName, String dorm, String department, String typeOfCourse, int enrollYear) {
		_info = new UniversityInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setUniversityId(universityId);
		_info.setUniversityName(universityName);
		_info.setDorm(dorm);
		_info.setDepartment(department);
		_info.setTypeOfCourse(typeOfCourse);
		_info.setEnrollYear(enrollYear);
		_infos = new ArrayList<UniversityInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected UniversityInfo genInfoCopy(int idStep) {
		UniversityInfo result = new UniversityInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setUniversityId(_info.getUniversityId() + 1);
		result.setUniversityName(_info.getUniversityName() + ".new");
		result.setDorm(_info.getDorm());
		result.setDepartment(_info.getDepartment());
		result.setTypeOfCourse(_info.getTypeOfCourse());
		result.setEnrollYear(_info.getEnrollYear());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "universityName", "dorm", "department", "typeOfCourse", 2008 }, });
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
		List<UniversityInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<UniversityInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getUniversityInfoList(_info.getUserId());
		List<UniversityInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getUniversityInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

	@Test
	public void testGetUniversityInfoCount() {
		init();

		int countOfDB = readDBByUserId(_info.getUserId()).size();
		int countOfCache = AdapterFactory.getNetworkAdapter().getUniversityInfoCount(_info.getUserId());
		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getUniversityInfoCount(_info.getUserId());
		int countOfOrigin = _infos.size();

		Assert.assertEquals(countOfOrigin, countOfDB);
		Assert.assertEquals(countOfOrigin, countOfCache);
		Assert.assertEquals(countOfOrigin, countOfReaderCache);
	}

	@Test
	public void testGetUniversityInfoList() {
		UniversityInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetUniversityInfoByUnivId() {
		init();

		UniversityInfo infoOfDB = readDBByUserIdAndInfoId(_info.getUserId(), _info.getUniversityId()).get(0);
		UniversityInfo infoOfCache = AdapterFactory.getNetworkAdapter().getUniversityInfoByUnivId(_info.getUserId(), _info.getUniversityId());
		UniversityInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getUniversityInfoByUnivId(_info.getUserId(), _info.getUniversityId());

		System.out.println(infoOfDB + " " + infoOfCache + " " + infoOfReaderCache + " " + _info);
		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testReloadUniversityInfoList() {
		init();

		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadUniversityInfoList(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddUniversityInfo() {
		UniversityInfo info = genInfoCopy(1);
	
		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addUniversityInfo(info);
		checkAll();

		// Writer
		init();
		
		//由于add会改变参数，所以新生成一个info
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addUniversityInfo(info);
		checkAll();
	}

	@Test
	public void testRemoveAllUniversityInfo() {
		UniversityInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllUniversityInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllUniversityInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testRemoveUniversityInfoByUnivId() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeUniversityInfoByUnivId(_info.getUserId(), _info.getUniversityId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeUniversityInfoByUnivId(_info.getUserId(), _info.getUniversityId());
		checkAll();
	}

	@Test
	public void testSetUniversityInfo() {
		UniversityInfo info = genInfoCopy(0);

		init();

		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
		
		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setUniversityInfo(info);
		checkAll();

		// Writer
		init();

		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
		
		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setUniversityInfo(info);
		checkAll();
	}

	protected void checkAll() {
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<UniversityInfo> infosOfDB = readDBByUserId(_info.getUserId());
		System.out.println("\ninfosOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<UniversityInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getUniversityInfoList(_info.getUserId());
		System.out.println("\ninfosOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<UniversityInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getUniversityInfoList(_info.getUserId());
		System.out.println("\ninfosOfReaderCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, UniversityInfo info) {
		String sql = "replace into university_info(id,userid,university_id,university_name,dorm,department,type_of_course,enroll_year) values (" + recordId
				+ "," + info.getUserId() + "," + info.getUniversityId() + ",'" + info.getUniversityName() + "','" + info.getDorm() + "','"
				+ info.getDepartment() + "','" + info.getTypeOfCourse() + "'," + info.getEnrollYear() + ")";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from university_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from university_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<UniversityInfo> readDBByUserId(int userId) {
		String sql = "select * from university_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<UniversityInfo> readDBByRecordId(int recordId) {
		String sql = "select * from university_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<UniversityInfo> readDBByUserIdAndInfoId(int userId, int infoId) {
		String sql = "select * from university_info where userid=" + userId + " and university_id=" + infoId;
		return readDB(sql);
	}

	protected List<UniversityInfo> readDB(String sql) {
		List<UniversityInfo> result = new ArrayList<UniversityInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						UniversityInfo info = new UniversityInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setUniversityId(rs.getInt("university_id"));
						info.setUniversityName(rs.getString("university_name"));
						info.setDorm(rs.getString("dorm"));
						info.setDepartment(rs.getString("department"));
						info.setTypeOfCourse(rs.getString("type_of_course"));
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
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
	}

	protected boolean isEqual(List<UniversityInfo> infos1, List<UniversityInfo> infos2) {
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

	protected boolean isEqual(UniversityInfo info1, UniversityInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}