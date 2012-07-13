package xce.eva.collect;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;

public class LogHolder {
	public List<LogTail> buffer = new ArrayList<LogTail>();
	// public Object mutex = new Object();
	private OutputTimer output;
	private Timer timer = new Timer();

	protected void initialize(int suffix) {
		output = new OutputTimer(this, suffix);
		// output.start();
		timer.schedule(output, 15 * 1000, 15 * 1000);
		// buffer.reserve(300000);

	}

	protected void add(LogTail tail) {
		synchronized (buffer) {
			buffer.add(tail);
			buffer.notify();
		}

	}

	protected int size() {
		return buffer.size();
	}

	protected List<LogTail> getAndClear() {
		List<LogTail> logTails = new ArrayList<LogTail>();
		synchronized (buffer) {
			logTails = buffer;
			buffer = new ArrayList<LogTail>();
		}
		return logTails;
	}

}
