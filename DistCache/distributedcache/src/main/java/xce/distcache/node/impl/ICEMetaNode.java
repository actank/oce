package xce.distcache.node.impl;

import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import xce.distcache.node.MetaNode;
import xce.distcache.server.MetaServerPrx;
import xce.distcache.server.MetaServerPrxHelper;

public class ICEMetaNode extends BaseNode implements MetaNode {
    private Ice.Communicator _ic;
    private MetaServerPrx _prx;
    private ThreadPoolExecutor _pool;

    private class AllocTask implements Runnable {

	public AllocTask(Item item) {
	    _item = item;
	}

	@Override
	public void run() {
	    _prx.alloc(_item.id, _item.block);
	}

	private Item _item;
    }

    @Override
    public void alloc(Item item) {
	this._pool.execute(new AllocTask(item));
    }

    @Override
    public List<Long> get(int blockId) {
	return _prx.get(blockId);
    }

    @Override
    public void close() {
	if (_ic != null) {
	    _ic.destroy();
	}
    }

    @Override
    public void initialize(String name, String config) {
	super.initialize(name, config);
	/*
	 * The config string format is ICE endpoins format. Example: M:tcp -h
	 * 1.2.3.4 -p 5677
	 */
	this._ic = Ice.Util.initialize();
	this._prx = MetaServerPrxHelper
		.uncheckedCast(_ic.stringToProxy(config));
	this._pool = new ThreadPoolExecutor(1, 5, 10, TimeUnit.SECONDS,
		new ArrayBlockingQueue<Runnable>(1000),
		new ThreadPoolExecutor.AbortPolicy());
    }

}
