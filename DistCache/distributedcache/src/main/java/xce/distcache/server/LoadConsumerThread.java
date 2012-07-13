package xce.distcache.server;

import java.util.Map;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-18
 */
public class LoadConsumerThread extends Thread {
	public void run() {
		while (true) {
			LoadClusterHelper.getInstance().add(LoadBlockingDeque.getInstance().pop());
		}
	}
}