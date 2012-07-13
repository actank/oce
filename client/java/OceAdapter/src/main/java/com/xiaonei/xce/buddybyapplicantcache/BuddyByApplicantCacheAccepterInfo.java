package com.xiaonei.xce.buddybyapplicantcache;

import xce.buddy.AccepterInfo;

public abstract class BuddyByApplicantCacheAccepterInfo {
	private int accepter;
	private int time;

	public int getAccepter() {
		return accepter;
	}

	public void setAccepter(int accepter) {
		this.accepter = accepter;
	}

	public int getTime() {
		return time;
	}

	public void setTime(int time) {
		this.time = time;
	}

	public AccepterInfo getBase() {
		AccepterInfo accepter = new AccepterInfo();
		accepter.accepter = this.getAccepter();
		accepter.time = this.getTime();
		return accepter;
	}
	
	protected void setBase(AccepterInfo accepter) {
		this.setAccepter(accepter.accepter);
		this.setTime(accepter.time);
	}

}