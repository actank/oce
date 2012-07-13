package xce.eva.collect;

import java.util.ArrayList;
import java.util.List;

public class EvaCollector {

	private static EvaCollector instance = new EvaCollector();
	private static final int BUCKET_SIZE = 10;
	private List<LogHolder> holders = new ArrayList<LogHolder>();
	private List<String> loggerCategoriesThread = new ArrayList<String>();

	private EvaCollector() {
		initialize();
	}

	public static EvaCollector getInstance() {
		return instance;
	}

	protected void initialize() {
		// 打一行提示性能评价系统布置成功的log
		System.out
				.println("[EvaCollector::initialize] start to use ice_evolution system\n");
		// 设置黑名单词
		// blackwords_.push_back("SubscriberRandom_");

		int bucketSize = BUCKET_SIZE;
		for (int i = 0; i < bucketSize; ++i) {
			LogHolder holder = new LogHolder();
			holder.initialize(i);
			holders.add(holder);
		}

		loggerCategoriesThread.add("IceLog"); // 设置threadPool的log的路径
		loggerCategoriesThread.add("threadPoolNum");

	}

	public String getServerName(String str) {
		// if (str.contains("_") && str.contains("Manager")
		// && str.contains("tcp -h ") && str.contains(" -p ")
		// && str.contains(" -t ")) {
		int beginPos = str.indexOf("_");
		int endPos = str.indexOf("DelM");
		String serverName = "";
		if (beginPos + 1 <= endPos) {
			serverName = str.substring(beginPos + 1, endPos);
		} else {
			return trimServerName(str);
		}

		if (str.contains("tcp -h ") && str.contains(" -p ")
				&& str.contains(" -t ")) {

			beginPos = str.indexOf("tcp -h ");
			endPos = str.indexOf(" -p ");

			if (beginPos + 7 <= endPos) {
				serverName = serverName + "["
						+ str.substring(beginPos + 7, endPos);
			} else {
				return trimServerName(serverName);
			}

			beginPos = str.indexOf(" -p ");
			endPos = str.indexOf(" -t ");

			if (beginPos + 4 <= endPos) {
				serverName = serverName + ":"
						+ str.substring(beginPos + 4, endPos) + "]";
			}
			// System.out.println(serverName);
			return trimServerName(serverName);
		} else if (str.contains("@")) {
			String[] tmpArr = str.split("@");
			serverName = serverName + "@" + tmpArr[tmpArr.length - 1];
			return trimServerName(serverName);

		} else {
			return trimServerName(serverName);
		}
		// } else {
		// return str;
		// }

	}

	public String trimServerName(String str) {
		String result = "";
		result = str.replace(" ", "");
		return result;
	}

	public void collect(String serviceName, String operaName, long time,
			int responseTime, boolean isException) {

		if ((serviceName.contains("SubscriberRandom") || (operaName
				.contains("-")))
				|| (serviceName.contains("Controller"))) {
			return;
		}

		if (serviceName.contains("@")) {
			try {
				serviceName = getServerName(serviceName);
			} catch (Exception e) {
				System.out.println("error from getServerName:" + serviceName);
				e.printStackTrace();
			}
		}

		// System.out.println("serviceName:" + serviceName);
		// System.out.println("operaName:" + operaName);

		LogTail tail = new LogTail(serviceName + "-" + operaName, time,
				responseTime, isException);
		LogHolder holder = findHolder();
		if (holder != null) {
			holder.add(tail);
		}
	}

	public void printThreadPoolLog(String prefix, int threadPoolNum) {

		// if (prefix.contains("-")) {
		// return;
		// }
		// String os;
		// os = System.currentTimeMillis() / 1000L + " " + prefix + " "
		// + threadPoolNum;

		// logger.addLog(loggerCategoriesThread.toArray(new
		// String[loggerCategoriesThread.size()]), os);

	}

	protected LogHolder findHolder() {

		int seed = (int) (Math.random() * 10);

		int size = holders.size();
		if (size > 0) {
			// System.out.println("seed : "+seed+" ---- size : "+holders.size());
			return holders.get(seed % size);
			// return holders.get(0);
		} else {
			return null;
		}
	}

//	public static void main(String[] args) {
//		EvaCollector test = new EvaCollector();
//		String strc = "xce.friendfinder._FriendFinderByActiveUserDelM@7375aaab+4594D63F-6627-480F-BFF7-361B39D95704 -t:tcp -h 10.3.17.103 -p 36532 -t 300";
//		// String tmp=strc.replace(" ", "");
//		// System.out.println(tmp);
//		System.out.println(strc);
//		String str = test.getServerName(strc);
//		System.out.println(str);
//		System.exit(1);
//	}

}
