package com.xiaonei.importer.relationship;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.dbutils.ResultSetHandler;

import com.xiaonei.svc.importer.BuddyInfoStatus;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyUserInfo;

public class FillUserNameHandler implements ResultSetHandler {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(FillUserNameHandler.class);
	private BuddyList _buddy;
	private Map<Integer, String> _id2key;

	public FillUserNameHandler(BuddyList buddy, Map<Integer, String> id2key) {
		_buddy = buddy;
		_id2key = id2key;
	}

	public Object handle(ResultSet rs) throws SQLException {
		while (rs.next()) {
			_logger.debug("FillUserInfo Handler handle");
			try {
				int id = rs.getInt("ID");
				String userName = rs.getString("NAME");

				if (id == _buddy.owner.userId)
					continue;

				BuddyUserInfo buddy = (BuddyUserInfo)_buddy.users.get(_id2key.get(id));
				buddy.userName = userName;				
			} catch (Exception e) {
				e.printStackTrace();
				throw new RuntimeException(e);
			}
		}
		return null;
	}
}
