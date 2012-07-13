package com.xiaonei.xce.test.usernetwork;

import mop.hi.oce.domain.network.Network;
import mop.hi.oce.adapter.AdapterFactory;
import com.xiaonei.xce.XceAdapter;
import xce.util.tools.DateFormatter;

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
public class NetworkTestCase {
	private static int testCount = 0;
	private List<Network> _infos;
	private final Network _info;

	public NetworkTestCase(int id, int userId, int stage, int networkId, String networkName, int infoId, String joinTime, int status) {
		_info = new Network();
		_info.setId(id);
		_info.setUserId(userId);
		_info.setStage(stage);
		_info.setNetworkId(networkId);
		_info.setNetworkName(networkName);
		_info.setInfoId(infoId);
		_info.setJoinTime(DateFormatter.parse(joinTime));
		_info.setStatus(status);
		_infos = new ArrayList<Network>();
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected Network genInfoCopy(int idStep) {
		Network result = new Network();
		result.setId(_info.getId() + idStep);
		result.setUserId(_info.getUserId());
		result.setStage(_info.getStage());
		result.setNetworkId(_info.getNetworkId() + idStep);
		result.setNetworkName(_info.getNetworkName() + ".new");
		result.setInfoId(_info.getInfoId());
		result.setJoinTime(_info.getJoinTime());
		result.setStatus(_info.getStatus() + idStep);

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { 
				{ 55555, 800000444, 10, 61234567, "networkName", 10, "2011-08-04 17:47:51", 0 },
				{ 55555, 800000444, 10, 612345678, "networkName", 10, "2011-08-04 17:47:51", 0 },
				{ 55555, 800000444, 10, 61234567, "networkName", 10, "2011-08-04 17:47:51", -1 },
				{ 55555, 800000444, 10, 61234567, "networkName", 10, "2011-08-04 17:47:51", 1 },
		});
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
		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		List<Network> infosOfCache = AdapterFactory.getNetworkAdapter().getNetworkList(_info.getUserId());
		List<Network> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkList(_info.getUserId());
		System.out.println(infosOfDB);
		System.out.println(infosOfCache);
		System.out.println(infosOfReaderCache);

		checkAll();
	}

	@Test
	public void testGetNetworkCountByStage() {
		init();

		int countOfDB = readDBByUserId(_info.getUserId()).size();
		int countOfCache = AdapterFactory.getNetworkAdapter().getNetworkCountByStage(_info.getUserId(), _info.getStage());
		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkCountByStage(_info.getUserId(), _info.getStage());
		int countOfOrigin = _infos.size();

		Assert.assertEquals(countOfOrigin, countOfDB);
		Assert.assertEquals(countOfOrigin, countOfCache);
		Assert.assertEquals(countOfOrigin, countOfReaderCache);
	}

	@Test
	public void testGetValidNetworkCountByStage() {
		init();

		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		int countOfDB = 0;
		for(int i = 0; i < infosOfDB.size(); i++){
			if (infosOfDB.get(i).getStatus() == -1 || infosOfDB.get(i).getStatus() == 0){
				countOfDB++;
			}
		}
		int countOfOrigin = 0;
		for(int i = 0; i < _infos.size(); i++){
			if (_infos.get(i).getStatus() == -1 || _infos.get(i).getStatus() == 0){
				countOfOrigin++;
			}
		}
		int countOfCache = AdapterFactory.getNetworkAdapter().getValidNetworkCountByStage(_info.getUserId(), _info.getStage());
		int countOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getValidNetworkCountByStage(_info.getUserId(), _info.getStage());

		System.out.println(countOfOrigin + " " + countOfDB + " " + countOfCache + " " + countOfReaderCache);
		Assert.assertEquals(countOfOrigin, countOfDB);
		Assert.assertEquals(countOfOrigin, countOfCache);
		Assert.assertEquals(countOfOrigin, countOfReaderCache);
	}

