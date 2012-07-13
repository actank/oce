package xce.buddy.logic.impl;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.antispam.AntiSpamUtils;
import com.xiaonei.platform.core.model.User;
import com.xiaonei.xce.buddygroup.BuddyGroupAdapter;

//TODO:需要实现

public class GroupLogicImpl {

	static List<String> hsGroupName = new ArrayList<String>();
	static List<String> colGroupName = new ArrayList<String>();
	static List<String> groupName = new ArrayList<String>();
	static {
		hsGroupName.add("高中同学");
		hsGroupName.add("家人亲戚");
		hsGroupName.add("挚交好友");
		hsGroupName.add("普通朋友");
		hsGroupName.add("朋友的朋友");
		hsGroupName.add("其他");
		colGroupName.add("大学同学");
		colGroupName.addAll(hsGroupName);
		groupName.add("现在同事");
		groupName.add("以前同事");
		groupName.addAll(colGroupName);

	}

	/** 加默认组 */
	public static void addDefaultGroup(final int hostId, final int stage) {

		List<String> group = null;
		switch (stage) {
		case 10:
			group = hsGroupName;
			break;
		case 20:
			group = colGroupName;
			break;
		case 30:
			group = groupName;
			break;
		default:
			return;
		}

		for (String name : group) {
			GroupLogicImpl.addGroup(hostId, name);
		}
		return;
	}

	/** 添加一个组 */
	public static void addGroup(final int hostId, final String groupName) {
		BuddyGroupAdapter.getInstance().addGroup(hostId, groupName);
	}

	/** 删除一个组 */
	public static void removeGroup(final int hostId, final String groupName) {
		BuddyGroupAdapter.getInstance().removeGroup(hostId, groupName);
	}

	/** 重命名一个组 */
	public static void renameGroup(final int hostId, final String oldName,
			final String newName) {
		BuddyGroupAdapter.getInstance().renameGroup(hostId, oldName, newName);
	}

	/** 将一个好友增加到一个组中 */
	public static void addBuddyToGroup(final int hostId,
			final String groupName, final int buddyId) {
		BuddyGroupAdapter.getInstance().addBuddyToGroup(hostId, groupName,
				buddyId);
	}

	/** 删除一个组中的好友，但不解除好友关系 */
	public static void removeBuddyFromGroup(final int hostId,
			final String groupName, final int buddyId) {
		BuddyGroupAdapter.getInstance().removeBuddyFromGroup(hostId, groupName,
				buddyId);
	}

	/** 批量增加好友到一个组中 */
	public static void addBuddiesToGroup(final int hostId,
			final String groupName, final int[] buddyIds) {
		BuddyGroupAdapter.getInstance().setBuddiesToGroup(hostId, groupName,
				buddyIds);
	}

	/** 将一个好友增加到多个组中（包括只添加到一个组） */
	public static void addBuddyToGroups(final int hostId,
			final String[] groupNames, final int buddyId) {
		BuddyGroupAdapter.getInstance().changeBuddyGroups(hostId, buddyId,
				groupNames);
	}

	/** 清除组中全部好友 */
	public static void cleanGroup(final int hostId, final String groupName) {
		BuddyGroupAdapter.getInstance().cleanGroup(hostId, groupName);
	}

	/** 得到某个好友所在的组 */
	public static List<String> getBuddyGroups(final User host, final int buddy) {
		List<String> groups = BuddyGroupAdapter.getInstance().getBuddyGroups(
				host.getId(), buddy);
		List<String> ret = new ArrayList<String>();
		for (String group : groups) {
			String g = GroupLogicImpl.antispam(host, group);
			if (g.trim().length() == 0) {
				continue;
			}
			ret.add(g);
		}
		return ret;
	}

	/** 取得组列表 */
	public static List<String> getGroups(final User host) {
		List<String> group = BuddyGroupAdapter.getInstance().getGroupList(
				host.getId());
		List<String> sortGroup = new ArrayList<String>();
		for (String str : groupName)
			if (group.contains(str)) {
				String g = GroupLogicImpl.antispam(host, str);
				if (g.trim().length() == 0) {
					continue;
				}
				sortGroup.add(g);
			}
		for (String str : sortGroup) {
			group.remove(str);
		}
		// sortGroup.addAll(group);
		for (String str : group) {
			String g = GroupLogicImpl.antispam(host, str);
			if (g.trim().length() == 0) {
				continue;
			}
			sortGroup.add(g);
		}
		return sortGroup;
	}

	/** 违禁组名 */
	public static boolean isForbiddenGroupName(final String name) {
		return ((name == null) || name.matches("我的好友") || name.matches("陌生人")
				|| name.matches("最近联系人") || name.matches("黑名单")
				|| name.contains("'") || name.contains("\""));
	}

	public static String antispam(final User host, final String content) {

		return AntiSpamUtils.getInstance().htmlSecurityEscape(content);

		/*
		 * CheckResult rs = AntiSpamAdapter.getInstance().checkAndFilter(
		 * host.getId(), CheckType.FRIEND_GROUP_TYPE, content); if
		 * (rs.getM_type() == CheckResult.PROHIBITED) {
		 * 
		 * } else if (rs.getM_type() == CheckResult.SYSTEM_ERROR) return
		 * content; return rs.getM_content() == null ? "" : rs.getM_content();
		 */
	}
}
