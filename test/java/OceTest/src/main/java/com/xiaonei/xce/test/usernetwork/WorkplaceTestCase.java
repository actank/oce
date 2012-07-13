package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.WorkplaceInfo;
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
public class WorkplaceTestCase {
	private static int testCount = 0;
	private List<WorkplaceInfo> _infos;
	private final WorkplaceInfo _info;

	public WorkplaceTestCase(int id, int userId, int workplaceId, String workplaceName, String address, String province, int city_id, String city_name,
			String description, int join_year, int join_month, int quit_year, int quit_month, int type, int job_title_id, int position_id) {
		_info = new WorkplaceInfo();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setWorkplaceId(workplaceId);
		_info.setWorkplaceName(workplaceName);
		_info.setAddress(address);
		_info.setProvince(province);
		_info.setCityId(city_id);
		_info.setCityName(city_name);
		_info.setDescription(description);
		_info.setJoinYear(join_year);
		_info.setJoinMonth(join_month);
		_info.setQuitYear(quit_year);
		_info.setQuitMonth(quit_month);
		_info.setType(type);
		_info.setJobTitleId(job_title_id);
		_info.setPositionId(position_id);
		_infos = new ArrayList<WorkplaceInfo>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected WorkplaceInfo genInfoCopy(int idStep) {
		WorkplaceInfo result = new WorkplaceInfo();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setWorkplaceId(_info.getWorkplaceId() + 1);
		result.setWorkplaceName(_info.getWorkplaceName() + ".new");
		result.setAddress(_info.getAddress());
		result.setProvince(_info.getProvince());
		result.setCityId(_info.getCityId());
		result.setCityName(_info.getCityName());
		result.setDescription(_info.getDescription());
		result.setJoinYear(_info.getJoinYear());
		result.setJoinMonth(_info.getJoinMonth());
		result.setQuitYear(_info.getQuitYear());
		result.setQuitMonth(_info.getQuitMonth());
		result.setType(_info.getType());
		result.setJobTitleId(_info.getJobTitleId());
		result.setPositionId(_info.getPositionId());

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 55555, 800000444, 10, "workplaceName", "address", "province", 10000, "city_name", "description", 2008, 12,
				2010, 12, 8888, 4444, 3333 }, });
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
		List<WorkplaceInfo> infosOfDB = readDBByUserId(_info.getUserId());
		List<WorkplaceInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getWorkplaceInfoList(_info.getUserId());
		List<WorkplaceInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getWorkplaceInfoList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

	@Test
	public void testGetWorkplaceInfoCount() {
		init();

		int countOfDB = readDBByUserId(_info.getUserId()).size();
		int countOfCache = AdapterFactory.getNetworkAdapter().getWorkplaceInfoCount(_info.getUserId());
		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getWorkplaceInfoCount(_info.getUserId());
		int countOfOrigin = _infos.size();

		Assert.assertEquals(countOfOrigin, countOfDB);
		Assert.assertEquals(countOfOrigin, countOfCache);
		Assert.assertEquals(countOfOrigin, countOfReaderCache);
	}

