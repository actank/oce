package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.ElementarySchoolInfo;
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
public class ElementarySchoolTestCase {
	private static int testCount = 0;
	private List<ElementarySchoolInfo> _infos;
	private final ElementarySchoolInfo _info;

	public ElementarySchoolTestCase(int id, int userId, int elementarySchoolId, String elementarySchoolName, int elementarySchoolYear) {
		_info = new ElementarySchoolInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setElementarySchoolId(elementarySchoolId);
		_info.setElementarySchoolName(elementarySchoolName);
		_info.setElementarySchoolYear(elementarySchoolYear);
		_infos = new ArrayList<ElementarySchoolInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected ElementarySchoolInfo genInfoCopy(int idStep) {
		ElementarySchoolInfo result = new ElementarySchoolInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setElementarySchoolId(_info.getElementarySchoolId());
		result.setElementarySchoolName(_info.getElementarySchoolName());
		result.setElementarySchoolYear(_info.getElementarySchoolYear());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "elementarySchoolName", 2008 }, });
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
		List<ElementarySchoolInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<ElementarySchoolInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getElementarySchoolInfoList(_info.getUserId());
		List<ElementarySchoolInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getElementarySchoolInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

//	@Test
//	public void testGetElementarySchoolInfoCount() {
//		init();
//
//		int countOfDB = readDBByUserId(_info.getUserId()).size();
//		int countOfCache = AdapterFactory.getNetworkAdapter().getElementarySchoolInfoCount(_info.getUserId());
//		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getElementarySchoolInfoCount(_info.getUserId());
//		int countOfOrigin = _infos.size();
//
//		Assert.assertEquals(countOfOrigin, countOfDB);
//		Assert.assertEquals(countOfOrigin, countOfCache);
//		Assert.assertEquals(countOfOrigin, countOfReaderCache);
//	}

	@Test
	public void testGetElementarySchoolInfoList() {
		ElementarySchoolInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetElementarySchoolInfoInt() {
		init();

		ElementarySchoolInfo infoOfDB = readDBByUserId(_info.getUserId()).get(0);
		ElementarySchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getElementarySchoolInfo(_info.getUserId());
		ElementarySchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getElementarySchoolInfo(_info.getUserId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}
	
	@Test
	public void testGetElementarySchoolInfoIntInt() {
		init();

		ElementarySchoolInfo infoOfDB = readDBByRecordId(_info.getId()).get(0);
		ElementarySchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getElementarySchoolInfo(_info.getUserId(), _info.getId());
		ElementarySchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getElementarySchoolInfo(_info.getUserId(), _info.getId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}
	
	@Test
	public void testGetElementarySchoolInfoBySchoolId() {
		init();

		ElementarySchoolInfo infoOfDB = readDBByUserIdAndInfoId(_info.getUserId(), _info.getElementarySchoolId()).get(0);
		ElementarySchoolInfo infoOfCache = AdapterFactory.getNetworkAdapter().getElementarySchoolInfoBySchoolId(_info.getUserId(), _info.getElementarySchoolId());
		ElementarySchoolInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getElementarySchoolInfoBySchoolId(_info.getUserId(), _info.getElementarySchoolId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testReloadElementarySchoolInfo() {
		init();

		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadElementarySchoolInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddElementarySchoolInfo() {
		ElementarySchoolInfo info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addElementarySchoolInfo(info.getUserId(), info);
		checkAll();

		// Writer
		init();
		
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addElementarySchoolInfo(info.getUserId(), info);
		checkAll();
	}

	@Test
	public void testRemoveElementarySchoolInfoInt() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeElementarySchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeElementarySchoolInfo(_info.getUserId());
		checkAll();
	}
	
	/**
	 * 注意removeElementarySchoolInfo中的id参数应该是record id
	 */
	@Test
	public void testRemoveElementarySchoolInfoIntInt() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeElementarySchoolInfo(_info.getUserId(), _info.getId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeElementarySchoolInfo(_info.getUserId(), _info.getId());
		checkAll();
	}

	@Test
	public void testRemoveAllElementarySchoolInfo() {
		ElementarySchoolInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllElementarySchoolInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllElementarySchoolInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testSetElementarySchoolInfo() {
		ElementarySchoolInfo info = genInfoCopy(0);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setElementarySchoolInfo(info);
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setElementarySchoolInfo(info);
		checkAll();
	}
	
	protected void checkAll() {
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<ElementarySchoolInfo> infosOfDB = readDBByUserId(_info.getUserId());
		System.out.println("\ninfosOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<ElementarySchoolInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getElementarySchoolInfoList(_info.getUserId());
		System.out.println("\ninfosOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<ElementarySchoolInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getElementarySchoolInfoList(_info.getUserId());
		System.out.println("\ninfosOfReaderCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, ElementarySchoolInfo info) {
		String sql = "replace into elementary_school_info(id,userid,elementary_school_id,elementary_school_name,elementary_school_year) values (" + recordId
					+ "," + info.getUserId() + "," + info.getElementarySchoolId() + ",'" + info.getElementarySchoolName() + "'," + info.getElementarySchoolYear() + ")";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from elementary_school_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from elementary_school_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<ElementarySchoolInfo> readDBByUserId(int userId) {
		String sql = "select * from elementary_school_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<ElementarySchoolInfo> readDBByRecordId(int recordId) {
		String sql = "select * from elementary_school_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<ElementarySchoolInfo> readDBByUserIdAndInfoId(int userId, int infoId) {
		String sql = "select * from elementary_school_info where userid=" + userId + " and elementary_school_id=" + infoId;
		return readDB(sql);
	}

	protected List<ElementarySchoolInfo> readDB(String sql) {
		List<ElementarySchoolInfo> result = new ArrayList<ElementarySchoolInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						ElementarySchoolInfo info = new ElementarySchoolInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setElementarySchoolId(rs.getInt("elementary_school_id"));
						info.setElementarySchoolName(rs.getString("elementary_school_name"));
						info.setElementarySchoolYear(rs.getInt("elementary_school_year"));
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
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadElementarySchoolInfo(_info.getUserId());
	}

	protected boolean isEqual(List<ElementarySchoolInfo> infos1, List<ElementarySchoolInfo> infos2) {
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

	protected boolean isEqual(ElementarySchoolInfo info1, ElementarySchoolInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}