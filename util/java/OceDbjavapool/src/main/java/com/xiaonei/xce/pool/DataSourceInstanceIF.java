package com.xiaonei.xce.pool;

import javax.sql.DataSource;

public interface DataSourceInstanceIF {
	public static final String CLASSNAME_MYSQL = "com.mysql.jdbc.Driver";
	public static final String CLASSNAME_POSTGRESQL = "org.postgresql.Driver";
	public static final String EMPTY_PATTERN = "";
	
	public DataSource getWriter(String pattern);

	public DataSource getReader(String pattern);

	public int getTimeStamp();

	public void close();
	
	public void detect();
	
	public void cutReadServer(String description);
}
