package com.xiaonei.xce.pool.tester;

import com.xiaonei.xce.log.Oce;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

//import junit.framework.TestCase;

import com.xiaonei.xce.XceAdapter;

public class ServiceTester {// extends TestCase {
	public static void main(String[] args) throws Throwable {
		ServiceTester o = new ServiceTester();
		o.testService();
	}
	
	public void testService() throws Exception {
		List<Thread> t = new ArrayList<Thread>();
		for (int i = 0; i < 1; ++i) {
			Thread a = new The();
			a.start();
			t.add(a);
		}
		Thread.sleep(10 * 60 * 1000);
	}

	public class The extends Thread {

		@Override
		public void run() {
			while (true) {
				Connection con = null;
				Statement stmt = null;
				ResultSet res = null;
				try {
					con = XceAdapter.getInstance().getReadConnection("dbjavatest");
					stmt = con.createStatement();
					res = stmt.executeQuery("SELECT * FROM test");
					while (res.next()) {
						Oce.getLogger().info(res.getString("id"));
					}
				} catch (Exception e) {
					Oce.getLogger().error(this.getClass().getName() + " " + e);
				} finally {
					try {
						res.close();
					} catch (Exception e) {
						Oce.getLogger().error(this.getClass().getName() + " " + e);
					}
					try {
						stmt.close();
					} catch (Exception e) {
						Oce.getLogger().error(this.getClass().getName() + " " + e);
					}
					try {
						con.close();
					} catch (Exception e) {
						Oce.getLogger().error(this.getClass().getName() + " " + e);
					}
				}
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					Oce.getLogger().error(this.getClass().getName() + " " + e);
				}
			}
		}
	}
}
