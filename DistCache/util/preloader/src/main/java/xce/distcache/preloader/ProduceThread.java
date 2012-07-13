package xce.distcache.preloader;

import java.util.ArrayList;
import java.util.List;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-06-17
 */

public class ProduceThread extends Thread {

    public void run() {

	PreloaderLogger.info("PreduceThread.start");
	ProductStack.getInstance().stop();

	while (true) {
	    try {
		Long beginId = IdsProducer.getInstance().getMaxId();
		ProductStack.getInstance().push(
			IdsProducer.getInstance().produce(beginId));
	    } catch (Exception e) {
		PreloaderLogger.warn(e);
	    }
	}
    }

}
