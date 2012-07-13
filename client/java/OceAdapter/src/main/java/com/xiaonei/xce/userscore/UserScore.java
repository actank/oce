package com.xiaonei.xce.userscore;

import java.util.Date;

import xce.util.tools.DateFormatter;

public abstract class UserScore implements java.io.Serializable {

	private static final long serialVersionUID = 6265153598556681714L;

	protected UserScore(int userId) {
		this.userId = userId;
	}

	private final int userId;
	private int currentScore;
	private int exchangeScore;
	private int consumeScore;
	private Date dealDate;

	public int getUserId() {
		return userId;
	}

	public int getCurrentScore() {
		return currentScore;
	}

	public void setCurrentScore(int currentScore) {
		this.currentScore = currentScore;
	}

	public int getExchangeScore() {
		return exchangeScore;
	}

	public void setExchangeScore(int exchangeScore) {
		this.exchangeScore = exchangeScore;
	}

	public int getConsumeScore() {
		return consumeScore;
	}

	public void setConsumeScore(int consumeScore) {
		this.consumeScore = consumeScore;
	}

	public Date getDealDate() {
		return dealDate;
	}

	public void setDealDate(Date dealDate) {
		this.dealDate = dealDate;
	}

	@Override
	public String toString() {
		StringBuffer buff = new StringBuffer();
		buff.append("UserScore [userId=" + getUserId());
		buff.append(";currentScore=" + getCurrentScore());
		buff.append(";exchangeScore=" + getExchangeScore());
		buff.append(";consumeScore=" + getConsumeScore());
		buff.append(";dealDate=" + DateFormatter.format(getDealDate()));
		return buff.toString();
	}
}
