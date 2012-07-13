package com.xiaonei.xce.userscore;

import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;

import mop.hi.oce.adapter.AdapterFactory;
import xce.generaldbcache.DbFields;
import xce.generaldbcache.DbRows;
import xce.generaldbcache.GeneralDbCacheManagerPrx;
import xce.generaldbcache.GeneralDbCacheManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import xce.util.task.Task;
import xce.util.task.TaskManager;
import xce.util.tools.DateFormatter;

public final class UserScoreAdapter<T extends UserScore> extends ServiceAdapter {
	private class ReloadUserScoreTask extends TimerTask {
		private final int userId;

		public ReloadUserScoreTask(int id) {
			this.userId = id;
		}

		@Override
		public void run() {
			try {
				getGeneralDbCacheManagerOneway(userId).load(userId);
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName() , e);
			}
		}

	}

	private static final String endpoints = "@DbCacheUserScore";
	private UserScoreFactory<T> factory;

	public UserScoreAdapter(UserScoreFactory<T> factory) {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		this.factory = factory;
	}

	private Vector<GeneralDbCacheManagerPrx> managers = new Vector<GeneralDbCacheManagerPrx>();
	private Vector<GeneralDbCacheManagerPrx> managersOneway = new Vector<GeneralDbCacheManagerPrx>();

	protected GeneralDbCacheManagerPrx getGeneralDbCacheManager(int id) {
		return locateProxy(managers, "M", id, Channel.Invoke.Twoway,
				GeneralDbCacheManagerPrxHelper.class, 300);
	}

	protected GeneralDbCacheManagerPrx getGeneralDbCacheManagerOneway(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Oneway,
				GeneralDbCacheManagerPrxHelper.class, 300);
	}

	private volatile boolean isPosGenerated = false;
	private int posUserId = -1;
	private int posCurrentScore = -1;
	private int posExchangeScore = -1;
	private int posConsumeScore = -1;
	private int posDealDate = -1;

	private synchronized void generatePos(int num) {
		if (isPosGenerated) {
			return;
		}
		DbFields res = getGeneralDbCacheManager(num).getDbFields();
		if (res == null) {
			throw new IllegalStateException(
					"user_score table is empty. UserScore will not working.");
		}
		String[] fields = res.fields;
		for (int i = 0; i < fields.length; ++i) {
			String field = fields[i];
			if (field.toUpperCase().equals("USERID")) {
				posUserId = i;
			} else if (field.toUpperCase().equals("CURRENT_SCORE")) {
				posCurrentScore = i;
			} else if (field.toUpperCase().equals("EXCHANGE_SCORE")) {
				posExchangeScore = i;
			} else if (field.toUpperCase().equals("CONSUME_SCORE")) {
				posConsumeScore = i;
			} else if (field.toUpperCase().equals("DEAL_DATE")) {
				posDealDate = i;
			} else {
				Oce.getLogger().error(field
								+ " should not be in fields, something new added. Please update the client");
			}
		}

		if (posUserId == -1 || posCurrentScore == -1 || posExchangeScore == -1
				|| posConsumeScore == -1 || posDealDate == -1) {
			throw new IllegalStateException(
					"Service config error, UserScore will not working");
		} else {
			isPosGenerated = true;
		}
	}

	private void ensurePosGenerated(int num) {
		if (!isPosGenerated) {
			generatePos(num);
		}
	}

	public T getUserScore(int userId) {
		ensurePosGenerated(userId);
		T result = factory.create(userId);
		DbRows rows = getGeneralDbCacheManager(userId).getDbRows(userId);
		if (rows.rows.length != 1) {
			throw new IllegalStateException(
					"GeneralCache should always return 1 row, userid=" + userId);
		}

		result.setCurrentScore(Integer
				.valueOf(rows.rows[0].values[posCurrentScore]));
		result.setConsumeScore(Integer
				.valueOf(rows.rows[0].values[posConsumeScore]));
		result.setExchangeScore(Integer
				.valueOf(rows.rows[0].values[posExchangeScore]));
		result.setDealDate(DateFormatter
				.parse(rows.rows[0].values[posDealDate]));

		return result;
	}

	private Timer timer = new Timer();

	public void reloadUserScore(int userId) {
		timer.schedule(new ReloadUserScoreTask(userId), 3000L);
	}

	public static void incUgcUserScore(int userId, int type, int score) {
		AdapterFactory.getSelfBatchUpdaterAdapter().incUgcUserScore(userId,
				type, score);
	}

	public static void decUgcUserScore(int userId, int type, int score) {
		AdapterFactory.getSelfBatchUpdaterAdapter().decUgcUserScore(userId,
				type, score);
	}

	/**
	 * This will invoke reload also.
	 * 
	 * @param userId
	 * @param score
	 */
	public static void incUserScore(int userId, int score) {
		AdapterFactory.getSelfBatchUpdaterAdapter().incUserScore(userId, score);
	}

	/**
	 * This will invoke reload also.
	 * 
	 * @param userId
	 * @param score
	 */
	public static void decUserScore(int userId, int score) {
		AdapterFactory.getSelfBatchUpdaterAdapter().decUserScore(userId, score);
	}

}
