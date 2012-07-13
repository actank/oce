package com.xiaonei.xce.pool.old;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentHashMap;

import javax.sql.DataSource;

import Ice.Current;

import com.xiaonei.xce.DbDescriptorPrx;
import com.xiaonei.xce.DbDescriptorPrxHelper;
import com.xiaonei.xce.DbInstance;
import com.xiaonei.xce.DbObserver;
import com.xiaonei.xce.DbObserverPrx;
import com.xiaonei.xce.DbObserverPrxHelper;
import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.pool.ConnectionPoolManagerIF;
import com.xiaonei.xce.pool.DataSourceInstanceIF;

public class ConnectionPoolManager extends DbObserver implements
		ConnectionPoolManagerIF {
	/**
     * 
     */
    private static final long serialVersionUID = 1L;

	org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(XceAdapter.class);

	private final DbObserverPrx _selfPrx;

	private Map<String, DataSourceInstanceIF> _ds;

	private final Timer _checkUpdate;

	private final DbDescriptorPrx _subject;

	public ConnectionPoolManager(Ice.ObjectAdapter adapter) {
		_subject = createProxy(adapter.getCommunicator());
		_ds = new ConcurrentHashMap<String, DataSourceInstanceIF>();
		_checkUpdate = new Timer(true);
		_checkUpdate.schedule(new UpdateTask(), 5 * 1000, 60 * 1000);
		_selfPrx = DbObserverPrxHelper.uncheckedCast(adapter.addWithUUID(this));
	}

	private DbDescriptorPrx createProxy(Ice.Communicator ic) {
		DbDescriptorPrx result = DbDescriptorPrxHelper.uncheckedCast(ic
				.stringToProxy("DCS@DbDescriptor").ice_connectionCached(false)
				.ice_timeout(1000));
		return result;
	}

	private void initDbInstance(String name) {
		if (_ds.containsKey(name)) {
			// _ds.remove(name);
			return;
		}
		_logger.info("Begin initializing " + name);
		boolean success = false;
		try {
			DbInstance db = _subject.getDbInstance(name);
			_ds.put(name, createDataSource(db));
			// _subject.unsubscribe(name, _selfPrx);
			_subject.subscribe(name, _selfPrx);
			success = true;
		} catch (Throwable t) {
			Oce.getLogger().error("No such instance name: "+name , t);
		} finally {
			if (!success) {
				_ds.remove(name);
			}
		}
		_logger.info("End initializing " + name);
	}

	private DataSourceInstanceIF createDataSource(DbInstance db) {
		if (db.type.equals("singler")) {
			_logger.info("Initializing singler instance: " + db.name);
			return new DataSourceSinglerInstance(db);
		} else if (db.type.equals("router")) {
			_logger.info("Initializing router instance: " + db.name);
			return new DataSourceRouterInstance(this, db);
		} else {
			throw new NullPointerException(
					"Instance type must be either singler or router. While this instance type is: " + db.type + " name:" + db.name);
		}
	}

	public Connection getWriter(String name) throws SQLException {
		return getWriter(name, DataSourceInstanceIF.EMPTY_PATTERN);
	}

	public Connection getWriter(String name, String pattern)
			throws SQLException {
		TimeCost cost = TimeCost.begin("DbJavaPool getWriter " + name + " "
				+ pattern);
		try {
			DataSource ds = getWriterDataSource(name, pattern);
			return ds.getConnection();
		} catch (SQLException e) {
			_logger.warn("ConnectionPoolException: getWriter(" + name + ", "
					+ pattern + "). Error " + e.getErrorCode() + ":"
					+ e.getMessage());
			throw e;
		} catch (Exception e) {
			_logger.warn("ConnectionPoolException: getWriter(" + name + ", \""
					+ pattern + "\"). Error: " + e.getMessage());
			Oce.getLogger().error(this.getClass().getName() + " instance: "+name +" pattern: "+pattern, e);
			return null;
		} finally {
			cost.endFinally();
		}
	}

	public DataSource getWriterDataSource(String name, String pattern) {
		DataSourceInstanceIF dsi = getInstance(name);
		DataSource ds = dsi.getWriter(pattern);
		return ds;
	}

	public DataSource getWriterDataSource(String name) {
		DataSourceInstanceIF dsi = getInstance(name);
		DataSource ds = dsi.getWriter(DataSourceInstanceIF.EMPTY_PATTERN);
		return ds;
	}

	public Connection getReader(String name) throws SQLException {
		return getReader(name, DataSourceInstanceIF.EMPTY_PATTERN);
	}

	public Connection getReader(String name, String pattern)
			throws SQLException {
		TimeCost cost = TimeCost.begin("DbJavaPool getReader " + name + " "
				+ pattern);
		try {
			DataSource ds = getReaderDataSource(name, pattern);
			return ds.getConnection();
		} catch (SQLException e) {
			_logger.warn("ConnectionPoolException: getReader(" + name + ", "
					+ pattern + "). Error " + e.getErrorCode() + ":"
					+ e.getMessage());
			throw e;
		} catch (Exception e) {
			_logger.warn("ConnectionPoolException: getReader(" + name + ", \""
					+ pattern + "\"). Error: " + e.getMessage());
			Oce.getLogger().error(this.getClass().getName()+" instance: "+name+" pattern: "+pattern ,e);
			return null;
		} finally {
			cost.endFinally();
		}
	}

	public DataSource getReaderDataSource(String name, String pattern) {
		DataSourceInstanceIF dsi = getInstance(name);
		DataSource ds = dsi.getReader(pattern);
		return ds;
	}

	public DataSource getReaderDataSource(String name) {
		DataSourceInstanceIF dsi = getInstance(name);
		DataSource ds = dsi.getReader(DataSourceInstanceIF.EMPTY_PATTERN);
		return ds;
	}

	@Override
	public DataSourceInstanceIF getInstance(String name) {
		initDbInstance(name);
		DataSourceInstanceIF ds = _ds.get(name);
		return ds;
	}

	public void updated(DbInstance changedInstance, Current __current) {
		_logger.info("update...");
		String name = changedInstance.name;
		if (!_ds.containsKey(name)) {
			_logger.info(name + " unused.");
			return;
		}
		DataSourceInstanceIF oldDataSource = _ds.get(name);

		try {
			String strOldDs = oldDataSource.toString();
			_logger.info(strOldDs);
		} catch (Exception e) {
			_logger
					.warn("printState exception. This is not affect anything. DON'T PANNIC");
			Oce.getLogger().error(this.getClass().getName() + " instance: "+name, e);
		}

		if (changedInstance.timestamp <= oldDataSource.getTimeStamp()) {
			_logger.info(name + " unchanged.");
			return;
		}
		DataSourceInstanceIF newDataSource = createDataSource(changedInstance);
		_ds.remove(name);
		_ds.put(name, newDataSource);
		oldDataSource.close();
	}

	private class UpdateTask extends TimerTask {
		@Override
		public void run() {
			try {
				for (String name : _ds.keySet()) {
					DbInstance db = _subject.getDbInstance(name);
					_logger.info("automatic update " + db.name);
					updated(db, new Ice.Current());
					_subject.subscribe(name, _selfPrx);
				}
			} catch (Exception e) {
				Oce.getLogger().error(this.getClass().getName()+" update" ,e);
			}
		}
	}
	
	public void detect(){
	    for(DataSourceInstanceIF i : _ds.values()){
		i.detect();
	    }
	}

	@Override
	public boolean isMasterConnected(String host, int port, Current c) {
      Socket s=null;
	    try {
		  s = new Socket();
		  s.connect(new InetSocketAddress(host, port), 300);
		  return true;
	      } catch (UnknownHostException e) {
		e.printStackTrace();
	      } catch (IOException e) {
		e.printStackTrace();
	      } catch (Exception e){
		  e.printStackTrace();
	      } finally {
          if (s!=null){
            try {
              s.close();
            } catch (Exception e){
            }
          }
        }
	      return false;
	}

	@Override
	public void alert(String name, String info) {
	    _subject.alert(name, info);
	}
}
