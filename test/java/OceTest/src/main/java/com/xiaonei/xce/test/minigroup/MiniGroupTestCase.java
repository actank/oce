/**
 * 测试目的：测试MiniGroup的功能
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.minigroup;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.minigroup.MiniGroupAdapter;
import com.xiaonei.xce.minigroup.MiniGroupInfo;

@RunWith(Parameterized.class)
public class MiniGroupTestCase {
	private static int testCount = 0;
	private final MiniGroupInfo _info;
	private final int _userId;

	public MiniGroupTestCase(int id, String name, String icon, int userId) {
		_info = extracted(id, name, icon);
		_userId = userId;
	}

  private MiniGroupInfo extracted(int id, String name, String icon) {
    return new com.xiaonei.xce.minigroup.MiniGroupInfo(id, name, icon);
  }

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { { 900000000, "name", "icon", 800004444 },
		// {
		// 844444444, "name", "icon", 800004444
		// },
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
	 * 测试程序本身的DB读写模块
	 */
	@Test
	public void testDB() {
		// 初始化测试数据
		int userId = _userId;
		int count = 2;
		List<Long> groupIdList = genGroupList(_info.getId(), count);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());

		DB.removeJoinedGroup(userId);
		DB.removeGroupInfo(groupIdList);
		
		DB.addGroupInfo(groupInfoList);
		Assert.assertTrue(isEqual(DB.getGroupInfo(groupIdList), groupInfoList));

		DB.addJoinedGourp(userId, groupIdList);
		System.out.println(DB.getJoinedGourp(userId));
		Assert.assertTrue(isEqual(DB.getGroupInfo(DB.getJoinedGourp(userId)), groupInfoList));

		DB.removeJoinedGroup(userId);
		Assert.assertEquals(DB.getJoinedGourp(userId).size(), 0);

		DB.removeGroupInfo(groupIdList);
		Assert.assertEquals(DB.getGroupInfo(groupIdList).size(), 0);
	}

	/**
	 * 测试get和reload方法
	 */
	@Test
	public void testGetMiniGroups() {
		// 初始化测试数据
		int count = 2;
		List<Long> groupIdList = genGroupList(_info.getId(), count);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());

		// 初始化DB数据
		DB.removeGroupInfo(groupIdList);
		DB.addGroupInfo(groupInfoList);

		// 初始化缓存
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdList);

		// 测试
		List<MiniGroupInfo> groupInfoListOfCache = MiniGroupAdapter.getInstance().getMiniGroups(groupIdList);
		Assert.assertTrue(isEqual(groupInfoList, groupInfoListOfCache));
	}

	@Test
	public void testAddMiniGroups() {
		testAddMiniGroupsUnit(0, 0, 0); // 原来0个，新增0个，覆盖0个
		testAddMiniGroupsUnit(1, 0, 1); // 原来1个，新增0个，覆盖1个
		testAddMiniGroupsUnit(1, 1, 1); // 原来1个，新增1个，覆盖0个
		testAddMiniGroupsUnit(2, 0, 1); // 原来2个，减少1个，覆盖0个
		testAddMiniGroupsUnit(2, 0, 2); // 原来2个，新增0个，覆盖2个
		testAddMiniGroupsUnit(2, 1, 1); // 原来2个，减少1个，覆盖1个
		testAddMiniGroupsUnit(2, 1, 2); // 原来2个，减少1个，新增1个，覆盖1个
		testAddMiniGroupsUnit(2, 2, 1); // 原来2个，减少2个，新增1个，覆盖0个
		testAddMiniGroupsUnit(2, 2, 2); // 原来2个，减少2个，新增2个，覆盖0个
	}

	/**
	 * @param originCount
	 *            初始小群数量，连续
	 * @param addCount
	 *            要增加的小群数量，连续
	 */
	public void testAddMiniGroupsUnit(int originCount, int addOffset, int addCount) {
		// 初始化测试数据
		List<Long> groupIdList = genGroupList(_info.getId(), originCount);
		System.out.println(groupIdList);
		List<Long> groupIdListAdd = genGroupList(_info.getId() + addOffset, addCount);
		System.out.println(groupIdListAdd);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());
		List<MiniGroupInfo> groupInfoListAdd = genGroupInfoList(groupIdListAdd, _info.getName() + "-add", _info.getIcon() + "-add");
		List<MiniGroupInfo> groupInfoListRemove = genGroupInfoList(groupIdListAdd, _info.getName(), _info.getIcon());

		// 初始化DB数据
		DB.removeGroupInfo(groupIdList);
		DB.addGroupInfo(groupInfoList);

		// 初始化Cache数据
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdList);

		// 测试
		groupInfoList.removeAll(groupInfoListRemove);
		groupInfoList.addAll(groupInfoListAdd);
		groupIdList.removeAll(groupIdListAdd);
		groupIdList.addAll(groupIdListAdd);
		MiniGroupAdapter.getInstance().addMiniGroups(groupInfoListAdd);
		List<MiniGroupInfo> groupInfoListOfCache = MiniGroupAdapter.getInstance().getMiniGroups(groupIdList);
		Assert.assertTrue(isEqual(groupInfoList, groupInfoListOfCache));
	}

	/**
	 * 测试get和reload方法
	 */
	@Test
	public void testGetJoinedMiniGroups() {
		// 初始化测试数据
		int userId = _userId;
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(userId);
		int count = 2;
		List<Long> groupIdList = genGroupList(_info.getId(), count);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());

		// 初始化DB数据
		DB.removeJoinedGroup(userId);
		DB.removeGroupInfo(groupIdList);
		DB.addGroupInfo(groupInfoList);
		DB.addJoinedGourp(userId, groupIdList);
		System.out.println("userId = " + userId + " groupIdList  = " + groupIdList);
		// 初始化Cache数据
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdList);
		MiniGroupAdapter.getInstance().reloadUsers(userIds);

		// 测试
		List<MiniGroupInfo> groupInfoListOfCache = MiniGroupAdapter.getInstance().getJoinedMiniGroups(userId);
		Assert.assertTrue(isEqual(groupInfoList, groupInfoListOfCache));
	}

	@Test
	public void testAddJoinedMiniGroups() {
		testAddJoinedMiniGroupsUnit(0, 1);
		testAddJoinedMiniGroupsUnit(1, 0);
		testAddJoinedMiniGroupsUnit(1, 1);
		testAddJoinedMiniGroupsUnit(1, 2);
		testAddJoinedMiniGroupsUnit(2, 1);
		testAddJoinedMiniGroupsUnit(2, 2);
	}

	/**
	 * @param originCount
	 *            初始小群数量，连续
	 * @param addCount
	 *            要增加的小群数量，连续
	 */
	public void testAddJoinedMiniGroupsUnit(int originCount, int addCount) {
		System.out.println("originCount = " + originCount + ", addCount = " + addCount);
		// 初始化测试数据
		int userId = _userId;
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(userId);
		List<Long> groupIdList = genGroupList(_info.getId(), originCount);
		System.out.println("Origin:" + groupIdList);
		List<Long> groupIdListAdd = genGroupList(_info.getId() + originCount, addCount);
		System.out.println("Add:" + groupIdListAdd);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());
		List<MiniGroupInfo> groupInfoListAdd = genGroupInfoList(groupIdListAdd, _info.getName(), _info.getIcon());

		// 初始化DB数据
		DB.removeJoinedGroup(userId);
		DB.removeGroupInfo(groupIdList);
		DB.removeGroupInfo(groupIdListAdd);
		DB.addGroupInfo(groupInfoList);
		DB.addGroupInfo(groupInfoListAdd);
		DB.addJoinedGourp(userId, groupIdList);

		// 初始化Cache数据
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdList);
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdListAdd);
		MiniGroupAdapter.getInstance().reloadUsers(userIds);

		// 测试
		groupInfoList.addAll(groupInfoListAdd);
		groupIdList.addAll(groupIdListAdd);
		MiniGroupAdapter.getInstance().addJoinedMiniGroups(userId, groupIdListAdd);
		List<MiniGroupInfo> groupInfoListOfCache = MiniGroupAdapter.getInstance().getJoinedMiniGroups(userId);
		Assert.assertTrue(isEqual(groupInfoList, groupInfoListOfCache));
	}

	@Test
	public void testSetJoinedMiniGroups() {
		testSetJoinedMiniGroupsUnit(0, 0, 0); // 原来0个，新增0个，覆盖0个
		testSetJoinedMiniGroupsUnit(1, 0, 1); // 原来1个，新增0个，覆盖1个
		testSetJoinedMiniGroupsUnit(1, 1, 1); // 原来1个，新增1个，覆盖0个
		testSetJoinedMiniGroupsUnit(2, 0, 1); // 原来2个，减少1个，覆盖0个
		testSetJoinedMiniGroupsUnit(2, 0, 2); // 原来2个，新增0个，覆盖2个
		testSetJoinedMiniGroupsUnit(2, 1, 1); // 原来2个，减少1个，覆盖1个
		testSetJoinedMiniGroupsUnit(2, 1, 2); // 原来2个，减少1个，新增1个，覆盖1个
		testSetJoinedMiniGroupsUnit(2, 2, 1); // 原来2个，减少2个，新增1个，覆盖0个
		testSetJoinedMiniGroupsUnit(2, 2, 2); // 原来2个，减少2个，新增2个，覆盖0个
	}

	/**
	 * @param originCount
	 *            初始小群数量，连续
	 * @param removeOffset
	 *            要设置的小群开始位置
	 * @param removeCount
	 *            要设置的小群数量，连续
	 */
	public void testSetJoinedMiniGroupsUnit(int originCount, int setOffset, int setCount) {
		// 初始化测试数据
		int userId = _userId;
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(userId);
		List<Long> groupIdList = genGroupList(_info.getId(), originCount);
		System.out.println("Origin:" + groupIdList);
		List<Long> groupIdListSet = genGroupList(_info.getId() + setOffset, setCount);
		System.out.println("Set:" + groupIdListSet);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());
		List<MiniGroupInfo> groupInfoListSet = genGroupInfoList(groupIdListSet, _info.getName(), _info.getIcon());

		// 初始化DB数据
		DB.removeJoinedGroup(userId);
		DB.removeGroupInfo(groupIdList);
		DB.removeGroupInfo(groupIdListSet);
		DB.addGroupInfo(groupInfoList);
		DB.addGroupInfo(groupInfoListSet);
		DB.addJoinedGourp(userId, groupIdList);

		// 初始化Cache数据
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdList);
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdListSet);
		MiniGroupAdapter.getInstance().reloadUsers(userIds);

		// 测试
		MiniGroupAdapter.getInstance().setJoinedMiniGroups(userId, groupIdListSet);
		List<MiniGroupInfo> groupInfoListOfCache = MiniGroupAdapter.getInstance().getJoinedMiniGroups(userId);
		Assert.assertTrue(isEqual(groupInfoListSet, groupInfoListOfCache));
	}

	@Test
	public void testRemoveJoinedMiniGroups() {
		testRemoveJoinedMiniGroupsUnit(4, 0, 2); // 4个删前2个
		testRemoveJoinedMiniGroupsUnit(4, 2, 2); // 4个删后2个
		testRemoveJoinedMiniGroupsUnit(3, 0, 2); // 3个删前2个
		testRemoveJoinedMiniGroupsUnit(3, 1, 2); // 3个删后2个
		testRemoveJoinedMiniGroupsUnit(3, 0, 1); // 3个删前1个
		testRemoveJoinedMiniGroupsUnit(3, 2, 1); // 3个删后1个
		testRemoveJoinedMiniGroupsUnit(2, 0, 2); // 删除所有
		testRemoveJoinedMiniGroupsUnit(2, 1, 2); // 删除1个存在的，1个不存在的
		testRemoveJoinedMiniGroupsUnit(3, 1, 3); // 删除2个存在的，1个不存在的
		testRemoveJoinedMiniGroupsUnit(3, 2, 3); // 删除1个存在的，2个不存在的
		testRemoveJoinedMiniGroupsUnit(4, 2, 4); // 删除2个存在的，2个不存在的
	}

	/**
	 * @param originCount
	 *            初始小群数量，连续
	 * @param removeOffset
	 *            要删除的小群开始位置
	 * @param removeCount
	 *            要删除的小群数量，连续
	 */
	public void testRemoveJoinedMiniGroupsUnit(int originCount, int removeOffset, int removeCount) {
		// 初始化测试数据
		int userId = _userId;
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(userId);
		List<Long> groupIdList = genGroupList(_info.getId(), originCount);
		System.out.println("Origin:" + groupIdList);
		List<Long> groupIdListRemove = genGroupList(_info.getId() + removeOffset, removeCount);
		System.out.println("Remove:" + groupIdListRemove);
		List<MiniGroupInfo> groupInfoList = genGroupInfoList(groupIdList, _info.getName(), _info.getIcon());
		List<MiniGroupInfo> groupInfoListRemove = genGroupInfoList(groupIdListRemove, _info.getName(), _info.getIcon());

		// 初始化DB数据
		DB.removeJoinedGroup(userId);
		DB.removeGroupInfo(groupIdList);
		DB.removeGroupInfo(groupIdListRemove);
		DB.addGroupInfo(groupInfoList);
		DB.addGroupInfo(groupInfoListRemove);
		DB.addJoinedGourp(userId, groupIdList);

		// 初始化Cache数据
		MiniGroupAdapter.getInstance().reloadMiniGroups(groupIdList);
		MiniGroupAdapter.getInstance().reloadUsers(userIds);

		// 测试
		groupInfoList.removeAll(groupInfoListRemove);
		MiniGroupAdapter.getInstance().removeJoinedMiniGroups(userId, groupIdListRemove);
		List<MiniGroupInfo> groupInfoListOfCache = MiniGroupAdapter.getInstance().getJoinedMiniGroups(userId);
		Assert.assertTrue(isEqual(groupInfoList, groupInfoListOfCache));
	}

	static class DB {
		// JoinedGroup
		// ------------------------------------------------------
		public static void addJoinedGourp(int userId, List<Long> groupIdList) {
			for (long groupId : groupIdList) {
				String sql = "replace into member_minigroup_" + userId % 100 + "(" + "minigroup_id, member_id, access_id, is_left) values (" + groupId + "," + userId
						+ ", 2, 1)";
				writeDB(sql, "minigroup");
			}
		}

		public static List<Long> getJoinedGourp(int userId) {
			String sql = "select minigroup_id from member_minigroup_" + userId % 100 + " where member_id=" + userId
			+ " and access_id >= 1 and access_id <= 3 and (is_left = 1 or is_left = 0)";
			List<Long> result = new ArrayList<Long>();
			try {
				Connection c = XceAdapter.getInstance().getReadConnection("minigroup");
				try {
					Statement s = c.createStatement();
					try {
						ResultSet rs = s.executeQuery(sql);
						while (rs.next()) {
							long id = rs.getLong("minigroup_id");
							result.add(id);
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

		public static void removeJoinedGroup(int userId) {
			String sql = "delete from member_minigroup_" + userId % 100 + " where member_id=" + userId;
			System.out.println("sql = " + sql);
			writeDB(sql, "minigroup");
		}

		// GroupInfo
		// ------------------------------------------------------
		public static void addGroupInfo(List<MiniGroupInfo> infoList) {
			for (MiniGroupInfo info : infoList) {
				String sql = "replace into minigroup_" + info.getId() % 100 + "(" + "id," + "name," + "icon," + "creater_name," + "creater_headurl,"
						+ "owner_headurl," + "owner_name," + "minigroup_type," + "custom_tiny_headurl," + "custom_large_headurl," + "invite_desc"
						+ ") values (" + info.getId() + ",'" + info.getName() + "','" + info.getIcon() + "','" + "creater_name" + "','" + "creater_headurl"
						+ "','" + "owner_headurl" + "','" + "owner_name" + "','" + "0" + "','" + "custom_tiny_headurl" + "','" + "custom_large_headurl" + "','"
						+ "invite_desc" + "')";
				writeDB(sql, "minigroup");
			}
		}

		public static List<MiniGroupInfo> getGroupInfo(List<Long> groupIdList) {
			List<MiniGroupInfo> result = new ArrayList<MiniGroupInfo>();
			for (long groupId : groupIdList) {
				String sql = "select id,name,icon from minigroup_" + groupId % 100 + " where id=" + groupId;
				try {
					Connection c = XceAdapter.getInstance().getReadConnection("minigroup");
					try {
						Statement s = c.createStatement();
						try {
							ResultSet rs = s.executeQuery(sql);
							while (rs.next()) {
								long id = rs.getLong("id");
								String name = rs.getString("name");
								String icon = rs.getString("icon");
								MiniGroupInfo info = new MiniGroupInfo(id, name, icon);

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
			}
			return result;
		}

		public static void removeGroupInfo(List<Long> groupIdList) {
			for (long groupId : groupIdList) {
				String sql = "delete from minigroup_" + groupId % 100 + " where id=" + groupId;
				writeDB(sql, "minigroup");
			}
		}

		private static void writeDB(String sql, String db) {
			try {
				Connection c = XceAdapter.getInstance().getWriteConnection(db);
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

	private List<Long> genGroupList(long groupId, int count) {
		List<Long> result = new ArrayList<Long>();
		for (int i = 0; i < count; i++) {
			result.add(groupId + i);
		}
		return result;
	}

	private List<MiniGroupInfo> genGroupInfoList(List<Long> groupIdList, String name, String icon) {
		List<MiniGroupInfo> result = new ArrayList<MiniGroupInfo>();
		for (long groupId : groupIdList) {
			MiniGroupInfo info = new MiniGroupInfo(groupId, name + groupId, icon + groupId);
			result.add(info);
		}
		return result;
	}

	private <T> boolean isEqual(List<T> info1, List<T> info2) {
		System.out.println("info1 = " + info1);
		System.out.println("info2 = " + info2);
		return info1.toString().equals(info2.toString());
	}

}