	@Test
	public void testGetNetworkList() {
		init();

		Network info = genInfoCopy(1);
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());
		checkAll();
	}

	@Test
	public void testGetNetworkListByStage() {
		init();

		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		List<Network> infosOfCache = AdapterFactory.getNetworkAdapter().getNetworkListByStage(_info.getUserId(), _info.getStage());
		List<Network> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkListByStage(_info.getUserId(), _info.getStage());

		Assert.assertTrue(isEqual(infosOfDB, _infos));
		Assert.assertTrue(isEqual(infosOfCache, _infos));
		Assert.assertTrue(isEqual(infosOfReaderCache, _infos));
	}

	@Test
	public void testGetNetworkListByStatusIntInt() {
		init();

		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		List<Network> infosOfCache = AdapterFactory.getNetworkAdapter().getNetworkListByStatus(_info.getUserId(), _info.getStatus());
		List<Network> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkListByStatus(_info.getUserId(), _info.getStatus());

		Assert.assertTrue(isEqual(infosOfDB, _infos));
		Assert.assertTrue(isEqual(infosOfCache, _infos));
		Assert.assertTrue(isEqual(infosOfReaderCache, _infos));
	}

	@Test
	public void testGetNetworkListByStageAndStatus() {
		init();
		Network info = genInfoCopy(1);
		int status[] = { _info.getStatus(), info.getStatus() };
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());

		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		List<Network> infosOfCache = AdapterFactory.getNetworkAdapter().getNetworkListByStageAndStatus(_info.getUserId(), _info.getStage(), status);
		List<Network> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkListByStageAndStatus(_info.getUserId(), _info.getStage(), status);

		Assert.assertTrue(isEqual(infosOfDB, _infos));
		Assert.assertTrue(isEqual(infosOfCache, _infos));
		Assert.assertTrue(isEqual(infosOfReaderCache, _infos));
	}

	@Test
	public void testGetNetworkListByStatusIntIntArray() {
		init();
		Network info = genInfoCopy(1);
		int status[] = { _info.getStatus(), info.getStatus() };
		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());

		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		List<Network> infosOfCache = AdapterFactory.getNetworkAdapter().getNetworkListByStatus(_info.getUserId(), status);
		List<Network> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkListByStatus(_info.getUserId(), status);

		Assert.assertTrue(isEqual(infosOfDB, _infos));
		Assert.assertTrue(isEqual(infosOfCache, _infos));
		Assert.assertTrue(isEqual(infosOfReaderCache, _infos));
	}

	@Test
	public void testGetNetwork() {
		init();

		Network infoOfDB = readDBByInfoId(_info.getNetworkId()).get(0);
		Network infoOfCache = AdapterFactory.getNetworkAdapter().getNetwork(_info.getUserId(), _info.getNetworkId());
		Network infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetwork(_info.getUserId(), _info.getNetworkId());

		Assert.assertTrue(isEqual(infoOfDB, _info));
		Assert.assertTrue(isEqual(infoOfCache, _info));
		Assert.assertTrue(isEqual(infoOfReaderCache, _info));
	}

	/**
	 * ((networkId >= 60000000) && (networkId <= 70000000) && ((status == 0) || (status == -1)))
	 */
	@Test
	public void testGetRegionalNetwork() {
		init();
		List<Network> all = readDBByRecordId(_info.getId());
		Network infoOfDB = null;
		for (int i = 0; i < all.size(); i++) {
			if (all.get(i).getNetworkId() >= 60000000 && all.get(i).getNetworkId() <= 70000000 && (all.get(i).getStatus() == 0 || all.get(i).getStatus() == -1)) {
				infoOfDB = all.get(i);
			}
		}
		Network infoOfOrigin = null;
		if (_info.getNetworkId() >= 60000000 && _info.getNetworkId() <= 70000000 && (_info.getStatus() == 0 || _info.getStatus() == -1)) {
			infoOfOrigin = _info;
		}

		Network infoOfCache = AdapterFactory.getNetworkAdapter().getRegionalNetwork(_info.getUserId());
		Network infoOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getRegionalNetwork(_info.getUserId());

		System.out.println(infoOfOrigin + "\n---\n" + infoOfDB + "\n---\n" + infoOfCache + "\n---\n" + infoOfReaderCache);
		Assert.assertTrue(isEqual(infoOfDB, infoOfOrigin));
		Assert.assertTrue(isEqual(infoOfCache, infoOfOrigin));
		Assert.assertTrue(isEqual(infoOfReaderCache, infoOfOrigin));
	}

	@Test
	public void testIsExistInNetwork() {
		init();

		Network infoOfDB = readDBByInfoId(_info.getNetworkId()).get(0);
		boolean isExistInInfo = _info.getStatus() <= 0;
		boolean isExistInDB = infoOfDB.getStatus() <= 0;
		boolean isExistInCache = AdapterFactory.getNetworkAdapter().isExistInNetwork(_info.getUserId(), _info.getNetworkId());
		boolean isExistInReaderCache = AdapterFactory.getNetworkReaderAdapter().isExistInNetwork(_info.getUserId(), _info.getNetworkId());

		Assert.assertEquals(isExistInDB, isExistInInfo);
		Assert.assertEquals(isExistInCache, isExistInInfo);
		Assert.assertEquals(isExistInReaderCache, isExistInInfo);
	}

	@Test
	public void testReloadNetworkList() {
		init();

		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());
		checkAll();

		// Writer
		init();

		AdapterFactory.getNetworkWriterAdapter().reloadNetworkList(_info.getUserId());
		checkAll();
	}

	@Test
	public void testAddNetwork() {
		Network info = genInfoCopy(1);

		init();

		_infos.add(info);
		AdapterFactory.getNetworkAdapter().addNetwork(info);
		checkAll();

		// Writer
		init();

		// 由于add会改变参数，所以新生成一个info
		info = genInfoCopy(1);
		_infos.add(info);
		AdapterFactory.getNetworkWriterAdapter().addNetwork(info);
		checkAll();
	}

	// //@Test
	// public void testRemoveAllNetwork() {
	// Network info = genInfoCopy(1);
	//
	// init();
	// setDBAndInfos(info.getId(), info);
	// AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());
	//
	// _infos.clear();
	// AdapterFactory.getNetworkAdapter().removeAllNetwork(_info.getUserId());
	// checkAll();
	//
	// // Writer
	// init();
	// setDBAndInfos(info.getId(), info);
	// AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());
	//
	// _infos.clear();
	// AdapterFactory.getNetworkWriterAdapter().removeAllNetwork(_info.getUserId());
	// checkAll();
	// }

	@Test
	public void testRemoveNetworkByStage() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeNetworkByStage(_info.getUserId(), _info.getStage());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeNetworkByStage(_info.getUserId(), _info.getStage());
		checkAll();
	}

	@Test
	public void testRemoveNetwork() {
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().removeNetwork(_info.getUserId(), _info.getNetworkId());
		checkAll();

		// Writer
		init();

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().removeNetwork(_info.getUserId(), _info.getNetworkId());
		checkAll();
	}

	@Test
	public void testSetNetwork() {
		Network info = genInfoCopy(0);

		init();

		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkAdapter().setNetwork(info);
		checkAll();

		// Writer
		init();

		setDBAndInfos(info.getId(), info);
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());

		_infos.remove(_info);
		AdapterFactory.getNetworkWriterAdapter().setNetwork(info);
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
		List<Network> infosOfDB = readDBByUserId(_info.getUserId());
		System.out.println("\ninfosOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infosOfDB);
		return isEqual(infosOfDB, _infos);
	}

	protected boolean isCacheCorrect() {
		List<Network> infosOfCache = AdapterFactory.getNetworkAdapter().getNetworkList(_info.getUserId());
		System.out.println("\ninfosOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfCache);
		return isEqual(infosOfCache, _infos);
	}

	protected boolean isReaderCacheCorrect() {
		List<Network> infosOfReaderCache = AdapterFactory.getNetworkReaderAdapter().getNetworkList(_info.getUserId());
		System.out.println("\ninfosOfReaderCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, _infos);
	}

	protected void setDBAndInfos(int recordId, Network info) {
		String sql = "replace into network_history(id,userid,stage,network_id,network_name,info_id,join_time,status) values (" + recordId + ","
				+ info.getUserId() + "," + info.getStage() + "," + info.getNetworkId() + ",'" + info.getNetworkName() + "'," + info.getInfoId() + ",'"
				+ DateFormatter.format(info.getJoinTime()) + "'," + info.getStatus() + ")";
		writeDB(sql);
		_infos.add(info);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from network_history where userid=" + userId;
		writeDB(sql);
	}

	protected void cleanDBByRecordId(int recordId) {
		String sql = "delete from network_history where id=" + recordId;
		writeDB(sql);
	}

	protected List<Network> readDBByUserId(int userId) {
		String sql = "select * from network_history where userid=" + userId;
		return readDB(sql);
	}

	protected List<Network> readDBByRecordId(int recordId) {
		String sql = "select * from network_history where id=" + recordId;
		return readDB(sql);
	}

	protected List<Network> readDBByInfoId(int infoId) {
		String sql = "select * from network_history where network_id=" + infoId;
		return readDB(sql);
	}

	protected List<Network> readDB(String sql) {
		List<Network> result = new ArrayList<Network>();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("common");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						Network info = new Network();
						info.setId(rs.getInt("id"));
						info.setUserId(rs.getInt("userid"));
						info.setStage(rs.getInt("stage"));
						info.setNetworkId(rs.getInt("network_id"));
						info.setNetworkName(rs.getString("network_name"));
						info.setInfoId(rs.getInt("info_id"));
						info.setJoinTime(DateFormatter.parse(rs.getString("join_time")));
						info.setStatus(rs.getInt("status"));
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
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache/_infos; 2. 初始化DB/Cache/_infos.
	 */
	protected void init() {
		cleanBbAndCache();
		_infos.clear();
		setDBAndInfos(_info.getId(), _info);
		AdapterFactory.getNetworkAdapter().reloadNetworkList(_info.getUserId());
	}

	protected boolean isEqual(List<Network> infos1, List<Network> infos2) {
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

	protected boolean isEqual(Network info1, Network info2) {
		if(info1 == null && info2 == null){
			return true;
		}else{
			return info1.toString().equals(info2.toString());
		}
	}

}