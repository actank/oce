package xce.eva.collect;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Stat {
	private List<LogTail> tails = new ArrayList<LogTail>();
	private Map<String, List<LogCount>> logMap = new HashMap<String, List<LogCount>>();
	private List<StatObject> statObjects = new ArrayList<StatObject>();
	
	protected Stat(List<LogTail> tail_) {
		tails = tail_;
		// I. Average/Max/Min/Timeout
		StatObject statAvgByTime = new StatAvgByTime();
		statObjects.add(statAvgByTime);
		// II. ThreadPool
		// StatObjectPtr statThreadPool = new StatThreadPool();
		// statObjects_.push_back(statThreadPool);
		// III. TopPart(暂不开放)
		StatObject statTopPart = new StatTopPart();
		statObjects.add(statTopPart);
	}

	protected void merge() {
		for (int i = 0; i < tails.size(); ++i) {
			LogTail logTail = tails.get(i);
			String keyValue = logTail.getName();
			List<LogCount> it = logMap.get(keyValue);
			if (it != null) {
				LogCount logCount = new LogCount(logTail);
				it.add(logCount);
			} else {
				List<LogCount> logList = new ArrayList<LogCount>();
				LogCount logCount = new LogCount(logTail);
				logList.add(logCount);

				logMap.put(keyValue, logList);
			}
		}

	}

	protected void output(long timeDur,List<String> loggerCategories) {

		for (Map.Entry<String, List<LogCount>> entry : logMap.entrySet()) {

			List<LogCount> logList = entry.getValue();

			String keyValue = entry.getKey();
			String serverName = keyValue;
			String operaName = "";

			if (keyValue.contains("-")) {
				int pos = keyValue.indexOf("-");
				serverName = keyValue.substring(0, pos);
				operaName = keyValue.substring(pos + 1);
			}

			// 依次执行各次统计
			for (int i = 0; i < statObjects.size(); ++i) {
				StatObject statObject = statObjects.get(i);
				statObject.output(timeDur, serverName, operaName, logList,loggerCategories);
			}
		}
	}

}