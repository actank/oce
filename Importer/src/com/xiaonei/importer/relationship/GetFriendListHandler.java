package com.xiaonei.importer.relationship;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.dbutils.ResultSetHandler;

public class GetFriendListHandler implements ResultSetHandler {

	public Object handle(ResultSet rs) throws SQLException {
		List<Integer> result = new ArrayList<Integer>();
		while(rs.next()){
			result.add(rs.getInt("GUEST"));
		}
		return result;
	}

}
