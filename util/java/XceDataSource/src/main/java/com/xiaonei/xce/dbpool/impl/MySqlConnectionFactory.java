package com.xiaonei.xce.dbpool.impl;

import java.sql.Connection;
import java.sql.SQLException;

import com.mysql.jdbc.jdbc2.optional.MysqlDataSource;
import com.xiaonei.xce.FactoryIF;
import com.xiaonei.xce.dbpool.PooledConnection;

public class MySqlConnectionFactory implements FactoryIF<Connection> {
	private final MysqlDataSource _realDataSource;

	public MySqlConnectionFactory(MysqlDataSource realDataSource) {
		_realDataSource = realDataSource;
	}

	@Override
	public boolean validObject(Connection obj) {
		try {
			return obj.isValid(100);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	@Override
	public void destoryObject(Connection obj) {
		if (obj instanceof PooledConnection) {
			throw new NullPointerException(
					"Cannot destory an PooledConnection. ");
		}
		try {
			obj.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		obj = null;
	}

	@Override
	public Connection makeObject() {
		try {
			return _realDataSource.getConnection();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return null;
	}
}
