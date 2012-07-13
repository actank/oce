package xce.tools.objectcache;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

public abstract class Producer extends Thread {
	protected final BlockingQueue<Long[]> queue;
	private AtomicBoolean isFinished = new AtomicBoolean(false);

	protected Producer(BlockingQueue<Long[]> queue) {
		this.queue = queue;
	}

	public Long[] take() throws InterruptedException {
		return queue.take();
	}

	public boolean isFinished() {
		return isFinished.get();
	}

	@Override
	public void run() {
		List<Long> batch = new ArrayList<Long>();
		do {
			if (!produce(batch)) {
				isFinished.set(true);
			}
			queue.offer(batch.toArray(new Long[batch.size()]));
			batch = new ArrayList<Long>();
		} while (!isFinished.get());
	}

	protected abstract boolean produce(List<Long> emptyData);
}
