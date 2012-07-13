package xce.serverstate;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import xce.util.channel.Channel;
import Ice.Current;
import MyUtil.SbjState;
import MyUtil.SbjSubscriber;
import MyUtil.SbjTopic;
import MyUtil.SbjTopicI;

public class ServerStateSubscriber extends SbjSubscriber{

	private String _endpoints;
	private long _interval;
	private Map<String, SbjTopic> _mapTopics=new ConcurrentHashMap<String, SbjTopic>();
	
	private static ConcurrentHashMap<String, ServerStateSubscriber> _instances = new ConcurrentHashMap<String, ServerStateSubscriber>();

	public static ServerStateSubscriber getInstance(String instanceName) {
		if (!_instances.containsKey(instanceName)) {
			_instances.putIfAbsent(instanceName, new ServerStateSubscriber());
		}
		return _instances.get(instanceName);
	}
	
	public ServerStateSubscriber(){
		
	}
public void initialize(String endpoints, String controlEndpoints,long interval){
		
		_endpoints=endpoints;
		_interval=interval;
		
		SbjTopicI topic=new SbjTopicI("ServerState",this,controlEndpoints,(int)_interval, Channel.newOceChannel(endpoints));
		
		Thread thr=new Thread(topic);
		thr.setDaemon(true);
		
		thr.start();
		
		_mapTopics.put("ServerState", topic);
		
	}

	public void initialize(String endpoints, String controlEndpoints,long interval, Channel channel){
		
		_endpoints=endpoints;
		_interval=interval;
		
		SbjTopicI topic=new SbjTopicI("ServerState",this,controlEndpoints,(int)_interval, channel);
		
		Thread thr=new Thread(topic);
		thr.setDaemon(true);
		
		thr.start();
		
		_mapTopics.put("ServerState", topic);
		
	}

	public SbjState opera(SbjState state, Current __current) {
		
		ServerStateInfo ssState=new ServerStateInfo();
		ssState.servername=_endpoints;
		ssState.status=1;	/* 默认的策略是只要能调到这个服务，就返回服务可用 */
		
		return ssState;
	}		
}
