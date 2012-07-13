package com.xiaonei.xce.scorecache;

import java.util.Date;
import java.util.Calendar;
import java.util.List;
import java.util.ArrayList;

import xce.scorecache.ScoreDataN;

public abstract class ScoreCache {

	public static enum ScoreSignType {
		TYPE_NO_FOOTPRINT(2, 15, 3)//
		, TYPE_COLOURFUL_TALKBOX(3, 7, 1)//
		, TYPE_SUPER_EMOTION(4, 9, 1)//
		, TYPE_HOME_SKIN(5, 11, 1)//
		;
		public final int type;// the bit in sign
		public final int level;// the score level
		public final int vipLevel;// the vip level

		private ScoreSignType(int type, int level, int vipLevel) {
			this.type = type;
			this.level = level;
			this.vipLevel = vipLevel;
		}

		public boolean isSignTrue(long sign) {
			return 0 != (sign & (1L << type));
		}
		/*
		public boolean isNewlyUpgrade(long sign){
			return 0 != ( sign & 1L );
		}
		public boolean isDoubleScore(long sign) {
			return 0 != (sign & (1L << 62));
		}
		*/
	}

	private int id;

	private int historyLoginDays;
	private int continueLoginDays;
	private int activeDays;
	private long lastLoginTime;
	private int loginType;

	private int score;
	private int level;
	private int nextLevelScore;

	private int awardCount;

	private int vip;

	private long sign;

	public int getId() {
		return id;
	}

	public int getHistoryLoginDays() {
		return historyLoginDays;
	}

	public int getContinueLoginDays() {
		return continueLoginDays;
	}

	public List<Boolean> getActiveDays() {
		List<Boolean> res = new ArrayList<Boolean>();
		for( int i=1; i<=31; ++i){
			res.add( (activeDays&(1<<i))>0 );
		}
		return res;
	}

	public long getLastLoginTime() {
		return lastLoginTime;
	}

	public int getLoginType() {
		return loginType;
	}

	public int getScore() {
		return score / 100;
	}

	public int getLevel() {
		return level;
	}

	public int getNextLevelScore() {
		return nextLevelScore / 100;
	}

	public int getAwardCount() {
		return awardCount;
	}

	public boolean isVipTrue() {
		return vip > 0;
	}

	public int getVipLevel() {
		if (vip < 1)
			return 1;
		return vip;
	}

	public boolean isSignTrue(ScoreSignType type) {
		//return 0 != (sign & (1L << type.type));
		return level>=type.level || vip>=type.vipLevel;
	}

	public boolean isNewlyUpgrade() {
		return 0 != ( sign & 1L );
	}

	public boolean isDoubleScore() {
		return 0 != (sign & (1L << 62));
	}

	public int fetchRealScore() {
		return score;
	}

	public int fetchRealNextLevelScore() {
		return nextLevelScore;
	}

	public int fetchRealVip() {
		return vip;
	}

	public long fetchSign() {
		return sign;
	}

	protected void setBase(ScoreDataN base) {
		id = base.id;
		historyLoginDays = base.historyLoginDays;
		continueLoginDays = base.continueLoginDays;
		lastLoginTime = base.lastLoginTime;

		activeDays = base.activeDays;
		int abs_count = 0;
		Calendar c = Calendar.getInstance();
		c.set(Calendar.HOUR_OF_DAY, 0);
		c.set(Calendar.MINUTE, 0);
		c.set(Calendar.SECOND, 0);
		c.set(Calendar.MILLISECOND, 0);
		long time_zero = c.getTimeInMillis();
		int days;
		if( time_zero < lastLoginTime ){
			days = -1;
		}else{
			days = ( (int)(time_zero/1000) - (int)(lastLoginTime/1000) ) / ( 24*3600 );
		}
		if( days==0 ){
			abs_count = -1;
		} else if( days>=30 ){
			activeDays = 0+(activeDays%2);
			abs_count = -1;
		} else{
			abs_count = days;
		}
		activeDays = ((((activeDays/2)*2)<<(abs_count+1))+(activeDays%2))&(0x7FFFFFFF);

		loginType = base.loginType;
		score = base.score;
		level = base.level;
		nextLevelScore = base.nextLevelScore;
		awardCount = base.awardCount;
		vip = base.vip;
		sign = base.sign;
	}

	@Override
	public String toString() {
		return "ScoreCache [id=" + id + ", historyLoginDays="
				+ historyLoginDays + ", continueLoginDays=" + continueLoginDays
				+ ", activeDays=" + activeDays
				+ ", lastLoginTime=" + new Date(lastLoginTime) + ", awardCount="
				+ awardCount + ", loginType=" + loginType + ", score=" + score
				+ ", level=" + level + ", nextLevelScore=" + nextLevelScore
				+ ", vip=" + vip + ", sign=" + sign + "]";
	}



}
