package xce.eva.collect;

public class LogTail {
 	private final String name;
	private final long time;
	private final int responseTime;
	private final boolean isException;
	
	protected LogTail(String n, long t, int r, boolean is){
        name = n;
        time = t;
        responseTime = r;
        isException = is;
   }

	protected final String getName() {
		return name;
	}

	protected long getTime() {
		return time;
	}

	protected int getResponseTime() {
		return responseTime;
	}

	protected boolean isException() {
		return isException;
	}
}
