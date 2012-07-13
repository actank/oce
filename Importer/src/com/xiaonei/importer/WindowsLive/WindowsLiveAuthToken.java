package com.xiaonei.importer.WindowsLive;

import java.util.Date;

public class WindowsLiveAuthToken {
	public WindowsLiveAuthToken(Date f, Date u, String t) {
		validateFrom = f;
		validateUntil = u;
		token = t;
	}

	public final Date validateFrom;
	public final Date validateUntil;
	public final String token;
}
