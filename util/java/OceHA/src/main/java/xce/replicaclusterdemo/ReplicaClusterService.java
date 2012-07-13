package xce.replicaclusterdemo;

import Ice.Communicator;

public class ReplicaClusterService extends Ice.LocalObjectImpl implements
	IceBox.Service{
	
	protected Ice.ObjectAdapter _adapter;
	protected String _name;
	protected Communicator _ic;
	
	public void finishup(){
		
	}

	public void initialize() {
		
		Ice.Properties properties=_ic.getProperties();

		String servicename=properties.getProperty("Service.Name");
		
		  _adapter.add(new ReplicaClusterDemoI(servicename), Ice.Util.stringToIdentity("Demo"));
		 System.out.println("server start......................................");
	}

	public void start(String name, Communicator ic, String[] args) {
		_name = name;
		_ic = ic;

		_adapter = ic.createObjectAdapter(name);
		initialize();
		_adapter.activate();
	}

	public void stop() {
		_adapter.deactivate();
		finishup();
		
	}

}
