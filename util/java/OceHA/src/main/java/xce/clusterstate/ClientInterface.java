package xce.clusterstate;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import xce.serverstate.ServerStateInfo;
import xce.serverstate.ServerStateInfoMutil;
import xce.util.channel.Channel;
import xce.util.ng.channel.ChannelIF;
import Ice.ObjectPrx;

import com.xiaonei.xce.log.Oce;

public class ClientInterface{

	private final ClusterStateSubscriber clusterState;
	private java.util.Random _random=new java.util.Random();
	private String _endpoints;
	
	public ClientInterface(String endpoints, int interval,Channel c){
	    	_endpoints=endpoints;
		clusterState=ClusterStateSubscriber.getInstance(endpoints,interval,c);
	}
	
	public ClientInterface(String endpoints, int interval){
	    	_endpoints=endpoints;
		clusterState=ClusterStateSubscriber.getInstance(endpoints,interval,Channel.newOceChannel(endpoints));
	}
	public ClientInterface(ChannelIF channel, String endpoints, int interval,Channel c){
		clusterState=ClusterStateSubscriberMC.getInstance(channel,endpoints,interval,c);
	}
	public ClientInterface(ChannelIF channel, String endpoints, int interval){
		clusterState=ClusterStateSubscriberMC.getInstance(channel,endpoints,interval,Channel.newOceChannel(endpoints));
	}
	
	// 得到可用的服务
	public Map<Integer, ServerStateInfo[]> getAvailableObjProxies(){
		return clusterState.getClusterState();
	}
	
	public int getCluster(){
		return clusterState.getCluster();
	}
	
	public Ice.ObjectPrx getValidProxy(int userid){
		
		if ( clusterState == null ) {
			Oce.getLogger().error(this.getClass().getName() + ".getValidProxy "+" "+_endpoints+" " + userid + " clusterState_is_null");
			return null;
		}
		Map<Integer, ServerStateInfo[]> validstate = clusterState.getValidState();
		ServerStateInfo[] nowPrx = null;
		if ( validstate != null ) {					
				nowPrx = validstate.get(userid%clusterState.getCluster());		
				// ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
				if(nowPrx != null && nowPrx.length > 0){
					int pos=Math.abs(_random.nextInt(nowPrx.length))%nowPrx.length;
					return nowPrx[pos].prx;
				}
		}
		
		Map<Integer, ServerStateInfo[]> validBackup = clusterState.getValidBackup();
		ServerStateInfo[] backupPrx = null;
		if ( validBackup != null ) {
					backupPrx = validBackup.get(userid%clusterState.getCluster());
					if(backupPrx != null && backupPrx.length > 0){
						int pos = Math.abs(_random.nextInt(backupPrx.length))%backupPrx.length;
						return backupPrx[pos].prx;
					}
		}
		
		if ( (nowPrx==null) && (backupPrx==null) ) {
			Oce.getLogger().error(this.getClass().getName() + ".getValidProxy "+" "+_endpoints+" " + userid + " " + "nowPrxs_and_backupPrxs_are_both_null");
		} else if (backupPrx != null && backupPrx.length <= 0) {
			Oce.getLogger().error(this.getClass().getName() + ".getValidProxy "+" "+_endpoints+" " + userid + " " + "backupPrxs_is_empty");			
		} else if (nowPrx !=null && nowPrx.length <= 0) {
			Oce.getLogger().error(this.getClass().getName() + ".getValidProxy "+" "+_endpoints+" " + userid + " " + "validPrxs_and_backupPrxs_are_empty");
		}
		return null;
	}
	
	//从一个controller取得同个进程中的多个服务的adapter

	public Ice.ObjectPrx getValidProxy(int userid, String strServiceName)
	{
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx != null && nowPrx.length > 0){
			int pos=Math.abs(_random.nextInt(nowPrx.length))%nowPrx.length;
			return getPrxFromMap(nowPrx[pos],strServiceName);
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx != null && oldPrx.length > 0){
			int pos = Math.abs(_random.nextInt(oldPrx.length))%oldPrx.length;
			return getPrxFromMap(oldPrx[pos],strServiceName);
		}
		return null;
	}
	
	public List getAllProxy(int userid, String strServiceName) {
		List result = new ArrayList<ObjectPrx>();
		ServerStateInfo[] nowPrx = clusterState.getValidState().get(userid%clusterState.getCluster());
		if(nowPrx != null && nowPrx.length > 0){
			for(int i = 0; i < nowPrx.length; ++i) {
				result.add(getPrxFromMap(nowPrx[i],strServiceName));
			}
		}
		ServerStateInfo[] oldPrx = clusterState.getValidBackup().get(userid%clusterState.getCluster());
		if(oldPrx != null && oldPrx.length > 0){
			for(int i = 0; i < oldPrx.length; ++i) {
				result.add(getPrxFromMap(oldPrx[i],strServiceName));
			}
		}
		return result;
	}

	private Ice.ObjectPrx getPrxFromMap(ServerStateInfo sst,String strName)
	{ 
		ServerStateInfoMutil ssim = (ServerStateInfoMutil)sst;
		return ssim.prxMap.get(strName);
	}

	// 按照id返回有效prx
	public Ice.ObjectPrx getProxy(long id) {		
		
		Long modLong = (id % clusterState.getCluster());		
		int mod = modLong.intValue();	
		
		Map<Integer, ServerStateInfo[]> mapPrx = getAvailableObjProxies();	
		if ( mapPrx.containsKey(mod)) {	
			ServerStateInfo[] seqPrx = mapPrx.get(mod);
			java.util.Random r=new java.util.Random();
			int pos = Math.abs(r.nextInt()) % seqPrx.length;		
			return seqPrx[pos].prx;
		}
		else {			
			return null;
		}
	}
}

