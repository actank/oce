package com.xiaonei.xce.buddygroup;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;
import com.xiaonei.model.BuddyGroupManagerPrx;
import com.xiaonei.model.BuddyGroupManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BuddyGroupAdapter extends ReplicatedClusterAdapter {


	private static final String ctr_endpoints = "ControllerBuddyGroup";
	private static int _interval = 120;

	private static BuddyGroupAdapter instance = new BuddyGroupAdapter();

	public static BuddyGroupAdapter getInstance() {
		return instance;
	}

	protected BuddyGroupAdapter() {
                super(ctr_endpoints, _interval);
		//super(endpoints, 10, Channel.newOceChannel(endpoints));
	}

	private BuddyGroupManagerPrx getManagerProxy(int id) {
              Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if (prx0 != null) {
			BuddyGroupManagerPrx prx = BuddyGroupManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	private BuddyGroupManagerPrx getManagerProxyOneway(int id) {
                Ice.ObjectPrx prx0 = getValidProxy(id, 300);
                if (prx0 != null) {
                        BuddyGroupManagerPrx prx = BuddyGroupManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
                        return prx;
                } else {
                        return null;
                }

 
	}
//----------------------------
	public void addBuddyToGroup(int host, String groupName, int buddy) {
		
		getManagerProxyOneway(host).addBuddyToGroup(host, groupName, buddy);
	}

	public void addBuddiesToGroup(int host, String groupName, int[] buddies) {
		
		getManagerProxyOneway(host).addBuddiesToGroup(host, groupName, buddies);
	}

	public void setBuddiesToGroup(int host, String groupName, int[] buddies) {

                getManagerProxyOneway(host).setBuddiesToGroup(host, groupName, buddies);
        }

	public void changeBuddyGroups(int host, int buddy, String[] nameSeq) {
		Map<String,String> ctx = new HashMap<String,String>();
		ctx.put("FROM", "WEB");
		getManagerProxyOneway(host).changeBuddyGroupsForWeb(host, buddy, nameSeq, ctx);
	}

	public void removeBuddyFromGroup(int host, String groupName, int buddy) {
		
		getManagerProxyOneway(host)
				.removeBuddyFromGroup(host, groupName, buddy);
	}

	public void addGroup(int host, String groupName) {
		
		getManagerProxyOneway(host).addGroup(host, groupName);
	}

	public void removeGroup(int host, String groupName) {
		
		getManagerProxyOneway(host).removeGroup(host, groupName);
	}

	public void renameGroup(int host, String oldName, String newName) {
		
		getManagerProxyOneway(host).renameGroup(host, oldName, newName);
	}

	public void cleanGroup(int host, String groupName) {
	
		//getManagerProxyOneway(host).cleanGroup(host, groupName);
		BuddyGroupManagerPrx prx = null;
		try{
			prx = getManagerProxy(host);		
			prx.cleanGroup(host,groupName);
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".cleanGroup " + host + " Ice.ConnectTimeoutException " + prx);
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".cleanGroup " + host + " Ice.TimeoutException " + prx);
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error( this.getClass().getName() + ".cleanGroup " + host + prx + " ", e);
        } finally {
        }

	}

	public List<String> getGroupList(int host) {
	
		List<String> groups = new ArrayList<String>();
		BuddyGroupManagerPrx prx = null;
		try{
			prx = getManagerProxy(host);
			String[] glist = prx.getGroupList(host);
			for (int i = 0; i < glist.length; ++i) {
                                groups.add(glist[i]);
                        }
		} catch (Ice.ConnectTimeoutException e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getGroupList " + host + " Ice.ConnectTimeoutException " + prx);
                } catch (Ice.TimeoutException e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getGroupList " + host + " Ice.TimeoutException " + prx);
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getGroupList " + host + prx + " ", e);
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
	 *            get all , set -1
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

	public int getCountByGroup(int host, String groupName) {
		return getManagerProxy(host).getCountByGroup(host, groupName);
	}

	public List<String> getBuddyGroups(int host, int buddy) {
		String[] groups = getManagerProxy(host).getBuddyGroups(host, buddy);
		List<String> g = new ArrayList<String>();
		for (int i = 0; i < groups.length; ++i) {
			g.add(groups[i]);
		}
		return g;
	}
	
	public Map<String, Integer> getAllGroupsBuddyCount(int host) {
		return getManagerProxy(host).getAllGroupsBuddyCount(host);
	}
	public Map<Integer, List<String>> getBuddiedGroupsMap(int host){
		Map<Integer, List<String>> res = new HashMap<Integer, List<String>>();
		Map<Integer, String[]> tmp = getManagerProxy(host).getBuddiedGroupsMap(host);
		for(java.util.Map.Entry<Integer, String[]> en:tmp.entrySet()){
			String[] groups = en.getValue();
			List<String> l = new ArrayList<String>();
			for(int i=0; i<groups.length; i++){
				l.add(groups[i]);
			}
			res.put(en.getKey(), l);
		}
		return res;
	}
	
	public void reload(int host){
		getManagerProxy(host).saveLoad(host);
	}
	public static void main(String[] args){
		BuddyGroupAdapter.getInstance().reload(231351378);
	}
}
