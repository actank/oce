package com.xiaonei.xce.pool;

import java.sql.Connection;
import java.sql.SQLException;

import javax.sql.DataSource;

public interface ConnectionPoolManagerIF {
	public Connection getReader(String name, String pattern) throws SQLException;

	public Connection getWriter(String name, String pattern) throws SQLException;

	public Connection getReader(String name) throws SQLException;

	public Connection getWriter(String name) throws SQLException;

	public DataSource getReaderDataSource(String name, String pattern) throws SQLException;

	public DataSource getWriterDataSource(String name, String pattern) throws SQLException;

	public DataSource getReaderDataSource(String name) throws SQLException;

	public DataSource getWriterDataSource(String name) throws SQLException;
	
	public DataSourceInstanceIF getInstance(String name);
	
	public void detect();
	
	public void alert(String name,String info);
}
