package xce.distcache.server;

import Ice.Communicator;
import Ice.ObjectAdapter;

public class WriteService implements IceBox.Service {

    private ObjectAdapter _adapter;
    private WriteServerI _server;

    @Override
    public void start(String name, Communicator ic, String[] args) {
	this._adapter = ic.createObjectAdapter(name);
	this._server = new WriteServerI();
	// String cluster =
	// ic.getProperties().getProperty("WriteServer.Cluster");
	try {
	    this._server.initialize();
	    this._adapter.add(this._server, ic.stringToIdentity("M"));
	    this._adapter.activate();
	} catch (Throwable e) {
	    e.printStackTrace();
	    System.exit(-1);
	}
	System.out.println("WriteService started");
    }

    @Override
    public void stop() {
	try {
	    this._server.close();
	} catch (InterruptedException e) {
	    e.printStackTrace();
	}
	this._adapter.deactivate();
	this._adapter.destroy();
    }
}
