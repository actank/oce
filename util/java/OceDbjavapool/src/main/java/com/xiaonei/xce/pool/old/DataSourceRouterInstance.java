package com.xiaonei.xce.pool.old;

import javax.sql.DataSource;

import com.xiaonei.xce.DbInstance;
import com.xiaonei.xce.Route;
import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.pool.DataSourceInstanceIF;

public class DataSourceRouterInstance implements DataSourceInstanceIF {

  org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
      .getLog(XceAdapter.class);

  public DataSourceRouterInstance(ConnectionPoolManager pool, DbInstance db) {
    _pool = pool;
    _db = db;
  }

  private ConnectionPoolManager _pool;
  private DbInstance _db;

  public DataSource getReader(String pattern) {
    return findInstance(pattern).getReader(pattern);
  }

  public DataSource getWriter(String pattern) {
    return findInstance(pattern).getWriter(pattern);
  }

  protected DataSourceInstanceIF findInstance(String pattern) {
    for (Route route : _db.routes) {
      if (pattern.matches(route.expression)) {
        return _pool.getInstance(route.instance);
      }
    }
    _logger.warn(pattern + " does not have a match for " + _db.name);
    return null;
  }

  public int getTimeStamp() {
    return _db.timestamp;
  }

  public void close() {
    // Do nothing for Router.
  }

  @Override
  public String toString() {
    return "RouterInstance: " + _db.name;
  }
  
  public void detect(){
      for(Route r:_db.routes){
	  _pool.getInstance(r.instance).detect();
      }
  }
  
  public void cutReadServer(String description){
      
  }
}
