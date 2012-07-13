package com.xiaonei.xce.footprint;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.xiaonei.xce.ilikeit.ILikeItCacheAdapter;
import com.xiaonei.xce.log.Oce;

import xce.svcapi.TaskManagerAPIPrx;
import xce.svcapi.TaskManagerAPIPrxHelper;
import xce.util.channel.Channel;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;
import MyUtil.ObjectCachePrx;
import MyUtil.ObjectCachePrxHelper;

public class ILikeItPreloader extends Thread {
	private int _pauseTime;
	private int _tableNumBegin;
	private String _driver = "com.mysql.jdbc.Driver";
	// private String _url = "jdbc:mysql://10.3.19.57:3306/ilike";// 外网
	private String _url = "jdbc:mysql://10.22.206.57:3306/ilike";// 内网
	private String _user = "xndev";
	private String _password = "rebornlOM";
	private Connection _conn = null;
	private Statement _statement = null;
	private long[] _maxArray = new long[100];
	private boolean[] _isOverArray = new boolean[100];
	private int _overCount = 0; // 扫描完的表的数量
	private Channel _channel;
	private String _endpoints;
	private final long _cluster = 2; // 服务散的份数
	private final int _category = 0; // 服务的标识
	private final int _limitCount = 500;
	private static int _isValid = 0; // 更改线程数量时一定要改这个变量
	private ILikeItCacheAdapter ilikeAdapter = ILikeItCacheAdapter.instance();

	protected ILikeItPreloader(int pauseTime, int tableNumBegin,
			String endpoints) {
		_pauseTime = pauseTime;
		_tableNumBegin = tableNumBegin;
		_endpoints = endpoints;
		_channel = Channel.newLikeChannel(_endpoints);

		for (int i = 0; i < _maxArray.length; i++) {
			_maxArray[i] = 32001751565715L;
			_isOverArray[i] = false;
		}

		try {
			Class.forName(_driver);
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}

		try {
			_conn = DriverManager.getConnection(_url, _user, _password);
		} catch (SQLException e) {
			e.printStackTrace();
		}
		try {
			if (!_conn.isClosed())
				System.out.println("Succeeded connecting to the Database!");
			_statement = _conn.createStatement();
		} catch (SQLException e) {
			e.printStackTrace();
		}

		// ilikeAdapter.setValid(false);

	}

	public void reload(long[] gids) {
		System.out.println("length......................" + gids.length);
		// 按照服务散开
		Map<Long, List<Long>> reloadMap = new HashMap<Long, List<Long>>();
		for (int i = 0; i < gids.length; ++i) {
			long mod = gids[i] % _cluster;
			if (reloadMap.containsKey(mod)) {
				reloadMap.get(mod).add(gids[i]);
			} else {
				List<Long> list = new ArrayList<Long>();
				list.add(gids[i]);
				reloadMap.put(mod, list);
			}
		}

		for (Map.Entry<Long, List<Long>> entry : reloadMap.entrySet()) {
			long mod = entry.getKey();
			List<Long> list = entry.getValue();
			int size = entry.getValue().size();

			// 将list转为数组
			long[] reloadGids = new long[size];
			for (int j = 0; j < size; ++j) {
				reloadGids[j] = list.get(j);
			}

			ObjectCachePrx objPrx = _channel.locate("SC", _endpoints,
					Channel.Invoke.Twoway, ObjectCachePrxHelper.class, 300);
			if (objPrx != null) {
				try {
					objPrx.preloadObjects(_category, reloadGids);
				} catch (ConnectTimeoutException e) {
					Oce.getLogger().error(
							this.getClass().getName() + ".reload " + e);
				} catch (TimeoutException e) {
					Oce.getLogger().error(
							this.getClass().getName() + ".reload " + e);
				}
			}
		}
	}

	public ResultSet queryDB(int pos) {
		ResultSet rs = null;
		int table = _tableNumBegin + pos;
		String sql = "select distinct gid from like_ship_" + table
				+ " where gid > " + _maxArray[pos] + " limit " + _limitCount;
		// System.out.println(sql);
		try {

			rs = _statement.executeQuery(sql);
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return rs;
	}

	@SuppressWarnings("finally")
	public void run() {
		while (_overCount < 100) {
			for (int i = 0; i < 100; i++) {
				if (!_isOverArray[i]) {// 若扫描完表则停止对该表的查询
					ResultSet rs = queryDB(i);

					if (rs != null) {
						try {

							int countTmp = 0;

							long[] reloadData = new long[500];

							while (rs.next()) {
								long gid = rs.getLong("gid");
								_maxArray[i] = Math.max(_maxArray[i], gid);
								System.out.println("add gid : " + gid);
								reloadData[countTmp] = gid;
								countTmp++;
							}

							try {
								reload(reloadData);
							} catch (ConnectTimeoutException e) {
								Oce.getLogger()
										.error(
												this.getClass().getName()
														+ ".run " + e);
							} catch (TimeoutException e) {
								Oce.getLogger()
										.error(
												this.getClass().getName()
														+ ".run " + e);
							} finally {
							}

							if (countTmp == 0) {// 扫描完该表
								int table = _tableNumBegin + i;
								System.out.println("table like_ship_" + table
										+ " is over!");
								_isOverArray[i] = true;
								_overCount++;

							}
						} catch (SQLException e) {
							e.printStackTrace();
						}
					}
				}
				try {
					Thread.sleep(5);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}

			try {
				Thread.sleep(_pauseTime);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			System.out.println();
		}

		System.out.println("over 1 while");

		while (true) {
			// int clusterTmp = 0;
			// for (int i = 0; i < _cluster; ++i) {

			TaskManagerAPIPrx taskManagerPrx = _channel.locate("TMA",
					_endpoints, Channel.Invoke.Twoway,
					TaskManagerAPIPrxHelper.class, 300);
			if (taskManagerPrx.size(-4) == 0) {
				// clusterTmp++;
				break;
			}
			// }

			// if (clusterTmp == _cluster) {
			// _isValid++;
			// break;
			// }
			try {
				sleep(5);
			} catch (InterruptedException e) {
				e.printStackTrace();
			} finally {
				continue;
			}

		}

		System.out.println("thread " + _tableNumBegin + " is over!");
		System.out.println("isValid............." + _isValid);
		// if (_isValid >= 1) {
		ilikeAdapter.setValid(true);
		Date dd = new Date();
		System.out.println(dd);
		System.exit(0);
		// }

	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ILikeItPreloader t1 = new ILikeItPreloader(1 * 1000, 0,
				"@ILikeItCacheM0");
		t1.start();

		// ILikeItPreloader t2 = new ILikeItPreloader(1 * 1000, 50,
		// "@ILikeItCacheM");
		// t2.start();

		// ILikeItPreloader t3 = new ILikeItPreloader(3 * 100, 40,
		// "@ILikeItCacheM");
		// t3.start();
		//
		// ILikeItPreloader t4 = new ILikeItPreloader(3 * 100, 60,
		// "@ILikeItCacheM");
		// t4.start();
		//
		// ILikeItPreloader t5 = new ILikeItPreloader(3 * 100, 80,
		// "@ILikeItCacheM");
		// t5.start();

	}

}