//	@Test
	public void testGetWorkplaceInfoList() {
		WorkplaceInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadWorkplaceInfoList(_info.getUserId());

		checkAll();
	}

	@Test
	public void testGetWorkplaceInfoByWorkId() {
		init();

		WorkplaceInfo infoOfDB = readDBByUserIdAndInfoId(_info.getUserId(), _info.getWorkplaceId()).get(0);
		WorkplaceInfo infoOfCache = AdapterFactory.getNetworkAdapter().getWorkplaceInfoByWorkId(_info.getUserId(), _info.getWorkplaceId());
		WorkplaceInfo infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getWorkplaceInfoByWorkId(_info.getUserId(), _info.getWorkplaceId());

		System.out.println(infoOfDB + "\n---\n" + infoOfCache + "\n---\n" + infoOfReaderCache + "\n---\n" + _info);
		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	@Test
	public void testReloadWorkplaceInfoList() {
		init();

		AdapterFactory.getNetworkAdapter().reloadWorkplaceInfoList(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadWorkplaceInfoList(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddWorkplaceInfo() {
		WorkplaceInfo info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addWorkplaceInfo(info);
		checkAll();

		// Writer
		init();

		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addWorkplaceInfo(info);
		checkAll();
	}

	@Test
	public void testRemoveAllWorkplaceInfo() {
		WorkplaceInfo info = genInfoCopy(1);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadWorkplaceInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkAdapter().removeAllWorkplaceInfo(_info.getUserId());
		checkAll();

		// Writer
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadWorkplaceInfoList(_info.getUserId());

		_infos.clear();
		AdapterFactory.getNetworkWriterAdapter().removeAllWorkplaceInfo(_info.getUserId());
		checkAll();
	}

	@Test
	public void testRemoveWorkplaceInfoByWorkId() throws Exception {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeWorkplaceInfoByWorkId(_info.getUserId(), _info.getWorkplaceId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeWorkplaceInfoByWorkId(_info.getUserId(), _info.getWorkplaceId());
		checkAll();
	}

	@Test
	public void testSetWorkplaceInfo() {
		WorkplaceInfo info = genInfoCopy(0);

		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
		
		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setWorkplaceInfo(info);
		checkAll();

		// Writer
		info = genInfoCopy(0);
		init();
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadUniversityInfoList(_info.getUserId());
		
		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setWorkplaceInfo(info);
		checkAll();
	}

	protected void checkAll() {
		Assert.assertTrue(isDBCorrect());
		Assert.assertTrue(isCacheCorrect());
		Assert.assertTrue(isReaderCacheCorrect());
	}

	protected boolean isDBCorrect() {
		List<WorkplaceInfo> infosOfDB = readDBByUserId(_info.getUserId());
		System.out.println("\ninfosOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<WorkplaceInfo> infosOfCache = AdapterFactory.getNetworkAdapter().getWorkplaceInfoList(_info.getUserId());
		System.out.println("\ninfosOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<WorkplaceInfo> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getWorkplaceInfoList(_info.getUserId());
		System.out.println("\ninfosOfReaderCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, WorkplaceInfo info) {
		String sql = "replace into workplace_info(id,userid,workplace_id,workplace_name,address,province,city_id,city_name,"
				+ "description,join_year,join_month,quit_year,quit_month,type,job_title_id, position_id) values ("
				+ recordId
				+ ","
				+ info.getUserId()
				+ ","
				+ info.getWorkplaceId()
				+ ",'"
				+ info.getWorkplaceName()
				+ "','"
				+ info.getAddress()
				+ "','"
				+ info.getProvince()
				+ "',"
				+ info.getCityId()
				+ ",'"
				+ info.getCityName()
				+ "','"
				+ info.getDescription()
				+ "',"
				+ info.getJoinYear()
				+ ","
				+ info.getJoinMonth()
				+ ","
				+ info.getQuitYear()
				+ ","
				+ info.getQuitMonth()
				+ ","
				+ info.getType()
				+ ","
				+ info.getJobTitleId()
				+ ","
				+ info.getPositionId() + ")";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from workplace_info where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from workplace_info where id=" + recordId;
		writeDB(sql);
	}

	protected List<WorkplaceInfo> readDBByUserId(int userId) {
		String sql = "select * from workplace_info where userid=" + userId;
		return readDB(sql);
	}

	protected List<WorkplaceInfo> readDBByRecordId(int recordId) {
		String sql = "select * from workplace_info where id=" + recordId;
		return readDB(sql);
	}

	protected List<WorkplaceInfo> readDBByUserIdAndInfoId(int userId, int infoId) {
		String sql = "select * from workplace_info where userid=" + userId + " and workplace_id=" + infoId;
		return readDB(sql);
	}

	protected List<WorkplaceInfo> readDB(String sql) {
		List<WorkplaceInfo> result = new ArrayList<WorkplaceInfo>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						WorkplaceInfo info = new WorkplaceInfo();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setWorkplaceId(rs.getInt("workplace_id"));
						info.setWorkplaceName(rs.getString("workplace_name"));
						info.setAddress(rs.getString("address"));
						info.setProvince(rs.getString("province"));
						info.setCityId(rs.getInt("city_id"));
						info.setCityName(rs.getString("city_name"));
						info.setDescription(rs.getString("description"));
						info.setJoinYear(rs.getInt("join_year"));
						info.setJoinMonth(rs.getInt("join_month"));
						info.setQuitYear(rs.getInt("quit_year"));
						info.setQuitMonth(rs.getInt("quit_month"));
						info.setType(rs.getInt("type"));
						info.setJobTitleId(rs.getInt("job_title_id"));
						info.setPositionId(rs.getInt("position_id"));
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
		AdapterFactory.getNetworkAdapter().reloadWorkplaceInfoList(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadWorkplaceInfoList(_info.getUserId());
	}

	protected boolean isEqual(List<WorkplaceInfo> infos1, List<WorkplaceInfo> infos2) {
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

	protected boolean isEqual(WorkplaceInfo info1, WorkplaceInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}