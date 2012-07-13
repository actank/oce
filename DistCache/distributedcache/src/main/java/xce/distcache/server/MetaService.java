package xce.distcache.server;

import java.io.FileNotFoundException;
import java.io.IOException;

import org.apache.zookeeper.KeeperException;

import Ice.Communicator;
import Ice.ObjectAdapter;

public class MetaService implements IceBox.Service {

    private ObjectAdapter _adapter;
    private MetaServerI _server;

    @Override
    public void start(String name, Communicator ic, String[] args) {
	this._adapter = ic.createObjectAdapter(name);
	this._server = new MetaServerI();
	String datadir = ic.getProperties().getPropertyWithDefault(
		"MetaServer.DataDir", "data/db");
	try {
	    this._server.initialize(datadir);
	    this._adapter.add(this._server, ic.stringToIdentity("M"));
	    this._adapter.activate();
	} catch (FileNotFoundException e) {
	    e.printStackTrace();
	    System.exit(-1);
	} catch (IOException e) {
	    e.printStackTrace();
	    System.exit(-1);
	} catch (KeeperException e) {
	    e.printStackTrace();
	    System.exit(-1);
	} catch (InterruptedException e) {
	    e.printStackTrace();
	    System.exit(-1);
	}
	System.out.println("MetaService started");
    }

    @Override
    public void stop() {
	this._server.close();
	this._adapter.deactivate();
	this._adapter.destroy();
    }
}
