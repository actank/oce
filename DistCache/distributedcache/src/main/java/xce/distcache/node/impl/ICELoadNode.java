package xce.distcache.node.impl;

import xce.distcache.node.LoadNode;
import xce.distcache.node.ReadNode;
import xce.distcache.server.LoadServerPrx;
import xce.distcache.server.LoadServerPrxHelper;
import xce.distcache.server.WriteServerPrx;
import xce.distcache.server.WriteServerPrxHelper;
/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-18
 */
public class ICELoadNode extends BaseNode implements LoadNode {
	private Ice.Communicator _ic;
	private LoadServerPrx _prx;

	@Override
	public void initialize(String name, String config) {
		super.initialize(name, config);
		/*
		 * The config string format is ICE endpoins format. Example: M:tcp -h
		 * 1.2.3.4 -p 5677
		 */
		this._ic = Ice.Util.initialize();
		this._prx = LoadServerPrxHelper.uncheckedCast(_ic
				.stringToProxy(config));
	}

	@Override
	public void close() {
		// TODO Auto-generated method stub
		if (_ic != null) {
		      _ic.destroy();
		    }
	}

	@Override
	public void loadByNode() {
		// TODO Auto-generated method stub
		_prx.loadByNode();
	}

}