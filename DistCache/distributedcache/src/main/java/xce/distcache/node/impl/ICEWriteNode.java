package xce.distcache.node.impl;

import java.util.Map;
import xce.distcache.node.WriteNode;
import xce.distcache.server.WriteServerPrx;
import xce.distcache.server.WriteServerPrxHelper;

public class ICEWriteNode extends BaseNode implements WriteNode {
    private Ice.Communicator _ic;
    private WriteServerPrx _prx;

    @Override
    public void set(String name, Item id, byte[] data) {
	getPrx().set(name, id.id, id.block, data, true);
    }

    @Override
    public void set(String name, Map<Long, byte[]> bulkDatas) {
	// TODO Auto-generated method stub
	getPrx().setBatch(name, bulkDatas, true);
    }

    public void add(String name, Map<Long, byte[]> bulkDatas) {
	// TODO Auto-generated method stub
	getPrx().addBatch(name, bulkDatas, true);
    }

    @Override
    public void close() {
	if (_ic != null) {
	    _ic.destroy();
	}
    }

    private WriteServerPrx getPrx() {
	if (_prx == null) {
	    synchronized (this) {
		_prx = WriteServerPrxHelper.uncheckedCast(_ic
			.stringToProxy(getConfig()));
	    }
	}
	return _prx;
    }

    @Override
    public void initialize(String name, String config) {
	super.initialize(name, config);
	/*
	 * The config string format is ICE endpoins format. Example: M:tcp -h
	 * 1.2.3.4 -p 5677
	 */
	this._ic = Ice.Util.initialize();
    }
}
