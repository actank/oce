package xce.replicaclusterdemo;

import xce.serverstate.ServerStateSubscriber;
import Ice.Current;

public class ReplicaClusterDemoI extends _repcludemoManagerDisp{
	
	private ServerStateSubscriber serverState;
	
	public ReplicaClusterDemoI(String servicename){
		
		serverState=ServerStateSubscriber.getInstance("Repcludemo");
		
		serverState.initialize(servicename,"ControllerFootprint", 300);

	}
	
	public void printprx(String bprx, Current __current) {
		System.out.println("The prx is : "+bprx);
	}
}
