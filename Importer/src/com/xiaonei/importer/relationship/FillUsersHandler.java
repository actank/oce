package com.xiaonei.importer.relationship;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.dbutils.ResultSetHandler;

import com.xiaonei.svc.importer.BuddyInfoStatus;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyUserInfo;

public class FillUsersHandler implements ResultSetHandler {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(FillUsersHandler.class);
	private BuddyList _buddy;
	private Map<String, Integer> _key2pos = new HashMap<String, Integer>();

	public FillUsersHandler(BuddyList buddy) {
		_buddy = buddy;
		for (int i = 0; i < _buddy.buddies.length; ++i) {
			_key2pos.put(_buddy.buddies[i].email, i);
		}
	}
	
	private Map<Integer, String> id2msnMap = null;
	public FillUsersHandler(BuddyList buddy, Map<Integer, String> id2msnMap) {
		this(buddy);
		this.id2msnMap = id2msnMap;
	}

	// select users.id, users.email, users.name, user_basic.tinyurl,
	// network_history.network_id, network_history.stage,
	// network_history.network_name FROM users, user_basic, network_history
	// WHERE users.id=user_basic.id AND network_history.userid=users.id AND
	// network_history.status=-1 AND users.email IN
	
	public Object handle(ResultSet rs) throws SQLException {
		Map<Integer, String> id2key = new HashMap<Integer, String>();
		while (rs.next()) {
			_logger.debug("FillUserInfo Handler handle");
			try {
				int id = rs.getInt("ID");
				String email = rs.getString("EMAIL");
				String userName = rs.getString("NAME");
//				String tinyurl = rs.getString("TINYURL");
//				int currentNetworkId = rs.getInt("NETWORK_ID");
//				int currentNetworkStage = rs.getInt("STAGE");
//				String currentNetworkName = rs.getString("NETWORK_NAME");
				if (_logger.isDebugEnabled()) {
					_logger.debug("email = " + email + " id=" + id + " name="
							+ userName);
				}
				
				String key = email;
				if(id2msnMap != null){
					key = id2msnMap.get(id);
				}
				id2key.put(id, key);

				if (_buddy.users.containsKey(key)) {
					_buddy.users.remove(key);
				}
				
				if (id == _buddy.owner.userId)
//					_buddy.buddies[_email2pos.get(key)].status = new BuddyInfoStatus(
//							-1, "SELF");
					continue;
				else
					_buddy.buddies[_key2pos.get(key)].status = new BuddyInfoStatus(
							1, "USER");				
				_buddy.users.put(key, new BuddyUserInfo(email, id, userName,
						null, 0, 0,
						null));
			} catch (Exception e) {
				e.printStackTrace();
				throw new RuntimeException(e);
			}
		}
		return id2key;
	}

}
