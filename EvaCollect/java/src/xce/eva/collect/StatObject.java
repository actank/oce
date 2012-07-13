package xce.eva.collect;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.xiaonei.xce.log.XceLogger;
import com.xiaonei.xce.log.XceLoggerAdapter;

public class StatObject {
	private XceLogger logger_ = XceLoggerAdapter.getLogger("M@EvaLogger0");

	protected void output(long timeDur, String serviceName, String operaName,
			List<LogCount> logVector, List<String> loggerCategories) {

		String statMsg = serviceName + "-" + operaName;
		Map<String, List<Integer>> recordMap = new HashMap<String, List<Integer>>();

		recordMap = stat(statMsg, recordMap, logVector);

		if ((recordMap == null) || (!recordMap.containsKey("result"))) {
			return;
		}
		List<Integer> statMsgList = recordMap.get("result");
		int type = statMsgList.get(0);

		int timeCur = (int) (System.currentTimeMillis() / 1000L);

		if (type == 1) { // 统计99％的stat，并且到达指定之间则打印log

			for (Map.Entry<String, List<Integer>> entry : recordMap.entrySet()) {

				String os;
				String key = entry.getKey();
				String serviceNameTmp = null;
				String operaNameTmp = null;

				if (!key.contains("-")) {
					continue;
				}

				int pos = key.indexOf("-");
				serviceNameTmp = key.substring(0, pos);
				operaNameTmp = key.substring(pos + 1);

				// os = timeCur + " " + serviceNameTmp + "Client" + " "
				// + operaNameTmp;

				os = timeCur + " " + serviceNameTmp + " " + operaNameTmp;

				List<Integer> recordSeq = entry.getValue();

				for (int i = 0; i < recordSeq.size(); ++i) {
					os = os + " " + recordSeq.get(i);
				}

				List<String> categories = new ArrayList<String>(
						loggerCategories);
				categories.remove(loggerCategories.size() - 1);
				categories.add("record_99");
				try {
//					System.out.println(categories.toArray(new String[categories
//							.size()]));
//					System.out.println(os);
					logger_.addLog(categories.toArray(new String[categories
							.size()]), os); // list转换String数组
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		} else if (type == 0) {
			int size = statMsgList.get(1);
			int averageTime = statMsgList.get(2);
			int maxTime = statMsgList.get(3);
			int minTime = statMsgList.get(4);
			int timeoutNum = statMsgList.get(5);

			String os;
			// os = timeCur + " " + timeDur + " " + serviceName + "Client" + " "
			// + operaName + " " + size + " " + averageTime + " "
			// + maxTime + " " + minTime + " " + timeoutNum;

			os = timeCur + " " + timeDur + " " + serviceName + " " + operaName
					+ " " + size + " " + averageTime + " " + maxTime + " "
					+ minTime + " " + timeoutNum;

			try {
				String[] categories = loggerCategories
						.toArray(new String[loggerCategories.size()]);

//				System.out.println(categories);
//				System.out.println(os);
				logger_.addLog(categories, os);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	protected Map<String, List<Integer>> stat(String statMsg,
			Map<String, List<Integer>> statMsgMap, List<LogCount> logVector) {
		return statMsgMap;
	}

}
