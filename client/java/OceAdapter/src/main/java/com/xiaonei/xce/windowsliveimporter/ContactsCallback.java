package com.xiaonei.xce.windowsliveimporter;

public interface ContactsCallback {
	public void finished(Contacts contacts);

	public void exception(Exception e);
}
