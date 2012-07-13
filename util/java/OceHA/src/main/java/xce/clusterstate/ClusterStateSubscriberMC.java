package xce.clusterstate;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.serverstate.ServerStateInfo;
import xce.serverstate.ServerStateInfoMutil;
import xce.util.channel.Channel;
import xce.util.ng.channel.ChannelIF;
import Ice.ObjectPrx;

public class ClusterStateSubscriberMC extends ClusterStateSubscriber{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	ChannelIF _channel;
	
	protected ClusterStateSubscriberMC(ChannelIF channel, String endpoints, int interval,Channel c) {
		_channel = channel;
		initialize(endpoints, interval,c);
	}
	
	private void transferToMC( ServerStateInfo info ) {
		if( info.getClass().getName() == "xce.serverstate.ServerStateInfoMutil" ) {
			ServerStateInfoMutil res = (ServerStateInfoMutil)info;
			for( Entry<String, ObjectPrx> entry:res.prxMap.entrySet() ) {
				entry.setValue( _channel.stringToProxy( entry.getValue().toString() ) );
			}
		}else {
			info.prx = _channel.stringToProxy(info.prx.toString());
		}
	}
	
	protected void getValidPrx(){
		Map<Integer, List<ServerStateInfo>> tmp=new HashMap<Integer,List<ServerStateInfo>>();
		Map<Integer, List<ServerStateInfo>> tmpBackup = new HashMap<Integer,List<ServerStateInfo>>();
		
		for(Entry<Integer,ServerStateInfo[]> entry:_clusterState.entrySet()){
			for(ServerStateInfo info:entry.getValue()){
				transferToMC( info );
				if(info.status==1){
					if(!tmp.containsKey(entry.getKey())){
						tmp.put(entry.getKey(), new ArrayList<ServerStateInfo>());
					}
					tmp.get(entry.getKey()).add(info);
				}
				if(info.status==0){
					if(!tmpBackup.containsKey(entry.getKey())){
						tmpBackup.put(entry.getKey(), new ArrayList<ServerStateInfo>());
					}
					tmpBackup.get(entry.getKey()).add(info);
				}
			}
		}
		
		Map<Integer,ServerStateInfo[]> tmpValid=new HashMap<Integer,ServerStateInfo[]>();
		for(Entry<Integer,List<ServerStateInfo>> entry:tmp.entrySet()){
			tmpValid.put(entry.getKey(), entry.getValue().toArray(new ServerStateInfo[entry.getValue().size()]));
		}
		_validState=tmpValid;
		
		Map<Integer,ServerStateInfo[]> tmpBackupValid=new HashMap<Integer,ServerStateInfo[]>();
		for(Entry<Integer,List<ServerStateInfo>> entry:tmpBackup.entrySet()){
			tmpBackupValid.put(entry.getKey(), entry.getValue().toArray(new ServerStateInfo[entry.getValue().size()]));
		}
		_validBackupState=tmpBackupValid;
	}

}
