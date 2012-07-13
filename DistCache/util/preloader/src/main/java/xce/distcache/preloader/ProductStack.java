package xce.distcache.preloader;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;

import xce.distcache.DistCacheAdapter;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-06-17
 */
public class ProductStack {
    private static final int MAX_SIZE = 20;

    private static final int POPSIZE = 1000;

    private BlockingDeque<ProducerData> _queue = new LinkedBlockingDeque<ProducerData>(
	    MAX_SIZE);

    private static final ProductStack _instance = new ProductStack();

    public static ProductStack getInstance() {
	return _instance;
    }

    public synchronized void start() {
	PreloaderLogger.debug("ProductStack.start");
	notify();
    }

    public synchronized void stop() {
	PreloaderLogger.debug("ProductStack.stop");
	try {
	    wait();
	} catch (InterruptedException e) {
	    PreloaderLogger.warn(e);
	}
    }

    public void push(ProducerData data) {
	if (data.getIds().size() <= 0) {
	    DistCacheAdapter adapter = DistCacheAdapter.connect("UserCache");
	    adapter.setNodeStatusUpgrading2On();
	    stop();
	    return;
	}
	try {
	    _queue.putLast(data);
	} catch (InterruptedException e) {
	    e.printStackTrace();
	}
    }

    public ProducerData pop() {
	try {
	    return _queue.takeFirst();
	} catch (InterruptedException e) {
	    e.printStackTrace();
	}
	return new ProducerData();
    }
}
