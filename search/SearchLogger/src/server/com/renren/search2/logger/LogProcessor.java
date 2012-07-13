package com.renren.search2.logger;

import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;

import mop.hi.oce.domain.network.HighSchoolInfo;
import mop.hi.oce.domain.network.UniversityInfo;
import mop.hi.oce.domain.network.WorkplaceInfo;

import com.xiaonei.platform.component.space.home.SpaceHome;

public class LogProcessor {
	public static Logger logger = Logger.getLogger("statisticLogger");

	public static String generateLog(Map<String, String> log) {
		StringBuffer sb = new StringBuffer();
		long time = 0;
		int hostId = 0;
		int userId = 0;
		String query = "";
		String json = "";
		int relation = 0;
		int commonFriends = 0;
		int totalCount = 0;
		int index = 0;
		String position = "";
		// 过滤数据
		if (log.get("time") != null && LogProcessor.isNumeric(log.get("time"))) {
			time = Long.parseLong(log.get("time"));
		}
		if (log.get("hostId") != null
				&& LogProcessor.isNumeric(log.get("hostId"))) {
			hostId = Integer.parseInt(log.get("hostId"));
		}
		if (log.get("userId") != null
				&& LogProcessor.isNumeric(log.get("userId"))) {
			userId = Integer.parseInt(log.get("userId"));
		}
		if (log.get("query") != null && !log.get("query").isEmpty()) {
			query = log.get("query");
		}
		if (log.get("json") != null && !log.get("json").isEmpty()) {
			json = log.get("json");
		}
		if (log.get("relation") != null
				&& LogProcessor.isNumeric(log.get("relation"))) {
			relation = Integer.parseInt(log.get("relation"));
		}
		if (log.get("commonFriends") != null
				&& LogProcessor.isNumeric(log.get("commonFriends"))) {
			commonFriends = Integer.parseInt(log.get("commonFriends"));
		}
		if (log.get("totalCount") != null
				&& LogProcessor.isNumeric(log.get("totalCount"))) {
			totalCount = Integer.parseInt(log.get("totalCount"));
		}
		if (log.get("index") != null
				&& LogProcessor.isNumeric(log.get("index"))) {
			index = Integer.parseInt(log.get("index"));
		}
		// if (log.get("position") != null
		// && LogProcessor.isNumeric(log.get("position"))) {
		// position = log.get("position");
		// }
		if (log.get("position") != null) {
			position = log.get("position");
		}
		SpaceHome spaceHome = SpaceHome.getInstance();
		// 执行搜索用户资料
		List<WorkplaceInfo> hostWorkPlaceList = spaceHome
				.getWorkplaceInfoList(hostId);
		List<UniversityInfo> hostUniversityList = spaceHome
				.getUniversityInfoList(hostId);
		List<HighSchoolInfo> hostHighSchoolList = spaceHome
				.getHighschoolInfoList(hostId);
		// 组装执行搜索用户资料
		StringBuffer sbHost = new StringBuffer();
		for (WorkplaceInfo workPlaceInfo : hostWorkPlaceList) {
			sbHost.append(workPlaceInfo.getWorkplaceName() + "|");
		}
		sbHost.append(";");
		for (UniversityInfo universityInfo : hostUniversityList) {
			sbHost.append(universityInfo.getUniversityName() + "|");
		}
		sbHost.append(";");
		for (HighSchoolInfo highSchoolInfo : hostHighSchoolList) {
			sbHost.append(highSchoolInfo.getHighSchoolName() + "|");
		}
		// 被搜索用户资料
		List<WorkplaceInfo> userWorkPlaceList = spaceHome
				.getWorkplaceInfoList(userId);
		List<UniversityInfo> userUniversityList = spaceHome
				.getUniversityInfoList(userId);
		List<HighSchoolInfo> userHighSchoolList = spaceHome
				.getHighschoolInfoList(userId);
		// 组装被搜索用户资料
		StringBuffer sbUser = new StringBuffer();
		for (WorkplaceInfo workPlaceInfo : userWorkPlaceList) {
			sbUser.append(workPlaceInfo.getWorkplaceName() + "|");
		}
		sbUser.append(";");
		for (UniversityInfo universityInfo : userUniversityList) {
			sbUser.append(universityInfo.getUniversityName() + "|");
		}
		sbUser.append(";");
		for (HighSchoolInfo highSchoolInfo : userHighSchoolList) {
			sbUser.append(highSchoolInfo.getHighSchoolName() + "|");
		}
		// 重新搜索一次
		// 组装数据
		sb.append(LogProcessor.getBlackets(String.valueOf(time)));
		sb.append(LogProcessor.getBlackets(String.valueOf(hostId)));
		sb.append(LogProcessor.getBlackets(String.valueOf(userId)));
		sb.append(LogProcessor.getBlackets(String.valueOf(query)));
		sb.append(LogProcessor.getBlackets(String.valueOf(json)));
		sb.append(LogProcessor.getBlackets(String.valueOf(relation)));
		sb.append(LogProcessor.getBlackets(String.valueOf(commonFriends)));
		sb.append(LogProcessor.getBlackets(String.valueOf(totalCount)));
		sb.append(LogProcessor.getBlackets(String.valueOf(index)));
		sb.append(LogProcessor.getBlackets(String.valueOf(position)));
		sb.append(LogProcessor.getBlackets(sbHost.toString()));
		sb.append(LogProcessor.getBlackets(sbUser.toString()));
		return sb.toString();
	}

	private static boolean isNumeric(String str) {
		if (str == null || str.length() == 0) {
			return false;
		}
		Pattern pattern = Pattern.compile("[0-9]+");
		Matcher isNum = pattern.matcher(str);
		if (!isNum.matches()) {
			return false;
		}
		return true;
	}

	private static String getBlackets(String content) {
		return "[" + content + "]  ";
	}

	public static void main(String[] args) {
		Map<String, String> map = new HashMap<String, String>();
		map.put("time",
				String.valueOf(Calendar.getInstance().getTimeInMillis()));
		map.put("hostId", String.valueOf(201783098));
		map.put("userId", String.valueOf(123456));
		map.put("query", "危烨虹");
		map.put("relation", String.valueOf(""));
		map.put("commonFriends", String.valueOf(235));
		map.put("totalCount", String.valueOf(453));
		map.put("position", String.valueOf(4));
		System.out.println(LogProcessor.generateLog(map));
	}
}
