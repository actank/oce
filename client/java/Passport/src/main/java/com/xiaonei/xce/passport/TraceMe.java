package com.xiaonei.xce.passport;

import java.util.HashMap;
import java.util.Map;

public class TraceMe {
	public static String getStackTrack() {
		/*
		StringBuffer out = new StringBuffer();
		try {
			throw new NullPointerException();
		} catch (NullPointerException e) {
			StackTraceElement[] ss = e.getStackTrace();
			for (StackTraceElement s : ss) {
				out.append(s.getClassName());
				out.append(":");
				out.append(s.getMethodName());
				out.append(":");
				out.append(s.getFileName());
				out.append(":");
				out.append(s.getLineNumber());
			}
		}
		return out.toString();*/
		return "";
	}

	public static Map<String, String> createStackTraceContext(String src) {
		Map<String, String> out = new HashMap<String, String>();
		out.put("STACK_TRACE", src);
		return out;
	}
}
