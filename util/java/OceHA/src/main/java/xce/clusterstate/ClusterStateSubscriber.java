package xce.clusterstate;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import xce.clustercontroller.ControllerManagerPrx;
import xce.clustercontroller.ControllerManagerPrxHelper;
import xce.serverstate.ServerStateInfo;
import xce.util.channel.Channel;
import xce.util.ng.channel.ChannelIF;
import Ice.Current;
import MyUtil.SbjState;
import MyUtil.SbjSubscriber;
import MyUtil.SbjTopic;
import MyUtil.SbjTopicI;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.Oce.FakeLogger;

public class ClusterStateSubscriber extends SbjSubscriber{

	/**
	 * 
	 */
	protected static final long serialVersionUID = 1L;
	protected String _endpoints;
	protected int _interval;
	protected int _cluster = 1;
	protected Map<String,SbjTopic> _mapTopics=new ConcurrentHashMap<String,SbjTopic>();	
	protected Map<Integer, ServerStateInfo[]> _validState = new HashMap<Integer, ServerStateInfo[]>();
	protected Map<Integer, ServerStateInfo[]> _validBackupState = new HashMap<Integer, ServerStateInfo[]>();
	protected Map<Integer, ServerStateInfo[]> _clusterState = new HashMap<Integer, ServerStateInfo[]>();	//当前的ClusterState数组
	protected Map<Integer, ServerStateInfo[]> _oldState = new HashMap<Integer, ServerStateInfo[]>();		//更新前的ClusterState数组	
	
	protected static ConcurrentHashMap<String, ClusterStateSubscriber> _instances = new ConcurrentHashMap<String, ClusterStateSubscriber>();
	public static ClusterStateSubscriber getInstance(String instanceName,int interval,Channel c) {
		if (!_instances.containsKey(instanceName)) {
			ClusterStateSubscriber subscriber = new ClusterStateSubscriber(instanceName,interval,c);
			_instances.putIfAbsent(instanceName, subscriber);
			return subscriber;
		} else {
				return _instances.get(instanceName);
		}
	}

	public static ClusterStateSubscriber getInstance(String instanceName,int interval) {
		if (!_instances.containsKey(instanceName)) {
			ClusterStateSubscriber subscriber = new ClusterStateSubscriber(instanceName,interval,Channel.newOceChannel(instanceName));
			_instances.putIfAbsent(instanceName, subscriber);
			return subscriber;
		} else {
				return _instances.get(instanceName);
		}
	}
	
	public static ClusterStateSubscriber getInstance(ChannelIF channel,String instanceName,int interval,Channel c) {
		if (!_instances.containsKey(instanceName)) {
			ClusterStateSubscriber subscriber = new ClusterStateSubscriberMC(channel,instanceName,interval,c);
			_instances.putIfAbsent(instanceName, subscriber);
			return subscriber;
		} else {
				return _instances.get(instanceName);
		}
	}
	
	public static ClusterStateSubscriber getInstance(ChannelIF channel,String instanceName,int interval) {
		if (!_instances.containsKey(instanceName)) {
			ClusterStateSubscriber subscriber = new ClusterStateSubscriberMC(channel,instanceName,interval,Channel.newOceChannel(instanceName));
			_instances.putIfAbsent(instanceName, subscriber);
			return subscriber;
		} else {
				return _instances.get(instanceName);
		}
	}
	
	protected ClusterStateSubscriber() {
		
	}
	
	/*protected ClusterStateSubscriber(String endpoints, int interval){		
		initialize(endpoints,interval);
	}*/
	
	protected ClusterStateSubscriber(String endpoints, int interval, Channel channel){		
		initialize(endpoints,interval,channel);
	}
	
	/*protected void initialize(String endpoints, int interval){
		
		_endpoints=endpoints;
		_interval=interval;
		Channel channel = Channel.newOceChannel(endpoints);
		SbjTopicI topic=new SbjTopicI("ClusterState",this,_endpoints,_interval, channel);
		
		Thread thr=new Thread(topic);		
		thr.start();
		
		_mapTopics.put("ClusterState", topic);		
		
		// 第一次,从controller那里获取初始的prx
		getInitPrx(channel);
	}*/

	protected void initialize(String endpoints, int interval, Channel channel){
		
		_endpoints=endpoints;
		_interval=interval;
		
		SbjTopicI topic=new SbjTopicI("ClusterState",this,_endpoints,_interval, channel);
		
		Thread thr=new Thread(topic);		
		thr.start();
		
		_mapTopics.put("ClusterState", topic);		
		
		// 第一次,从controller那里获取初始的prx
		getInitPrx(channel);
	}
	
	protected void getValidPrx(){
		Map<Integer, List<ServerStateInfo>> tmp=new HashMap<Integer,List<ServerStateInfo>>();
		Map<Integer, List<ServerStateInfo>> tmpBackup = new HashMap<Integer,List<ServerStateInfo>>();
		
		for(Entry<Integer,ServerStateInfo[]> entry:_clusterState.entrySet()){
			for(ServerStateInfo info:entry.getValue()){
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
	
	public SbjState opera(SbjState state, Current __current) {
		
		if(state.getClass().getName().equals("xce.clusterstate.ClusterStateInfo")){			
			
			ClusterStateInfo csState=(ClusterStateInfo)state;					
						
			//备份现有ClusterState
			_oldState=_clusterState;
					
			// 对新赋值			
			Map<Integer, ServerStateInfo[]> tempState=csState.states;
			for(Entry<Integer,ServerStateInfo[]> entry:_clusterState.entrySet()){
					if (!tempState.containsKey(entry.getKey())){
						tempState.put(entry.getKey(), entry.getValue());
					}
			}
			_clusterState=tempState;
			
			// 获取_cluster
			loadCluster(csState.states);
			getValidPrx();
			
		}else{
			Oce.getLogger().error(this.getClass().getName() + ".getInitPrx type_of_state_is_not_ClusterSeqState");			
		}
		
		return new SbjState();
	}
	
	protected void getInitPrx(Channel channel) {		
		try {
			Channel _channel= channel;
			ControllerManagerPrx _ctlPrx = _channel.locate("M", "@"+_endpoints, Channel.Invoke.Twoway, ControllerManagerPrxHelper.class, 300);
			_clusterState = _ctlPrx.GetServerState().states;
			loadCluster(_clusterState);
			getValidPrx();
			FakeLogger logger = Oce.getLogger();
			logger.info(this.getClass().getName() + ".getInitPr Get Init Proxy from " + _endpoints);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getInitPrx " + e);
		}
	}
	
	public int getCluster() {
		return _cluster;
	}
	
	protected void loadCluster(Map<Integer, ServerStateInfo[]> states) {	
		
		if ( ! states.keySet().isEmpty() ) {
			for ( int key : states.keySet()) {
				if ( states.get(key).length >= 0 ) {
					_cluster = states.get(key)[0].cluster;
					return;
				}
			}
		}				
	}
	
	public Map<Integer, ServerStateInfo[]> getValidState(){
		return _validState;
	}
	
	public Map<Integer, ServerStateInfo[]> getValidBackup(){
		return _validBackupState;
	}
	
	public Map<Integer, ServerStateInfo[]> getClusterState(){
		return _clusterState;
	}
	
	public Map<Integer, ServerStateInfo[]> getOldClusterState(){
		return _oldState;
	}
}
