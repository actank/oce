package com.xiaonei.xce.pool.old;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import javax.sql.DataSource;

import org.apache.commons.dbcp.BasicDataSource;

import com.mysql.jdbc.jdbc2.optional.MysqlDataSource;
import com.xiaonei.xce.DbInstance;
import com.xiaonei.xce.DbServer;
import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.dbpool.XceDataSource;
import com.xiaonei.xce.dbpool.impl.MySqlConnectionFactory;
import com.xiaonei.xce.detector.DataSourceDetector;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.pool.DataSourceInstanceIF;

public class DataSourceSinglerInstance implements DataSourceInstanceIF {
    org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
	.getLog(XceAdapter.class);
  private static final boolean USE_XCEDS = System.getProperty("USE_XCEDS") != null;

  public DataSourceSinglerInstance(DbInstance db) {
    _db = db;
    _trouble = false;
    new ConcurrentHashMap<String,Integer>();
  }

  private DbInstance _db;
  private DataSource _writer;
  private DataSource[] _readers;
  private boolean _trouble;
  
  protected synchronized DataSource buildDbServer(DbServer server) {
    if (USE_XCEDS && server.type.equals("mysql")) {
      MysqlDataSource realDatasource = new MysqlDataSource();
      realDatasource.setUser(server.user);
      realDatasource.setPassword(server.password);
      realDatasource.setServerName(server.host);
      if (server.port != 0) {
        realDatasource.setPort(server.port);
      }
      realDatasource.setDatabaseName(server.database);
      realDatasource.setConnectTimeout(100);
      realDatasource.setAutoReconnect(true);
      MySqlConnectionFactory factory = new MySqlConnectionFactory(
          realDatasource);
      XceDataSource ds = new XceDataSource(server.host, 3, 1000, factory);
      return ds;
    } else {
      BasicDataSource ds = new BasicDataSource();
      if (server.type.equals("postgresql")) {
        ds.setDriverClassName(CLASSNAME_POSTGRESQL);
        ds.setUrl("jdbc:postgresql://" + server.host
            + (server.port == 0 ? "" : ":" + String.valueOf(server.port)) + "/"
            + server.database);
      } else if (server.type.equals("mysql")) {
        ds.setDriverClassName(CLASSNAME_MYSQL);
        ds.setUrl("jdbc:mysql://" + server.host
            + (server.port == 0 ? "" : ":" + String.valueOf(server.port)) + "/"
            + server.database + "?user=" + server.user + "&password="
            + server.password + "&connectTimeout=100");
      } else {
        throw new NullPointerException(
            "server type must be either postgresql or mysql, while this server type is: "
                + server.type);
      }
      ds.setUsername(server.user);
      ds.setPassword(server.password);
      ds.setInitialSize(0);// This ensures no idle connection which only
      // for
      // initialize.
      ds.setMaxActive(1000);
      ds.setMinIdle(0);
      ds.setMaxIdle(ds.getMaxActive() / 10 + 1);
      ds.setMaxWait(10);
      ds.setTestOnReturn(false);
      ds.setTestWhileIdle(true);
      ds.setMinEvictableIdleTimeMillis(10 * 1000);
      ds.setNumTestsPerEvictionRun(2);
      ds.setTimeBetweenEvictionRunsMillis(10000);
      ds.setValidationQuery("SELECT 3");
      return ds;
    }
  }

  private synchronized void buildWriter() {
    if (null != _writer) {
      return;
    }
    _writer = buildDbServer(_db.wserver);
  }

  public DataSource getWriter(String pattern) {
    buildWriter();
    return _writer;
  }

  public synchronized void buildReaders() {
    if (_readers != null) {
      return;
    }
    _readers = new DataSource[_db.rservers.length];
    for (int buildReader = 0; buildReader < _db.rservers.length; ++buildReader) {
      _readers[buildReader] = buildDbServer(_db.rservers[buildReader]);
    }
  }

  public DataSource getReader(String pattern) {
    buildReaders();
    return _readers[(int) (Math.random() * _readers.length)];
  }

  public int getTimeStamp() {
    return _db.timestamp;
  }

  public void close() {
    closeDataSource(_writer);
    if (_readers == null)
      return;
    for (DataSource reader : _readers) {
      closeDataSource(reader);
    }
  }

  private void closeDataSource(DataSource ds) {
    if (ds instanceof BasicDataSource) {
      BasicDataSource new_name = (BasicDataSource) ds;
      try {
        new_name.close();
      } catch (SQLException e) {
        Oce.getLogger().error(this.getClass().getName() + " " + e);
      }
    }
  }

  @Override
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append("SinglerInstance: ").append(_db.name).append(" ");
    if (_writer != null) {
      buf.append("W[");
      if (_writer instanceof BasicDataSource) {
        buf.append(((BasicDataSource) _writer).getNumActive()).append(",")
            .append(((BasicDataSource) _writer).getNumIdle()).append("] ");
      } else if (_writer instanceof XceDataSource) {
      }
      buf.append("] ");
    }
    if (_readers != null) {
      for (int x = 0; x < _readers.length; ++x) {
        if (_readers[x] != null) {
          buf.append("R" + x + "[");
          if (_readers[x] instanceof BasicDataSource) {
            buf.append(((BasicDataSource) _readers[x]).getNumActive()).append(
                ",").append(((BasicDataSource) _readers[x]).getNumIdle());
          } else if (_readers[x] instanceof XceDataSource) {
          }
          buf.append("] ");
        }
      }
    }
    return buf.toString();
  }
  
  public void detect(){
      if(_trouble){
	  return;
      }
      for(int buildReader = 0; buildReader < _db.rservers.length; ++buildReader){
	  if(!isConnected(_db.rservers[buildReader])){
	      DataSourceDetector.getInstance().redetect(_db.name,this,generateKey(_db.rservers[buildReader]));
	  }
      }
  }
  
  private String generateKey(DbServer server){
      return server.type+":"+server.host+":"+server.port+":"+server.user+":"+server.password+":"+server.database+":"+server.charset;
  }
  
  private boolean isConnected(DbServer server) {
      Socket s = null;
      try {
	  s = new Socket();
	  s.connect(new InetSocketAddress(server.host, server.port), 300);
	  return true;
      } catch (UnknownHostException e) {
    	  Oce.getLogger().error(this.getClass().getName()+".isConnected "+generateKey(server), e);
      } catch (IOException e) {
    	  Oce.getLogger().error(this.getClass().getName()+".isConnected "+generateKey(server), e);
      } catch (Exception e){
    	  Oce.getLogger().error(this.getClass().getName()+".isConnected "+generateKey(server), e);
      } finally {
        if (s != null){
          try {
          s.close();
          } catch (Exception e) {
          }
        }
      }
      Oce.getLogger().error(this.getClass().getName()+".isConnected "+generateKey(server) +" isn't connected");
      return false;
  }
  
  public void cutReadServer(String description){
      if(_trouble){
	  return;
      }
      _trouble=true;
      _logger.warn("cutReadServer "+description);
      List<DataSource> ds=new ArrayList<DataSource>();
      for (int buildReader = 0; buildReader < _db.rservers.length; ++buildReader) {
	  if(generateKey(_db.rservers[buildReader]).equals(description)){
	      continue;
	  }
	  ds.add(buildDbServer(_db.rservers[buildReader]));
      }
      if(ds.size()>0){
	  _readers = ds.toArray(new DataSource[ds.size()]);
      }
  }
}
