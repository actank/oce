package xce.clusterstate;

public class ClientInterface{

	private ClusterStateSubscriber clusterState;
	
	public ClientInterface(String endpoints, int interval){
		clusterState=ClusterStateSubscriber.getInstance(endpoints);
		clusterState.initialize(endpoints, interval);
	}
	
	//得到可用的服务
	public String[] getAvailableObjProxies(){
		return clusterState.getClusterState();
	}
}

