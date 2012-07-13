package xce.util.task;

public abstract class Task implements Runnable {
	public Task() {
		_level = TaskManager.TASK_LEVEL_NORMAL;
	}

	public Task(int level) {
		_level = level;
	}

	private final int _level;

	public int getLevel() {
		return _level;
	}

}
