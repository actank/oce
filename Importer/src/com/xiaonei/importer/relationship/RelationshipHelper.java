package com.xiaonei.importer.relationship;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.ResultSetHandler;

import com.xiaonei.svc.importer.BuddyInfo;
import com.xiaonei.svc.importer.BuddyList;
import com.xiaonei.svc.importer.BuddyUserInfo;
import com.xiaonei.xce.XceAdapter;

public class RelationshipHelper {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(RelationshipHelper.class);

	private static RelationshipHelper _instance = new RelationshipHelper();

	public static RelationshipHelper getInstance() {
		return _instance;
	}

	public RelationshipHelper() {

	}

	private Map<Integer, String> getUserMSNIds(BuddyList buddy) {
		StringBuffer parmMsn = new StringBuffer();
		for (int i = 0; i < buddy.buddies.length; ++i) {
			if (i != 0) {
				parmMsn.append(", ");
			}
			parmMsn.append("'").append(buddy.buddies[i].email).append("'");
		}

		String sqlInfo = "select userid, email from user_msn where email IN ("
				+ parmMsn.toString() + ")";

		QueryRunner runRelation = new QueryRunner();
		Connection connRelation = null;
		Map<Integer, String> result = null;
		try {
			connRelation = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_USER_MSN);
			result = (Map<Integer, String>) runRelation.query(connRelation, sqlInfo,
					new GetMsnListHandler());
		} catch (SQLException e) {
			_logger.warn("Get from relation error", e);
		} finally {
			try {
				connRelation.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return result;
	}

	private void fillUserMsnInfo(BuddyList buddy) {
		Map<Integer, String> id2msnMap = getUserMSNIds(buddy);
		if (id2msnMap == null || id2msnMap.size() == 0) {
			return;
		}
		StringBuffer parmUserid = new StringBuffer();
		for (int id : id2msnMap.keySet()) {
			if (parmUserid.length() != 0) {
				parmUserid.append(", ");
			}
			parmUserid.append("'").append(id).append("'");
		}

		// 读取email
//		String sqlUserInfoUserid = "select users.id, users.email, users.name, "
//				+ "user_basic.tinyurl, network_history.network_id, network_history.stage, "
//				+ "network_history.network_name FROM users, user_basic, network_history "
//				+ "WHERE users.id=user_basic.id AND network_history.userid=users.id "
//				+ "AND network_history.status=-1 AND users.id IN ("
//				+ parmUserid.toString() + ")";		
		String sqlusers = "select id, account FROM user_passport WHERE id IN ("
			+ parmUserid.toString() + ")";
		_logger.debug(sqlusers);

		QueryRunner runUser = new QueryRunner();
		ResultSetHandler handler = new FillUserMailHandler(buddy, id2msnMap);
		Connection connCommon = null;
		Map<Integer, String> id2key = null;
		try {
			connCommon = XceAdapter.getInstance().getReadConnection(
					"user_passport");
			id2key = (Map<Integer, String>)runUser.query(connCommon, sqlusers, handler);
		} catch (SQLException e) {
			_logger.warn("Get from common error.", e);
		} finally {
			try {
				connCommon.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		// 读取用户名		
		String sqlusername = "select id, name FROM user_names WHERE id IN ("
			+ parmUserid.toString() + ")";
		_logger.debug(sqlusername);

		handler = new FillUserNameHandler(buddy, id2key);
		try {
			connCommon = XceAdapter.getInstance().getReadConnection(
					"user_names");
			runUser.query(connCommon, sqlusername, handler);
		} catch (SQLException e) {
			_logger.warn("Get from common sqlexception.", e);
		} catch (Exception e) {
			_logger.warn("Get from common exception.", e);
		} finally {
			try {
				connCommon.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
//		String sqlUserInfo = "select users.id, users.email, users.name, user_basic.tinyurl, network_history.network_id, network_history.stage, network_history.network_name FROM users, user_basic, network_history WHERE users.id=user_basic.id AND network_history.userid=users.id AND network_history.status=-1 "
//		+" AND users.email IN ("
//			+ parm.toString() + ")";
		String sqluserbasic = "select id, tinyurl FROM user_basic WHERE id IN ("
				+ parmUserid.toString() + ")";
		_logger.debug(sqluserbasic);

		handler = new FillUserBaseHandler(buddy, id2key);
		try {
			connCommon = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_COMMON);
			runUser.query(connCommon, sqluserbasic, handler);
		} catch (SQLException e) {
			_logger.warn("Get from common sqlexception.", e);
		} catch (Exception e) {
			_logger.warn("Get from common exception.", e);
		} finally {
			try {
				connCommon.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
//		String sqlUserInfo = "select users.id, users.email, users.name, user_basic.tinyurl, network_history.network_id, network_history.stage, network_history.network_name FROM users, user_basic, network_history WHERE users.id=user_basic.id AND network_history.userid=users.id AND network_history.status=-1 "
//		+" AND users.email IN ("
//			+ parm.toString() + ")";
		String sqlusernetwork = "select userid, network_id, stage, network_name FROM network_history WHERE userid IN ("
				+ parmUserid.toString() + ")";
		_logger.debug(sqlusernetwork);

		handler = new FillUserNetworkHandler(buddy, id2key);
		try {
			connCommon = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_COMMON);
			runUser.query(connCommon, sqlusernetwork, handler);
		} catch (SQLException e) {
			_logger.warn("Get from common sqlexception.", e);
		} catch (Exception e) {
			_logger.warn("Get from common exception.", e);
		} finally {
			try {
				connCommon.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	}

	public void fillRelationship(BuddyList buddy) {
		if (buddy.users == null) {
			buddy.users = new HashMap<String, BuddyUserInfo>();
		}
		if (!(buddy.buddies.length > 0))
			return;
		
		 try {
			fillUserMsnInfo(buddy);
		} catch (Exception e) {
			_logger.warn("user_msn error", e);
		}
		 
		// get contacts's user info
		_logger.debug("Get from common");
		StringBuffer parm = new StringBuffer();
		for (int i = 0; i < buddy.buddies.length; ++i) {
			if (i != 0) {
				parm.append(", ");
			}
			parm.append("'").append(buddy.buddies[i].email).append("'");
		}
//		String sqlUserInfo = "select users.id, users.email, users.name, user_basic.tinyurl, network_history.network_id, network_history.stage, network_history.network_name FROM users, user_basic, network_history WHERE users.id=user_basic.id AND network_history.userid=users.id AND network_history.status=-1 "
//			+" AND users.email IN ("
//				+ parm.toString() + ")";
		String sqlusers = "select id, account FROM user_passport WHERE account IN ("
				+ parm.toString() + ")";
		_logger.debug(sqlusers);

		QueryRunner runUser = new QueryRunner();
		ResultSetHandler handler = new FillUserMailHandler(buddy);
		Connection connCommon = null;
		Map<Integer, String> id2key = null;
		try {
			connCommon = XceAdapter.getInstance().getReadConnection(
					"user_passport");
			id2key = (Map<Integer, String>)runUser.query(connCommon, sqlusers, handler);
		} catch (SQLException e) {
			_logger.warn("Get from common sqlexception.", e);
		} catch (Exception e) {
			_logger.warn("Get from common exception.", e);
		} finally {
			try {
				connCommon.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}		
		parm.setLength(0);
		for(Integer id : id2key.keySet()) {
			if(parm.length() != 0)
				parm.append(", ");
			parm.append(id);
		}
		
		if (parm.length() > 0) {
			
			// 读取用户名		
			String sqlusername = "select id, name FROM user_names WHERE id IN ("
				+ parm.toString() + ")";
			_logger.debug(sqlusername);
			handler = new FillUserNameHandler(buddy, id2key);
			try {
				connCommon = XceAdapter.getInstance().getReadConnection(
						"user_names");
				runUser.query(connCommon, sqlusername, handler);
			} catch (SQLException e) {
				_logger.warn("Get from common sqlexception.", e);
			} catch (Exception e) {
				_logger.warn("Get from common exception.", e);
			} finally {
				try {
					connCommon.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			
			// String sqlUserInfo = "select users.id, users.email, users.name,
			// user_basic.tinyurl, network_history.network_id,
			// network_history.stage, network_history.network_name FROM users,
			// user_basic, network_history WHERE users.id=user_basic.id AND
			// network_history.userid=users.id AND network_history.status=-1 "
			// +" AND users.email IN ("
			// + parm.toString() + ")";
			String sqluserbasic = "select id, tinyurl FROM user_basic WHERE id IN ("
					+ parm.toString() + ")";
			_logger.debug(sqluserbasic);

			handler = new FillUserBaseHandler(buddy, id2key);
			try {
				connCommon = XceAdapter.getInstance().getReadConnection(
						XceAdapter.DB_COMMON);
				runUser.query(connCommon, sqluserbasic, handler);
			} catch (SQLException e) {
				_logger.warn("Get from common sqlexception.", e);
			} catch (Exception e) {
				_logger.warn("Get from common exception.", e);
			} finally {
				try {
					connCommon.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}

			// String sqlUserInfo = "select users.id, users.email, users.name,
			// user_basic.tinyurl, network_history.network_id,
			// network_history.stage, network_history.network_name FROM users,
			// user_basic, network_history WHERE users.id=user_basic.id AND
			// network_history.userid=users.id AND network_history.status=-1 "
			// +" AND users.email IN ("
			// + parm.toString() + ")";
			String sqlusernetwork = "select userid, network_id, stage, network_name FROM network_history WHERE userid IN ("
					+ parm.toString() + ")";
			_logger.debug(sqlusernetwork);

			handler = new FillUserNetworkHandler(buddy, id2key);
			try {
				connCommon = XceAdapter.getInstance().getReadConnection(
						XceAdapter.DB_COMMON);
				runUser.query(connCommon, sqlusernetwork, handler);
			} catch (SQLException e) {
				_logger.warn("Get from common sqlexception.", e);
			} catch (Exception e) {
				_logger.warn("Get from common exception.", e);
			} finally {
				try {
					connCommon.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}


		// get owner's friends
		_logger.debug("Get from relation");
		int ownerTable = buddy.owner.userId % 100;
		String sqlRelation = "SELECT guest FROM relation_" + ownerTable
				+ " WHERE host=" + buddy.owner.userId;
		_logger.debug(sqlRelation);

		QueryRunner runRelation = new QueryRunner();
		Connection connRelation = null;
		try {
			connRelation = XceAdapter.getInstance().getReadConnection(
					"relation", "relation_" + ownerTable);
			List<Integer> result = (List<Integer>) runRelation.query(
					connRelation, sqlRelation, new GetFriendListHandler());
			for (int i = 0; i < buddy.buddies.length; ++i) {
				BuddyInfo info = buddy.buddies[i];
//				if (!buddy.users.containsKey(info.email)) {
//					continue;
//				}
				BuddyUserInfo userinfo = (BuddyUserInfo) buddy.users
						.get(info.email);
				if (userinfo != null && result.contains(userinfo.userId)) {
					info.status.code = 4;
					info.status.message = "FRIEND";
				}
			}
		} catch (SQLException e) {
			_logger.warn("Get from relation sqlexception", e);
		} catch (Exception e) {
			_logger.warn("Get from relation exception", e);
		} finally {
			try {
				connRelation.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		// done
	}
	
}
