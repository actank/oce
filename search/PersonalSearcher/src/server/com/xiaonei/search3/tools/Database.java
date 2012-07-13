package com.xiaonei.search3.tools;

import java.io.IOException;

import com.xiaonei.search.util.db.DatabaseFactory;
import com.xiaonei.search.util.db.DatabaseI;

public class Database {
	private DatabaseI[] database_;
	private String[] conn_;
	private int mod;

	public Database(String[] conn) {
		mod = conn.length;
		conn_ = conn;
		database_ = new DatabaseI[mod];
		for (int i = 0; i < mod; ++i) {
			try {
				database_[i] = DatabaseFactory.getInstance().createDatabase();
				database_[i].open(conn_[i]);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public Object query(int userId) {
		try {
			return database_[userId % mod].query(userId);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}
}
