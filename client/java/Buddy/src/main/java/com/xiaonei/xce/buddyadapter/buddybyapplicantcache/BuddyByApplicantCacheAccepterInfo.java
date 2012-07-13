package com.xiaonei.xce.buddyadapter.buddybyapplicantcache;

import xce.buddy.AccepterInfo;

/**
 * @author yuyang
 *
 */
public abstract class BuddyByApplicantCacheAccepterInfo {
	private int accepter;
	private int time;

	/**
	 * @return
	 */
	public int getAccepter() {
		return accepter;
	}

	/**
	 * @param accepter
	 */
	public void setAccepter(int accepter) {
		this.accepter = accepter;
	}

	/**
	 * @return
	 */
	public int getTime() {
		return time;
	}

	/**
	 * @param time
	 */
	public void setTime(int time) {
		this.time = time;
	}

	/**
	 * @return
	 */
	public AccepterInfo getBase() {
		AccepterInfo accepter = new AccepterInfo();
		accepter.accepter = this.getAccepter();
		accepter.time = this.getTime();
		return accepter;
	}
	
	/**
	 * @param accepter
	 */
	protected void setBase(AccepterInfo accepter) {
		this.setAccepter(accepter.accepter);
		this.setTime(accepter.time);
	}

}
