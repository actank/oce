package com.xiaonei.xce.datasource.test;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Timer;
import java.util.TimerTask;

import javax.rmi.CORBA.Tie;

import com.mysql.jdbc.jdbc2.optional.MysqlDataSource;
import com.xiaonei.xce.dbpool.XceDataSource;
import com.xiaonei.xce.dbpool.impl.MySqlConnectionFactory;

public class Try {

	private static final String CONNECTION = "jdbc:mysql://10.2.18.45:3306/fb?user=ipo2008&password=meiguiyuan&connectTimeout=100";

	/**
	 * @param args
	 * @throws Throwable
	 */
	public static void main(String[] args) throws Throwable {
		doSql();
		Thread.sleep(100 * 1000);

	}

	private static void doSql() throws Throwable {
		Class.forName("com.mysql.jdbc.Driver");
		MysqlDataSource ds = new MysqlDataSource();
		ds.setServerName("10.2.18.45");
		ds.setPort(3306);
		ds.setDatabaseName("fb");
		ds.setUser("ipo2008");
		ds.setPassword("meiguiyuan");
		ds.setConnectTimeout(10000);

		XceDataSource p = new XceDataSource("TryPool", 1, 10,
				new MySqlConnectionFactory(ds));
		Connection conn = p.getConnection();
		PreparedStatement stmt = conn.prepareStatement("SELECT now()");
		ResultSet rs = stmt.executeQuery();
		while (rs.next()) {
			System.out.println(rs.getString(1));
		}
		stmt.close();
		conn.close();
		System.out.println("ok");
	}
}
