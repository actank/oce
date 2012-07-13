package xce.eva.collect;

public class LogCount {
	private final long time;
	private final int responseTime;
	private final boolean isException;
	
	protected long getTime() {
		return time;
	}

	protected int getResponseTime() {
		return responseTime;
	}

	protected boolean isException() {
		return isException;
	}

	protected LogCount(LogTail p) {
		time = p.getTime();
		responseTime = p.getResponseTime();
		isException = p.isException();
	}

}
