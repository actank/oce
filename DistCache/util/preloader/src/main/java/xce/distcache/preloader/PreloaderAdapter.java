package xce.distcache.preloader;

import xce.distcache.server.PreloaderServerPrx;
import xce.distcache.server.PreloaderServerPrxHelper;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-06-17
 * 
 *        现在是简单的Adapter形式，为了测试，可以集成为node加入zookeeper，这样就可以连成一个整体。
 */
public class PreloaderAdapter {

    private Ice.Communicator _ic;

    private PreloaderServerPrx _prx;

    private static final PreloaderAdapter _instance = new PreloaderAdapter();

    public static PreloaderAdapter getInstance() {
	return _instance;
    }

    public PreloaderAdapter() {
	this._ic = Ice.Util.initialize();
	this._prx = PreloaderServerPrxHelper.checkedCast(_ic.stringToProxy(
		"M:tcp -h 10.22.206.34 -p 32875").ice_timeout(2000));
    }

    public void PreloaderByName(String name) {
	PreloaderLogger.info("PreloaderAdapter.PreloaderByName, name: " + name);
	_prx.preloaderByName(name);
    }

    public static void main(String[] args) {
	//PreloaderAdapter.getInstance().PreloaderByName("UserCache");
	PreloaderAdapter.getInstance().PreloaderByName("DistSearchCache");
	System.exit(0);
    }
}
