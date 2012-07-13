package com.xiaonei.xce.buddyadapter.buddygroup;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.model.BuddyGroupManagerPrx;
import com.xiaonei.model.BuddyGroupManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

/**
 * BuddyGroupAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public class BuddyGroupAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerBuddyGroup";
	private static int _interval = 120;

	private static BuddyGroupAdapter instance = new BuddyGroupAdapter();

	public static BuddyGroupAdapter getInstance() {
		return instance;
	}

	protected BuddyGroupAdapter() {
		super(ctr_endpoints, _interval);
		// super(endpoints, 10, Channel.newOceChannel(endpoints));
	}

	/**
	 * @param id
	 * @return
	 */
	private BuddyGroupManagerPrx getManagerProxy(int id) {
		Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if (prx0 != null) {
			BuddyGroupManagerPrx prx = BuddyGroupManagerPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param id
	 * @return
	 */
	private BuddyGroupManagerPrx getManagerProxyOneway(int id) {
		Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if (prx0 != null) {
			BuddyGroupManagerPrx prx = BuddyGroupManagerPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}

	}

	// ----------------------------
	/**
	 * @param host
	 * @param groupName
	 * @param buddy
	 */
	public void addBuddyToGroup(int host, String groupName, int buddy) {

		getManagerProxyOneway(host).addBuddyToGroup(host, groupName, buddy);
	}

	/**
	 * @param host
	 * @param groupId
	 * @param buddy
	 */
	public void addBuddyToGroup(int host, int groupId, int buddy) {

		getManagerProxyOneway(host).addBuddyToGroupWithGid(host, groupId, buddy);
	}

	/**
	 * @param host
	 * @param groupName
	 * @param buddies
	 */
	public void addBuddiesToGroup(int host, String groupName, int[] buddies) {

		getManagerProxyOneway(host).addBuddiesToGroup(host, groupName, buddies);
	}

	/**
	 * @param host
	 * @param groupId
	 * @param buddies
	 */
	public void addBuddiesToGroup(int host, int groupId, int[] buddies) {

		getManagerProxyOneway(host).addBuddiesToGroupWithGid(host, groupId, buddies);
	}
	
	/**
	 * @param host
	 * @param groupName
	 * @param buddies
	 */
	public void setBuddiesToGroup(int host, String groupName, int[] buddies) {

		getManagerProxyOneway(host).setBuddiesToGroup(host, groupName, buddies);
	}

	/**
	 * @param host
	 * @param groupId
	 * @param buddies
	 */
	public void setBuddiesToGroup(int host, int groupId, int[] buddies) {

		getManagerProxyOneway(host).setBuddiesToGroupWithGid(host, groupId, buddies);
	}
	
	/**
	 * @param host
	 * @param buddy
	 * @param nameSeq
	 */
	public void changeBuddyGroups(int host, int buddy, String[] nameSeq) {
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("FROM", "WEB");
		getManagerProxyOneway(host).changeBuddyGroupsForWeb(host, buddy, nameSeq,
				ctx);
	}

	/**
	 * @param host
	 * @param buddy
	 * @param groupIdSeq
	 */
	public void changeBuddyGroups(int host, int buddy, int[] groupIdSeq) {
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("FROM", "WEB");
		getManagerProxyOneway(host).changeBuddyGroupsForWebWithGid(host, buddy, groupIdSeq,
				ctx);
	}
	
	/**
	 * @param host
	 * @param groupName
	 * @param buddy
	 */
	public void removeBuddyFromGroup(int host, String groupName, int buddy) {

		getManagerProxyOneway(host).removeBuddyFromGroup(host, groupName, buddy);
	}

	/**
	 * @param host
	 * @param groupId
	 * @param buddy
	 */
	public void removeBuddyFromGroup(int host, int groupId, int buddy) {

		getManagerProxyOneway(host).removeBuddyFromGroupWithGid(host, groupId, buddy);
	}
	
	/**
	 * @param host
	 * @param groupName
	 */
	public void addGroup(int host, String groupName) {

		getManagerProxyOneway(host).addGroup(host, groupName);
	}

	/**
	 * @param host
	 * @param groupName
	 */
	public void removeGroup(int host, String groupName) {

		getManagerProxyOneway(host).removeGroup(host, groupName);
	}

	/**
	 * @param host
	 * @param groupId
	 */
	public void removeGroup(int host, int groupId) {

		getManagerProxyOneway(host).removeGroupWithGid(host, groupId);
	}
	
	/**
	 * @param host
	 * @param groupId
	 * @param newName
	 */
	public void renameGroup(int host, int groupId, String newName) {

		getManagerProxyOneway(host).renameGroupWithGid(host, groupId, newName);
	}

	/**
	 * @param host
	 * @param groupName
	 */
	public void cleanGroup(int host, String groupName) {

		// getManagerProxyOneway(host).cleanGroup(host, groupName);
		BuddyGroupManagerPrx prx = null;
		try {
			prx = getManagerProxy(host);
			prx.cleanGroup(host, groupName);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".cleanGroup " + host
							+ " Ice.ConnectTimeoutException " + prx);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".cleanGroup " + host
							+ " Ice.TimeoutException " + prx);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".cleanGroup " + host + prx + " ", e);
		} finally {
		}

	}

	/**
	 * @param host
	 * @param groupId
	 */
	public void cleanGroup(int host, int groupId) {

		// getManagerProxyOneway(host).cleanGroup(host, groupName);
		BuddyGroupManagerPrx prx = null;
		try {
			prx = getManagerProxy(host);
			prx.cleanGroupWithGid(host, groupId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".cleanGroup " + host
							+ " Ice.ConnectTimeoutException " + prx);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".cleanGroup " + host
							+ " Ice.TimeoutException " + prx);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".cleanGroup " + host + prx + " ", e);
		} finally {
		}

	}
	
	/**
	 * @param host
	 * @return
	 */
	public List<String> getGroupList(int host) {

		List<String> groups = new ArrayList<String>();
		BuddyGroupManagerPrx prx = null;
		try {
			prx = getManagerProxy(host);
			String[] glist = prx.getGroupList(host);
			for (int i = 0; i < glist.length; ++i) {
				groups.add(glist[i]);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getGroupList " + host
							+ " Ice.ConnectTimeoutException " + prx);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getGroupList " + host
							+ " Ice.TimeoutException " + prx);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getGroupList " + host + prx + " ", e);
		} finally {
		}
		return groups;
	}

	/**
	 * @param host
	 * @return
	 */
	public Map<Integer,String> getBuddyGroupList(int host) {

		Map<Integer,String> groups = new HashMap<Integer,String>();
		BuddyGroupManagerPrx prx = null;
		try {
			prx = getManagerProxy(host);
			groups = prx.getGroupListData(host);
			
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getGroupList " + host
							+ " Ice.ConnectTimeoutException " + prx);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getGroupList " + host
							+ " Ice.TimeoutException " + prx);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getGroupList " + host + prx + " ", e);
		} finally {
		}
		return groups;
	}
	
	/**
	 * 
	 * @param host
	 * @param groupName
	 * @param begin
	 * @param limit
	 *          get all , set -1
	 * @return
	 */
	public List<Integer> getBuddyList(int host, String groupName, int begin,
			int limit) {

		int[] ids = getManagerProxy(host).getBuddyList(host, groupName, begin,
				limit);
		List<Integer> buddyList = new ArrayList<Integer>();
		for (int i = 0; i < ids.length; ++i) {
			buddyList.add(ids[i]);
		}
		return buddyList;
	}

	/**
	 * 
	 * @param host
	 * @param groupName
	 * @param begin
	 * @param limit
	 *          get all , set -1
	 * @return
	 */
	public List<Integer> getBuddyList(int host, int groupId, int begin,
			int limit) {

		int[] ids = getManagerProxy(host).getBuddyListWithGid(host, groupId, begin,
				limit);
		List<Integer> buddyList = new ArrayList<Integer>();
		for (int i = 0; i < ids.length; ++i) {
			buddyList.add(ids[i]);
		}
		return buddyList;
	}
	
	/**
	 * @param host
	 * @param groupName
	 * @return
	 */
	public int getCountByGroup(int host, String groupName) {
		return getManagerProxy(host).getCountByGroup(host, groupName);
	}
	
	/**
	 * @param host
	 * @param groupId
	 * @return
	 */
	public int getBuddyGroupsCount(int host, int groupId) {
		return getManagerProxy(host).getCountByGroupId(host, groupId);
	}

	/**
	 * @param host
	 * @param buddy
	 * @return
	 */
	public List<String> getBuddyGroups(int host, int buddy) {
		String[] groups = getManagerProxy(host).getBuddyGroups(host, buddy);
		List<String> g = new ArrayList<String>();
		for (int i = 0; i < groups.length; ++i) {
			g.add(groups[i]);
		}
		return g;
	}

	/**
	 * @param host
	 * @param buddy
	 * @return
	 */
	public Map<Integer, String> getBuddyBelongGroups(int host, int buddy) {
		return  getManagerProxy(host).getBuddyGroupsData(host, buddy);
	}
	
	/**
	 * @param host
	 * @return
	 */
	public Map<String, Integer> getAllGroupsBuddyCount(int host) {
		return getManagerProxy(host).getAllGroupsBuddyCount(host);
	}

	/**
	 * @param host
	 * @return
	 */
	public Map<Integer, Integer> getBuddyGroupsCount(int host) {
		return getManagerProxy(host).getAllGroupsBuddyCountData(host);
	}
	
	/**
	 * @param host
	 * @return
	 */
	public Map<Integer, List<String>> getBuddiedGroupsMap(int host) {
		Map<Integer, List<String>> res = new HashMap<Integer, List<String>>();
		Map<Integer, String[]> tmp = getManagerProxy(host)
				.getBuddiedGroupsMap(host);
		for (java.util.Map.Entry<Integer, String[]> en : tmp.entrySet()) {
			String[] groups = en.getValue();
			List<String> l = new ArrayList<String>();
			for (int i = 0; i < groups.length; i++) {
				l.add(groups[i]);
			}
			res.put(en.getKey(), l);
		}
		return res;
	}
	
	/**
	 * 
	 * @param host
	 *          
	 * @return Map<Integer, Map<Integer, String> > 
	 * 						Map< UserId, Map< GroupId, GroupName> >
	 */
	public Map<Integer, Map<Integer, String> > getBuddyGroupsMap(int host){
		Map<Integer, Map<Integer,String>> res = new HashMap<Integer, Map<Integer,String>>();
		BuddyGroupManagerPrx prx = null;
		try {
			prx = getManagerProxy(host);
			res = getManagerProxy(host).getBuddyGroupsMap(host);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyGroupsMap " + host
							+ " Ice.ConnectTimeoutException " + prx);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyGroupsMap " + host
							+ " Ice.TimeoutException " + prx);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyGroupsMap " + host + " " + prx + " ", e);
		} finally {
		}
		return res;
	}
	
	
}
