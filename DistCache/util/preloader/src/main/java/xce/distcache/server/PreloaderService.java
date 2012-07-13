package xce.distcache.server;

import xce.distcache.preloader.PreloaderLogger;
import Ice.Communicator;
import Ice.ObjectAdapter;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-06-13
 */
public class PreloaderService implements IceBox.Service {

    private ObjectAdapter _adapter;
    private PreloaderServerI _server;

    @Override
    public void start(String name, Communicator ic, String[] args) {
	// TODO Auto-generated method stub
	PreloaderLogger.info("PreloaderService starting");
	this._adapter = ic.createObjectAdapter(name);
	this._server = new PreloaderServerI();
	try {
	    this._server.initialize();
	    this._adapter.add(this._server, ic.stringToIdentity("M"));
	    this._adapter.activate();
	} catch (Throwable e) {
	    PreloaderLogger.error(e);
	    System.exit(-1);
	    System.out.println("PreloaderService started");
	}
	PreloaderLogger.info("PreloaderService started");
    }

    @Override
    public void stop() {
	// TODO Auto-generated method stub
	try {
	    this._server.close();
	} catch (InterruptedException e) {
	    PreloaderLogger.error(e);
	}
	this._adapter.deactivate();
	this._adapter.destroy();
	PreloaderLogger.info("PreloaderService stop");
    }
}
