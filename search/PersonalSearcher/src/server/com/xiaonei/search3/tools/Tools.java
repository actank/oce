package com.xiaonei.search3.tools;

public class Tools {
	private static Tools tools = new Tools();
	private final int mod1 = 1000;

	private final int mod2 = 1000;

	private final String outPath = "/data/xce/XiaoNeiSearch/newIndex/";

	public static Tools getInstance() {
		return tools;
	}

	private int[] x = new int[100];
	private String[] conn = new String[] { "s17:1981", "s18:1981", "s19:1981",
			"s20:1981" };

	public String[] getConnections() {
		return conn;
	}

	public String getIndexPath(int userId) {
		return outPath + userId % mod1 + "/" + (userId / mod1) % mod2 + "/"
				+ userId;
	}
}
