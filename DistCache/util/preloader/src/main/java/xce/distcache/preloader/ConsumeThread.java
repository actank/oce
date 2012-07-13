package xce.distcache.preloader;

import java.util.Map;
import java.util.TreeMap;

public class ConsumeThread extends Thread {

    public ConsumeThread() {
    }

    public void run() {

	Map<Long, byte[]> data = new TreeMap<Long, byte[]>();
	PreloaderLogger.debug("ConsumeThread.start");
	while (true) {
	    try {
		long b1 = System.currentTimeMillis();
		ProducerData pd = ProductStack.getInstance().pop();
		long b2 = System.currentTimeMillis();
		data = PreferCreate.getInstance().create(pd);
		long b3 = System.currentTimeMillis();
		long minid = Long.MAX_VALUE;
		long maxid = Long.MIN_VALUE;
		PreloaderLogger.debug(this + " minid: " + pd.getMin()
			+ " maxid: " + pd.getMax() + " size: "
			+ pd.getIds().size());
		SetData.getInstance().set(data);
		long b4 = System.currentTimeMillis();
		PreloaderLogger.debug("set timeCost: " + "t1: " + (b2 - b1)
			+ " t2: " + (b3 - b2) + " t3: " + (b4 - b3));
	    } catch (Exception e) {
		e.printStackTrace();
		PreloaderLogger
			.warn("cosumeThread, Exception: " + e.toString());
	    }
	}
    }

}
