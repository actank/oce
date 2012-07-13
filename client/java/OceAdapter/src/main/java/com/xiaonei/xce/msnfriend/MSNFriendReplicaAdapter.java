package com.xiaonei.xce.msnfriend;

import java.util.Map;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ClientInterface;
import xce.mfd.MSNFriendManagerPrx;
import xce.mfd.MSNFriendManagerPrxHelper;
import xce.mfd.MSNFriend;

import xce.serverstate.ServerStateInfo;


public class MSNFriendReplicaAdapter {

	
	
	private static final String endpoints = "ControllerMSNFriendN";
	
	private static int interval = 120;	

	private static MSNFriendReplicaAdapter instance = new MSNFriendReplicaAdapter();
	
	private static ClientInterface _csClient = new ClientInterface(endpoints, interval);


	public static MSNFriendReplicaAdapter getInstance() {
		return instance;
	}

	private MSNFriendManagerPrx getMSNFriendManager(int userId) {
		
		Ice.ObjectPrx prx0 = _csClient.getProxy(userId);
		if ( prx0 != null ) {
			MSNFriendManagerPrx prx = MSNFriendManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}	
		
	}

	
	public MSNFriend[] getFriendWithCommon(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "MSNFriend "
				+ userId);
    MSNFriendManagerPrx prx = null;
		try {
      prx = getMSNFriendManager(userId);
			return prx.getFriendWithCommon(userId, limit);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("getFriendWithCommon timeout " + userId+" "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return null;
	}
	
	
	public static void main(String[] args){
		try {
			int userId = 222677620;
				
			Ice.ObjectPrx prx0 = _csClient.getProxy(userId);
			if ( prx0 == null) {					
				System.out.println("Prx is null");					
			}
			else {
				MSNFriendReplicaAdapter.getInstance().getMSNFriendManager(userId).getCommonFriend(userId, 0);
				MSNFriendReplicaAdapter.getInstance().getMSNFriendManager(userId).getFriendWithCommon(userId, 0);
				//System.out.println(" : " + prx.toString());
				//System.out.println(" : " + prx.getUserCache(userId).toString());				
			}
				
				
			/* Map<Integer, ServerStateInfo[]> mapPrx = _csClient.getAvailableObjProxies();				
			if(mapPrx!=null){
				for ( int key : mapPrx.keySet()) {
					System.out.print(key);
					System.out.println(" : " + mapPrx.get(key).length);
						for ( int j = 0; j < mapPrx.get(key).length; ++j)  {
							System.out.println("\tmod : " + mapPrx.get(key)[j].mod
									+ mapPrx.get(key)[j].prx.ice_toString());							
					}						
				}					
			} */
				
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			System.exit(0);
	}
}
