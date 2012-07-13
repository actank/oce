package xce.eva.collect;

import java.util.ArrayList;
import java.util.List;
import java.util.TimerTask;

public class OutputTimer extends TimerTask {
	protected OutputTimer(LogHolder holder, int suffix) {
		holder_ = holder;
		suffix_ = suffix;
	}

//	private static final int TIME_INTERVAL = 15;
//	private static final int BATCH_SIZE = 10000;
	private long timeLast = System.currentTimeMillis() / 1000L;

	public void run() {
		List<String> loggerCategories = new ArrayList<String>();
		String bucket;
		bucket = "bucket_" + suffix_;
		loggerCategories.add("IceLog");
		loggerCategories.add(bucket);

//		int sleep_time = TIME_INTERVAL;
//		long timeLast = System.currentTimeMillis() / 1000L;
//		while (true) {
			long timeDur = System.currentTimeMillis() / 1000L - timeLast;
			synchronized (holder_.buffer) {
//				if ((holder_.size() < (int) (BATCH_SIZE))
//						&& (timeDur < sleep_time)) {
				if(holder_.size()==0){
					try {
						holder_.buffer.wait();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					return;
				}
			}

			List<LogTail> t = new ArrayList<LogTail>();
			t = holder_.getAndClear();
			Stat stat = new Stat(t);
			stat.merge();
			stat.output(timeDur,loggerCategories);
			timeLast = System.currentTimeMillis() / 1000L;

//		}

	}

	private LogHolder holder_;
	private int suffix_;
}
