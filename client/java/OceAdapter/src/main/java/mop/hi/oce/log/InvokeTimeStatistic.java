package mop.hi.oce.log;

import java.io.PrintStream;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.log.Oce;

public class InvokeTimeStatistic {
	private static final boolean ENABLED = false;

	public enum Type {
		USER_GET, USER_SET, USERBASIC_GET, USERBASIC_SET, USERDESC_GET, USERDESC_SET, USERVALUE_GET, USERVALUE_SET, RELATION_GET, RELATION_SET, NETWORK_GET, NETWORK_SET
	};

	public class Statistic {
		private int _count = 1;
		private long _time;
		private long _max;

		public void addOne(long time) {
			if (ENABLED) {
				++_count;
				_time += time;
				_max = Math.max(time, _max);
			}
		}

		public void writeStat(Type type, PrintStream ps) {
			if (ENABLED) {
				StringBuffer r = new StringBuffer();
				r.append(type);
				r.append(" Count:").append(_count);
				r.append(" Time:").append(_time);
				r.append(" Max:").append(_max);
				r.append(" Avg:").append(_time / _count).append("\n");
				ps.append(r);
			}
		}

		public void reset() {
			if (ENABLED) {
				_count = 1;
				_time = 0;
				_max = 0;
			}
		}
	}

	private class Printer extends Thread {
		@Override
		public void run() {
			if (ENABLED) {
				while (true) {
					try {
						System.out
								.println("****************************************");
						_stats.get(Type.USER_GET).writeStat(Type.USER_GET,
								System.out);
						_stats.get(Type.USER_GET).reset();
						_stats.get(Type.USERBASIC_GET).writeStat(
								Type.USERBASIC_GET, System.out);
						_stats.get(Type.USERBASIC_GET).reset();
						_stats.get(Type.USERDESC_GET).writeStat(
								Type.USERDESC_GET, System.out);
						_stats.get(Type.USERDESC_GET).reset();
						_stats.get(Type.USERVALUE_GET).writeStat(
								Type.USERVALUE_GET, System.out);
						_stats.get(Type.USERVALUE_GET).reset();
						_stats.get(Type.RELATION_GET).writeStat(
								Type.RELATION_GET, System.out);
						_stats.get(Type.RELATION_GET).reset();
						_stats.get(Type.NETWORK_GET).writeStat(
								Type.NETWORK_GET, System.out);
						_stats.get(Type.NETWORK_GET).reset();
						_stats.get(Type.USER_SET).writeStat(Type.USER_SET,
								System.out);
						_stats.get(Type.USER_SET).reset();
						_stats.get(Type.USERBASIC_SET).writeStat(
								Type.USERBASIC_SET, System.out);
						_stats.get(Type.USERBASIC_SET).reset();
						_stats.get(Type.USERDESC_SET).writeStat(
								Type.USERDESC_SET, System.out);
						_stats.get(Type.USERDESC_SET).reset();
						_stats.get(Type.USERVALUE_SET).writeStat(
								Type.USERVALUE_SET, System.out);
						_stats.get(Type.USERVALUE_SET).reset();
						_stats.get(Type.RELATION_SET).writeStat(
								Type.RELATION_SET, System.out);
						_stats.get(Type.RELATION_SET).reset();
						_stats.get(Type.NETWORK_SET).writeStat(
								Type.NETWORK_SET, System.out);
						_stats.get(Type.NETWORK_SET).reset();
						Thread.sleep(30 * 1000);
					} catch (Exception e) {
						Oce.getLogger().error(this.getClass().getName() + " " + e);
					}
				}
			}
		}
	}

	private static InvokeTimeStatistic _instance = new InvokeTimeStatistic();

	public static InvokeTimeStatistic getInstance() {
		return _instance;
	}

	private Map<Type, Statistic> _stats = new HashMap<Type, Statistic>();

	private Thread _thread;

	public InvokeTimeStatistic() {
		if (ENABLED) {
			_stats.put(Type.USER_GET, new Statistic());
			_stats.put(Type.USERBASIC_GET, new Statistic());
			_stats.put(Type.USERDESC_GET, new Statistic());
			_stats.put(Type.USERVALUE_GET, new Statistic());
			_stats.put(Type.RELATION_GET, new Statistic());
			_stats.put(Type.NETWORK_GET, new Statistic());
			_stats.put(Type.USER_SET, new Statistic());
			_stats.put(Type.USERBASIC_SET, new Statistic());
			_stats.put(Type.USERDESC_SET, new Statistic());
			_stats.put(Type.USERVALUE_SET, new Statistic());
			_stats.put(Type.RELATION_SET, new Statistic());
			_stats.put(Type.NETWORK_SET, new Statistic());
			_thread = new Printer();
			_thread.start();
		}
	}

	public void statSomething(Type type, long time) {
		if (ENABLED) {
			_stats.get(type).addOne(time);
		}
	}

}
