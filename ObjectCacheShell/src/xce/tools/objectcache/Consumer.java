package xce.tools.objectcache;

public abstract class Consumer extends Thread {
	protected final Producer producer;

	public Consumer(Producer producer) {
		this.producer = producer;
	}

	public void waitFinished() throws InterruptedException {
		if (producer.getState() != Thread.State.RUNNABLE) {
			producer.start();
		}
		if (this.getState() != Thread.State.RUNNABLE) {
			this.start();
		}
		synchronized (this) {
			this.wait();
		}
	}

	@Override
	public void run() {
		try {
			while (!producer.isFinished()) {
				Long[] batch = producer.take();
				try {
					consume(batch);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			System.out.println("Producer finished, Stage2 started.");
			while (true) {
				Long[] batch = producer.queue.poll();
				if (batch == null) {
					System.out.println("Stage2 finished");
					break;
				}
				try {
					consume(batch);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		} finally {
			synchronized (this) {
				this.notify();
			}
		}
	}

	protected abstract void consume(Long[] batch);
}
