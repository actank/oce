package xce.clusterstate;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import Ice.Current;

import MyUtil.SbjState;
import MyUtil.SbjSubscriber;
import MyUtil.SbjTopic;
import MyUtil.SbjTopicI;

import xce.clusterstate.ClusterSeqState;

public class ClusterStateSubscriber extends SbjSubscriber{

	private String _endpoints;
	private long _interval;
	private Map<String,SbjTopic> _mapTopics=new ConcurrentHashMap<String,SbjTopic>();
	private String[] _clusterState;			//当前的ClusterState数组
	private String[] _oldState;				//更新前的ClusterState数组
	
	
	private static ConcurrentHashMap<String, ClusterStateSubscriber> _instances = new ConcurrentHashMap<String, ClusterStateSubscriber>();

	public static ClusterStateSubscriber getInstance(String instanceName) {
		if (!_instances.containsKey(instanceName)) {
			_instances.putIfAbsent(instanceName, new ClusterStateSubscriber());
		}
		return _instances.get(instanceName);
	}
	
	public ClusterStateSubscriber(){
		
	}
	
	
	void initialize(String endpoints,long interval){
		
		_endpoints=endpoints;
		_interval=interval;
		
		SbjTopicI topic=new SbjTopicI("ClusterState",this,_endpoints,_interval);
		
		Thread thr=new Thread(topic);
		
		thr.start();
		
		_mapTopics.put("ClusterState", topic);
		
	}
	
	public SbjState opera(SbjState state, Current __current) {
		
		if(state.getClass().getName().equals("xce.clusterstate.ClusterSeqState")){
			ClusterSeqState csState=new ClusterSeqState();
			csState=(ClusterSeqState)state;
			
			//备份现有ClusterState
			_oldState=_clusterState;
					
			// 对新赋值
			_clusterState=csState.states;
		}else{
			System.out.println("The type of state is not ClusterSeqState");
		}
		
		SbjState empty=new SbjState();
		return empty;
	}
	
	public String[] getClusterState(){
		return _clusterState;
	}
	
	public String[] getOldClusterState(){
		return _oldState;
	}
}