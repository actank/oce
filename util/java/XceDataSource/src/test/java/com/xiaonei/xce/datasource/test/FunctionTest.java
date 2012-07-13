package com.xiaonei.xce.datasource.test;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.HashSet;
import java.util.Set;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import com.mysql.jdbc.jdbc2.optional.MysqlDataSource;
import com.xiaonei.xce.dbpool.XceDataSource;
import com.xiaonei.xce.dbpool.impl.MySqlConnectionFactory;

public class FunctionTest {

	private XceDataSource dataSource;

	@Before
	public void init() throws ClassNotFoundException {
		Class.forName("com.mysql.jdbc.Driver");
		MysqlDataSource ds = new MysqlDataSource();
		ds.setServerName("127.0.0.1");
		ds.setPort(3306);
		ds.setDatabaseName("test");
		ds.setUser("bbc");
		ds.setPassword("");
		ds.setConnectTimeout(10000);

		dataSource = new XceDataSource("FunctionTestPool", 10, 10,
				new MySqlConnectionFactory(ds));
	}

	@Test
	public void borrowInstances() throws SQLException {
		Set<Connection> ins = new HashSet<Connection>();
		for (int x = 0; x < 1000; ++x) {
			System.out.println(x);
			Connection conn = dataSource.getConnection();
			ins.add(conn);
			conn.close();
		}
		System.out.println(ins.size());
		Assert.assertEquals("Borrowed 1000 times only 10 object goted.", ins
				.size(), 10);
	}
}
