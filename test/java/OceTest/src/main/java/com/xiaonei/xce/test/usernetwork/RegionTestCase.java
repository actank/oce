package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.RegionInfo;
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
public class RegionTestCase {
	private static int testCount = 0;
	private List<RegionInfo> _infos;
	private final RegionInfo _info;

	public RegionTestCase(int id, int userId, int regionId, String provinceName, String cityName) {
		_info = new RegionInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setRegionId(regionId);
		_info.setProvinceName(provinceName);
		_info.setCityName(cityName);
		_infos = new ArrayList<RegionInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected RegionInfo genInfoCopy(int idStep) {
		RegionInfo result = new RegionInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setRegionId(_info.getRegionId() + 1);
		result.setProvinceName(_info.getProvinceName());
		result.setCityName(_info.getCityName());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "provinceName", "cityName" }, });
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
		List<RegionInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<RegionInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getRegionInfoList(_info.getUserId());
		List<RegionInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getRegionInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

//	@Test
//	public void testGetRegionInfoCount() {
//		init();
//
//		int countOfDB = readDBByUserId(_info.getUserId()).size();
//		int countOfCache = AdapterFactory.getNetworkAdapter().getRegionInfoCount(_info.getUserId());
//		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getRegionInfoCount(_info.getUserId());
//		int countOfOrigin = _infos.size();
//
//		Assert.assertEquals(countOfOrigin, countOfDB);
//		Assert.assertEquals(countOfOrigin, countOfCache);
//		Assert.assertEquals(countOfOrigin, countOfReaderCache);
//	}

	@Test
	public void testGetRegionInfoList() {
		RegionInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetRegionInfo() {
		init();

		RegionInfo infoOfDB = readDBByInfoId(_info.getRegionId()).get(0);
		RegionInfo infoOfCache = AdapterFactory.getNetworkAdapter().getRegionInfo(_info.getUserId(), _info.getRegionId());
		RegionInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getRegionInfo(_info.getUserId(), _info.getRegionId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testReloadRegionInfo() {
		init();

		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadRegionInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddRegionInfo() {
		RegionInfo info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addRegionInfo(info);
		checkAll();

		// Writer
		init();
		
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addRegionInfo(info);
		checkAll();
	}

	@Test
	public void testRemoveAllRegionInfo() {
		RegionInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllRegionInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllRegionInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testRemoveRegionInfo() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeRegionInfo(_info.getUserId(), _info.getRegionId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeRegionInfo(_info.getUserId(), _info.getRegionId());
		checkAll();
	}

	@Test
	public void testSetRegionInfo() {
		RegionInfo info = genInfoCopy(0);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setRegionInfo(info);
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setRegionInfo(info);
		checkAll();
	}

	protected void checkAll() {
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<RegionInfo> infosOfDB = readDBByUserId(_info.getUserId());
		// System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<RegionInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getRegionInfoList(_info.getUserId());
		// System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<RegionInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getRegionInfoList(_info.getUserId());
		// System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, RegionInfo info) {
		String sql = "replace into region_info(id,userid,region_id,province_name,city_name) values (" + recordId
		+ "," + info.getUserId() + "," + info.getRegionId() + ",'" + info.getProvinceName() + "','" + info.getCityName() + "')";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from region_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from region_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<RegionInfo> readDBByUserId(int userId) {
		String sql = "select * from region_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<RegionInfo> readDBByRecordId(int recordId) {
		String sql = "select * from region_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<RegionInfo> readDBByInfoId(int infoId) {
		String sql = "select * from region_info where region_id=" + infoId;
		return readDB(sql);
	}

	protected List<RegionInfo> readDB(String sql) {
		List<RegionInfo> result = new ArrayList<RegionInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						RegionInfo info = new RegionInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setRegionId(rs.getInt("region_id"));
						info.setProvinceName(rs.getString("province_name"));
						info.setCityName(rs.getString("city_name"));
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
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadRegionInfo(_info.getUserId());
	}

	protected boolean isEqual(List<RegionInfo> infos1, List<RegionInfo> infos2) {
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

	protected boolean isEqual(RegionInfo info1, RegionInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}