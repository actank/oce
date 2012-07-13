package xce.util.task;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class TaskManager {
	public static int TASK_LEVEL_NORMAL = 0;

	private static TaskManager _instance = new TaskManager();

	public static TaskManager getInstance() {
		return _instance;
	}

	private TaskManager() {
	}

	private Map<Integer, TaskPool> _pools = new ConcurrentHashMap<Integer, TaskPool>();

	public void push(Task task) {
		configAndGet(task.getLevel(), new TaskConfig()).push(task);
	}
	
	private TaskPool configAndGet(int level, TaskConfig config){
		TaskPool pool = _pools.get(level);
		if (pool == null) {
			synchronized (_pools) {
				if (pool == null) {
					pool = new TaskPool(config);
					_pools.put(level, pool);
				}
			}
		}
		return pool;
	}
	
	public void config(int level, TaskConfig config){
		configAndGet(level, config);
	}

	public static void main(String[] args) throws InterruptedException {
		for (int i = 1; i < 100; ++i) {
			Task task = new Task() {
				public void run() {
					System.out.println(System.currentTimeMillis());
				}
			};
			TaskManager.getInstance().push(task);
		}
		System.out.println("push done.");
		Thread.sleep(1000);
		System.exit(0);
	}
}
