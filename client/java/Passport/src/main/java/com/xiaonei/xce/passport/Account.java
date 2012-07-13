package com.xiaonei.xce.passport;

import passport.AccountInfo;

public class Account {
	public Account(int id, int success, String account, String domain, int status, String ticket) {
		this.id = id;
		this.success = success;
		this.account = account;
		this.domain = domain;
		this.status = status;
		this.ticket = ticket;
	}
	protected Account(AccountInfo ai, String ticket) {
		this.id = ai.userId;
		this.success = ai.success;
		this.account = ai.account;
		this.domain = ai.domain;
		this.status = ai.status;
		this.ticket = ticket;
	}
	protected Account(AccountInfo ai) {
		this.id = ai.userId;
		this.success = ai.success;
		this.account = ai.account;
		this.domain = ai.domain;
		this.status = ai.status;
		this.ticket = ai.ticket;
	}

	private final int id;
	private final int success;// success == 0 means account and pwd is right
	// ,success ==-1 means no account ,success == -2
	// means password is wrong
	private final String account;
	private final String domain;
	private final int status;
	private final String ticket;

	public int getId() {
		return id;
	}

	public String getAccount() {
		return account;
	}

	public int getSuccess() {
		return success;
	}

	public String getDomain() {
		return domain;
	}

	public int getStatus() {
		return status;
	}

	public String getTicket() {
		return ticket;
	}

	@Override
	public String toString() {
		StringBuffer buff = new StringBuffer();
		buff.append("id=").append(id).append(";");
		buff.append("account=").append(account).append(";");
		buff.append("success=").append(success).append(";");
		buff.append("domain=").append(domain).append(";");
		buff.append("status=").append(status).append(";");
		buff.append("ticket=").append(ticket).append(".");
		return buff.toString();
	}
}
