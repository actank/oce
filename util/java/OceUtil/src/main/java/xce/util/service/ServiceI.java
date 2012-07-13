package xce.util.service;

import xce.util.log.Log;
import Ice.Communicator;

public abstract class ServiceI implements
		IceBox.Service {

	public void start(String name, Communicator ic, String[] args) {
		_name = name;
		_ic = ic;

		_adapter = ic.createObjectAdapter(name);
		initialize();
		_adapter.activate();
		Log.getInstance().init(name);
		Log.getInstance().info("Service started...");
	}

	public void stop() {
		_adapter.deactivate();
		finishup();
	}

	protected Ice.ObjectAdapter _adapter;
	protected String _name;
	protected Communicator _ic;

	public abstract void initialize();

	public abstract void finishup();
}
