package com.xiaonei.xce.dbpool;

import java.sql.Connection;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.atomic.AtomicInteger;

import com.xiaonei.xce.FactoryIF;
import com.xiaonei.xce.dbpool.XceDataSource.Shrink;

public class ConnectionPool implements com.xiaonei.xce.PoolIF<Connection> {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog("XceDataSource");
	private final BlockingDeque<Connection> _queue = new LinkedBlockingDeque<Connection>();
	private final int _maxSize;
	private final AtomicInteger _currentSize = new AtomicInteger(0);
	private final ConcurrentHashMap<Connection, Integer> _borrowed = new ConcurrentHashMap<Connection, Integer>();
	private final FactoryIF<Connection> _factory;
	private final AtomicInteger _toShinkSize = new AtomicInteger(0);
	private final String _name;

	public ConnectionPool(String name, int initSize, int maxSize,
			FactoryIF<Connection> factory) {
		_name = name;
		_maxSize = maxSize;
		_factory = factory;
		for (int i = 0; i < initSize; ++i) {
			try {
				_queue.add(new PooledConnection(_factory.makeObject(), this));
				_currentSize.incrementAndGet();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		Shrink.registryConnectionPool(this);
	}

	@Override
	public Connection borrowObject() {
		Connection conn = _queue.pollFirst();
		if (conn != null) {
			_borrowed.put(conn, 0);
			return conn;
		}

		int size = _currentSize.incrementAndGet();

		if (size > _maxSize) {
			_currentSize.decrementAndGet();
			return null;
		}
		Connection realConn = _factory.makeObject();
		if (realConn == null) {
			_currentSize.decrementAndGet();
			return null;
		}
		conn = new PooledConnection(realConn, this);
		if (conn != null) {
			_borrowed.put(conn, 0);
			return conn;
		}
		return null;
	}

	@Override
	public void returnObject(Connection obj) {
		if (obj == null)
			throw new NullPointerException();
		if (_borrowed.remove(obj) == null)
			throw new NullPointerException(
					"return object not belongs this pool");
		if (_toShinkSize.getAndDecrement() > 0) {
			_factory
					.destoryObject(((PooledConnection) obj).getRealConnection());
		} else {
			_queue.offerLast(obj);
		}
	}

	protected void shrink(int count) {
		int x = _toShinkSize.getAndSet(count);
		if (_logger.isDebugEnabled()) {
			_logger.debug("[ConnectionPool][" + _name + "] shrink : unshinked:"
					+ x + " count:" + count);
		}
		if (x > 0) {
			// last shrink cannot be applied.
			for (int t = 0; t < x; ++t) {
				returnObject(borrowObject());
			}
		}
	}

	public int getIdleSize() {
		return _queue.size();
	}

	public void close() {
		Shrink.unregistryConnectionPool(this);
	}

	@Override
	public String toString() {
		return _name;
	}
}
