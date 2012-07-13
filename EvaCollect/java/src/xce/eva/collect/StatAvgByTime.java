package xce.eva.collect;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class StatAvgByTime extends StatObject {

	private static final int timeout = 300000;
	
	protected Map<String, List<Integer>> stat(String statMsg,
			Map<String, List<Integer>> statMsgMap, List<LogCount> logList) {

		long sumTime = 0;
		int averageTime = 0;
		int maxTime = 0;
		int minTime = 0x7FFFFFFF;
		int timeoutNum = 0;

		for (int i = 0; i < logList.size(); ++i) {
			LogCount logCount = logList.get(i);
			sumTime += logCount.getResponseTime();
			maxTime = logCount.getResponseTime() > maxTime ? logCount
					.getResponseTime() : maxTime;
			minTime = logCount.getResponseTime() < minTime ? logCount
					.getResponseTime() : minTime;
			if (logCount.getResponseTime() > timeout)
				++timeoutNum;
		}

		int size = logList.size();
		if (size > 0) {
			averageTime = (int) (sumTime / size);
		} else {
			averageTime = 0;
		}
//		os = size + " " + averageTime + " " + maxTime + " " + minTime + " "
//				+ timeoutNum;
//		statMsg = os;

		// statMsgMap
		List<Integer> typeList = new ArrayList<Integer>();
		typeList.add(0);	//0为avg，1为record99
		
		typeList.add(size);
		typeList.add(averageTime);
		typeList.add(maxTime);
		typeList.add(minTime);
		typeList.add(timeoutNum);
		
		statMsgMap.put("result", typeList);
		
		return statMsgMap;
	}
}