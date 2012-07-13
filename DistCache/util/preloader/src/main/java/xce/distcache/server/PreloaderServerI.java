package xce.distcache.server;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.zookeeper.KeeperException;
import xce.distcache.preloader.ConsumeThread;
import xce.distcache.preloader.IdsProducer;
import xce.distcache.preloader.PreferCreate;
import xce.distcache.preloader.PreloaderLogger;
import xce.distcache.preloader.ProduceThread;
import xce.distcache.preloader.ProductStack;
import xce.distcache.preloader.SetData;
import xce.distcache.preloader.SetThread;
import Ice.Current;

public class PreloaderServerI extends xce.distcache.server._PreloaderServerDisp {

    private static final long serialVersionUID = 6611249479988954883L;
    private static final int CONSUMESIZE = 8;
    private static final int SETSIZE = 10;
    private final ProduceThread _produceThread = new ProduceThread();
    private List<ConsumeThread> _consumeThreads = new ArrayList<ConsumeThread>();
    private List<SetThread> _setThreads = new ArrayList<SetThread>();

    protected void initialize() throws IOException, KeeperException,
	    InterruptedException {
	_produceThread.start();

	for (int i = 0; i < CONSUMESIZE; i++) {
	    _consumeThreads.add(new ConsumeThread());
	}

	for (ConsumeThread el : _consumeThreads) {
	    el.start();
	}

	for (int i = 0; i < SETSIZE; i++) {
	    _setThreads.add(new SetThread());
	}
	for (SetThread el : _setThreads) {
	    el.start();
	}
    }

    @Override
    public void preloaderAll(Current current) {
	// TODO Auto-generated method stub

    }

    @Override
    public void preloaderByName(String name, Current current) {
	PreloaderLogger
		.debug("PreloaderServerI.preloaderByName, name: " + name);
	System.out.println("PreloaderServerI.preloaderByName, name: " + name);
	IdsProducer.getInstance().resetMaxId();
	PreferCreate.getInstance().setName(name);
	SetData.getInstance().setName(name);
	ProductStack.getInstance().start();

	return;
    }

    public void close() throws InterruptedException {
    }
}
