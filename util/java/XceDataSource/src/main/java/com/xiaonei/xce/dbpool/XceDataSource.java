package com.xiaonei.xce.dbpool;

import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import javax.sql.DataSource;

import com.mysql.jdbc.SQLError;
import com.xiaonei.xce.FactoryIF;

public class XceDataSource implements DataSource {

	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog("XceDataSource");

	public static class Shrink {

		private final static class ShrinkThread extends Thread {
			@Override
			public void run() {
				while (true) {
					for (Entry<ConnectionPool, int[]> entry : _idleSizeSnapshots
							.entrySet()) {
						try {
							ConnectionPool key = entry.getKey();
							int[] value = entry.getValue();
							if (_logger.isDebugEnabled()) {
								_logger.debug("[ShrinkThread][" + key + "]"
										+ Arrays.toString(value));
							}
							int currentPos = value[0];
							int currentSize = key.getIdleSize();
							value[currentPos++] = currentSize;
							if (currentPos >= 11) {
								currentPos = 1;
							}
							value[0] = currentPos;
							entry.setValue(value);

							int minIdle = value[1];
							for (int x = 1; x < 11; ++x) {
								minIdle = minIdle < value[x] ? minIdle
										: value[x];
							}
							key.shrink(minIdle);
						} catch (Exception e) {
							e.printStackTrace();
						}
					}
					try {
						Thread.sleep(1 * 1000L);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		}

		private static final ShrinkThread _shrinkThread = new ShrinkThread();
		static {
			_shrinkThread.setDaemon(true);
			_shrinkThread.start();
		}

		private static final Map<ConnectionPool, int[]> _idleSizeSnapshots = new HashMap<ConnectionPool, int[]>();

		public synchronized static void registryConnectionPool(
				ConnectionPool thePool) {
			_idleSizeSnapshots.put(thePool, new int[11]);
		}

		public synchronized static void unregistryConnectionPool(
				ConnectionPool thePool) {
			_idleSizeSnapshots.remove(thePool);
		}
	}

	private ConnectionPool _pool;
	private PrintWriter _logWriter;
	private String url_;

	public XceDataSource(String name, int initSize, int maxSize,
			FactoryIF<Connection> factory) {
		_pool = new ConnectionPool(name, initSize, maxSize, factory);
		url_ = "";
	}

	public XceDataSource(String name, int initSize, int maxSize,
		FactoryIF<Connection> factory, String url) {
		_pool = new ConnectionPool(name, initSize, maxSize, factory);
		url_ = url;
	}

	@Override
	public Connection getConnection() throws SQLException {
		try {
			return _pool.borrowObject();
		} catch (Throwable e) {
			e.printStackTrace();
			throw new SQLException(
					"[XceDataSource] cannot borrow from the pool");
		}
	}

	@Override
	public Connection getConnection(String username, String password)
			throws SQLException {
		_logger
				.error("[XceDataSource] getConnection(username, password) is identical to getConnection()");
		return getConnection();
	}

	@Override
	public PrintWriter getLogWriter() throws SQLException {
		return this._logWriter;
	}

	@Override
	public int getLoginTimeout() throws SQLException {
		return 0;
	}

	@Override
	public void setLogWriter(PrintWriter out) throws SQLException {
		this._logWriter = out;
	}

	@Override
	public void setLoginTimeout(int seconds) throws SQLException {
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		throw SQLError.notImplemented();
	}

	public String getUrl() {
		return url_;
	}
}
