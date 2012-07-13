package com.xiaonei.xce.passport;

public interface Transfer {
	String createTicket(int userId, int time);

	int verifyTicket(String ticket);
}
