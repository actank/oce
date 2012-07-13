package xce.eva.collect;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class StatTopPart extends StatObject {
	// 0....20,30,40,50,60,70,80,90,100,150,200,250,300+,logSize
	private static Map<String, int[]> recordMap = new HashMap<String, int[]>();
	private static int timeLast = (int) (System.currentTimeMillis() / 1000L);
	private static Object _mapMutex = new Object();
	private static Object _timeMutex = new Object();

	protected Map<String, List<Integer>> stat(String statMsg,
			Map<String, List<Integer>> statMsgMap, List<LogCount> logList) {
		String os = "";
		// 统计log，将logVector中的数据收集到_recordSeq中
		synchronized (_mapMutex) {
			int[] recordList = recordMap.get(statMsg);
			if (!recordMap.containsKey(statMsg)) {
				int[] recordSeq = new int[34];
				for (int j = 0; j < 34; ++j) {
					recordSeq[j] = 0;
				}

				recordMap.put(statMsg, recordSeq);
				recordList = recordMap.get(statMsg);
			}

			for (int i = 0; i < logList.size(); ++i) {
				LogCount logCount = logList.get(i);
				int pos = logCount.getResponseTime() / 1000;
				int index = getIndex(pos);
				++recordList[index];
			}

			recordList[33] += logList.size();
		}

		// 如果到达时间间隔则输出，返回statMsgMap，否则返回statMsg为" "

		statMsg = os;
		long nowTime = System.currentTimeMillis() / 1000L;
		synchronized (_timeMutex) {
			if (nowTime - timeLast < 300) {
				return null;
			}
			timeLast = (int) nowTime;
		}

		Map<String, int[]> recordMapTemp = recordMap;

		synchronized (_mapMutex) {
			recordMap = new HashMap<String, int[]>();
		}

		for (Map.Entry<String, int[]> entry : recordMapTemp.entrySet()) {
			String key = entry.getKey();
			int[] recordSeq = entry.getValue();

			// 统计服务名+方法名的key的log的99%，99.9%，99.99%的记录
			int logSize = recordSeq[33];
			int size99 = logSize / 100;
			boolean flag99 = true;
			int result99 = 0;

			int size99_9 = logSize / 1000;
			boolean flag99_9 = true;
			int result99_9 = 0;

			int size99_99 = logSize / 10000;
			boolean flag99_99 = true;
			int result99_99 = 0;

			int count = 0;
			int index = 32;

			for (; index >= 0; --index) {
				count += recordSeq[index];
				if (count > size99_99 && flag99_99) {
					result99_99 = getRecord(index);
					flag99_99 = false;
				}

				if (count > size99_9 && flag99_9) {
					result99_9 = getRecord(index);
					flag99_9 = false;
				}

				if (count > size99 && flag99) {
					result99 = getIndex(index);
					flag99 = false;
					break;
				}
			}

			List<Integer> resultSeq = new ArrayList<Integer>();
			resultSeq.add(result99);
			resultSeq.add(result99_9);
			resultSeq.add(result99_99);

			statMsgMap.put(key, resultSeq);

			timeLast = (int) (System.currentTimeMillis() / 1000L);

		}

		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(1); // 0为avg，1为record99
		statMsgMap.put("result", typeList);

		return statMsgMap;

	}

	private int getIndex(int record) {

		int result = 0;
		if ((record <= 20) && (record >= 0)) {
			result = record;
		} else if ((record <= 100) && (record >= 21)) {
			result = record / 10 + 18;
		} else if ((record <= 300) && (record >= 101)) {
			result = (record - 100) / 50 + 28;
		} else {
			result = 32;
		}

		return result;

	}

	private int getRecord(int index) {
		int result = 0;
		if (index == 32) {
			result = 300;
		} else if ((index <= 31) && (index >= 28)) {
			result = (index - 28) * 50 + 100;
		} else if ((index <= 27) && (index >= 21)) {
			result = (index - 18) * 10;
		} else if ((index <= 20) && (index >= 0)) {
			result = index;
		}

		return result;
	}

}