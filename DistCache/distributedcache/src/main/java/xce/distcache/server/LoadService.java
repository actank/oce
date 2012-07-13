package xce.distcache.server;

import Ice.Communicator;
import Ice.ObjectAdapter;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-09
 */
public class LoadService implements IceBox.Service {

	private ObjectAdapter _adapter;
	private LoadServerI _server;
	private LoadMonitor _monitor;

	@Override
	public void start(String name, Communicator ic, String[] args) {
		this._adapter = ic.createObjectAdapter(name);
		this._server = new LoadServerI();
		String clusterName = ic.getProperties().getProperty("LoadServer.Cluster");
		try {
			this._monitor = new LoadMonitor(clusterName, this._server);
			this._server.initialize(clusterName);
			this._adapter.add(this._server, ic.stringToIdentity("M"));
			this._adapter.activate();

		} catch (Throwable e) {
			e.printStackTrace();
			System.exit(-1);
		}
		LoadLogger.info("LoadServer start!");
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