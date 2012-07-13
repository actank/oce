package com.xiaonei.importer.relationship;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.dbutils.ResultSetHandler;

import com.xiaonei.svc.importer.BuddyInfoStatus;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyUserInfo;

public class FillUserBaseHandler implements ResultSetHandler {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(FillUserBaseHandler.class);
	private BuddyList _buddy;
	private Map<Integer, String> _id2key;

	public FillUserBaseHandler(BuddyList buddy, Map<Integer, String> id2key) {
		_buddy = buddy;
		_id2key = id2key;
	}

	// select users.id, users.email, users.name, user_basic.tinyurl,
	// network_history.network_id, network_history.stage,
	// network_history.network_name FROM users, user_basic, network_history
	// WHERE users.id=user_basic.id AND network_history.userid=users.id AND
	// network_history.status=-1 AND users.email IN
	public Object handle(ResultSet rs) throws SQLException {
		while (rs.next()) {
			_logger.debug("FillUserInfo Handler handle");
			try {
				int id = rs.getInt("ID");
//				String email;
//				email = rs.getString("EMAIL");
//				String userName = rs.getString("NAME");
				String tinyurl = rs.getString("TINYURL");
//				int currentNetworkId = rs.getInt("NETWORK_ID");
//				int currentNetworkStage = rs.getInt("STAGE");
//				String currentNetworkName = rs.getString("NETWORK_NAME");
//				if (_logger.isDebugEnabled()) {
//					_logger.debug("email = " + email + " id=" + id + " name="
//							+ userName);
//				}
//
//				String key = email;
//				if (msnIdList != null) {
//					key = msnIdList.get(id);
//				}
//				if (_buddy.users.containsKey(key)) {
//					_buddy.users.remove(key);
//				}

				if (id == _buddy.owner.userId)
					// _buddy.buddies[_email2pos.get(key)].status = new
					// BuddyInfoStatus(
					// -1, "SELF");
					continue;
//				else
//					_buddy.buddies[_email2pos.get(key)].status = new BuddyInfoStatus(
//							1, "USER");
				BuddyUserInfo buddy = (BuddyUserInfo)_buddy.users.get(_id2key.get(id));
				buddy.tinyurl = tinyurl;
//				_buddy.users.put(key, new BuddyUserInfo(email, id, userName,
//						tinyurl, currentNetworkId, currentNetworkStage,
//						currentNetworkName));
			} catch (Exception e) {
				e.printStackTrace();
				throw new RuntimeException(e);
			}
		}
		return null;
	}
}
