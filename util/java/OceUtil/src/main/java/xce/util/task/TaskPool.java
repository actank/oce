package xce.util.task;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class TaskPool {

	public TaskPool(TaskConfig config) {
		_pool = new ThreadPoolExecutor(config.getMin(), config.getMax(), 10,
				TimeUnit.SECONDS, _tasks);
	}

	public void config(TaskConfig config) {
		_pool.setCorePoolSize(config.getMin());
		_pool.setMaximumPoolSize(config.getMax());
	}

	private final ThreadPoolExecutor _pool;
	private BlockingQueue<Runnable> _tasks = new LinkedBlockingQueue<Runnable>();

	public void push(Task task) {
		_pool.execute(task);
	}

	@Override
	protected void finalize() throws Throwable {
		_pool.shutdown();
	}
}
