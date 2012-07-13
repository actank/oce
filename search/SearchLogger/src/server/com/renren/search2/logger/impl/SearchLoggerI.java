// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1

package com.renren.search2.logger.impl;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

import org.apache.log4j.Logger;
import org.apache.log4j.xml.DOMConfigurator;

import Ice.Current;

import com.renren.search2.logger.LogProcessor;
import com.renren.search2.logger._SearchLoggerDisp;
import com.renren.search2.logger.condition.ConditionProcessor;
import com.renren.search2.logger.util.Analyze;

public final class SearchLoggerI extends _SearchLoggerDisp {

	private static final long serialVersionUID = 4621280078303520622L;

	public SearchLoggerI() {
	}

	public void access(java.util.Map<java.lang.String, java.lang.String> log,
			Ice.Current __current) {
		String logStr = "";
		if (log.get("hostId") != null
				&& Integer.parseInt(log.get("hostId")) % 1000 == 98) {
			logStr = Analyze.parse(SearchTool.explain(
					ConditionProcessor.getIndexCondition(log),
					Integer.parseInt(log.get("userId"))).toHtml());
		}
		LogProcessor.logger.info(LogProcessor.generateLog(log) + "  [" + logStr
				+ "]");
		// logger.info(LogProcessor.generateLog(log));
	}

	public String getExplain(Map<String, String> log, Current __current) {
		// System.out.println("2222222222222222222222");
		String logStr = SearchTool.explain(
				ConditionProcessor.getIndexCondition(log),
				Integer.parseInt(log.get("userId"))).toHtml();
		return logStr;
	}

	public static void main(String[] args) {
		Map<String, String> map = new HashMap<String, String>();
		map.put("time",
				String.valueOf(Calendar.getInstance().getTimeInMillis()));
		map.put("hostId", String.valueOf(201783098));
		map.put("userId", String.valueOf(123456));
		map.put("query", "史玉龙");
		map.put("json",
				"[{\"t\":\"base\",\"gend\":\"女生\",\"prov\":\"北京\"},{\"t\":\"age\",\"range\":\"1\"}]");
		map.put("relation", String.valueOf(7));
		map.put("commonFriends", String.valueOf(235));
		map.put("totalCount", String.valueOf(453));
		map.put("position", String.valueOf(4));
		System.out.println(LogProcessor.generateLog(map));
		LogProcessor.logger.info(LogProcessor.generateLog(map));
		// System.out.println(SearchLoggerI.logger.isInfoEnabled());
		// SearchLoggerI.logger.info("测试而已");
	}

}
