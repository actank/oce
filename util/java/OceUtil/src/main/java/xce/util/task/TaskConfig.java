package xce.util.task;

public class TaskConfig {
	public TaskConfig(int min, int max) {
		_min = min;
		_max = max;
	}

	public TaskConfig() {
		_min = 10;
		_max = 100;
	}

	private final int _min;
	private final int _max;

	public int getMin() {
		return _min;
	}

	public int getMax() {
		return _max;
	}
}
