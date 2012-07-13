package xce.distcache.server;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.apache.zookeeper.KeeperException;

import Ice.Current;

public class MetaServerI extends xce.distcache.server._MetaServerDisp {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	    .getLog(MetaServerI.class);

    private static final long serialVersionUID = 6269145580045109301L;
    private MetaDb _db;
    private ThreadPoolExecutor _pool;

    private class AllocTask implements Runnable {
	public AllocTask(long id, int block) {
	    _id = id;
	    _block = block;
	}

	@Override
	public void run() {
	    _db.add(_block, _id);
	}

	private long _id;
	private int _block;
    }

    protected MetaServerI() {

    }

    protected synchronized void initialize(String data_dir) throws IOException,
	    KeeperException, InterruptedException {

	_db = new MetaDb();
	_db.initialize(data_dir);
	this._pool = new ThreadPoolExecutor(1, 5, 10, TimeUnit.SECONDS,
		new ArrayBlockingQueue<Runnable>(10000),
		new ThreadPoolExecutor.AbortPolicy());

	System.out.println("MetaServerI initialized");
    }

    protected void close() {
	_db.close();
    }

    public boolean alloc(long id, int block, Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("MetaServerI::alloc id=" + id + " block=" + block);
	}
	try {
	    this._pool.execute(new AllocTask(id, block));
	} catch (Exception e) {
	    return false;
	}
	return true;
    }

    public List<Long> get(int block, Current current) {
	if (_logger.isDebugEnabled()) {
	    _logger.debug("MetaServerI::get block=" + block);
	}
	Set<Long> dt = _db.get(block);
	if (dt == null) {
	    return null;
	}
	List<Long> rt = new ArrayList<Long>();
	for (Long d : dt) {
	    rt.add(d);
	}
	return rt;
    }

}
