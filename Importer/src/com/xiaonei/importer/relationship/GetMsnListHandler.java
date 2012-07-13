package com.xiaonei.importer.relationship;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.dbutils.ResultSetHandler;

public class GetMsnListHandler implements ResultSetHandler {

	public Object handle(ResultSet rs) throws SQLException {
		Map<Integer, String> result = new HashMap<Integer, String>();
		while (rs.next()) {
			result.put(rs.getInt("userid"), rs.getString("email"));
		}
		return result;
	}

}
