package com.xiaonei.importer;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;


public class TaskManager {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(TaskManager.class);

	private static TaskManager _instance = new TaskManager();

	public static TaskManager getInstance() {
		return _instance;
	}

	public TaskManager() {
		_threads = new WorkerThread[10];
		_tasks = new ConcurrentLinkedQueue<Runnable>();
		for (int i = 0; i < _threads.length; i++) {
			_threads[i] = new WorkerThread();
			_threads[i].start();
		}
	}

	private Thread[] _threads;
	private Queue<Runnable> _tasks;

	public void addTask(Runnable o) {
		_tasks.offer(o);
	}

	private class WorkerThread extends Thread {
		public void run() {
			while (true) {
				try {
					Runnable task = null;
					synchronized (_tasks) {
						if(!_tasks.isEmpty())
							task = _tasks.poll();
					}
					if (task == null) {
						if (_logger.isTraceEnabled()) {
							_logger
									.trace("TaskManager has no task to run. sleep 1 second.");
						}
						Thread.sleep(1000);
						continue;
					}
					Thread thread = new Thread(task);
					thread.start();
					thread.join();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}

}
