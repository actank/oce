package com.xiaonei.xce.buddyadapter.relation;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import mop.hi.oce.domain.buddy.BuddyRelation;

import com.xiaonei.xce.XceAdapter;

/**
 * RelationBlockByDbAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public final class RelationBlockByDbAdapter {

	private static RelationBlockByDbAdapter _instance = new RelationBlockByDbAdapter();

	public static RelationBlockByDbAdapter getInstance() {
		return _instance;
	}

	/**
	 * @param userId
	 * @param begin
	 * @param limit
	 * @return
	 */
	public List<BuddyRelation> getBlockList(int userId, int begin, int limit) {
		List<BuddyRelation> result = new ArrayList<BuddyRelation>();
		List<BuddyRelation> finalresult = new ArrayList<BuddyRelation>();
		StringBuffer sql = new StringBuffer();
		sql.append("SELECT guest FROM relation_block WHERE host = '")
				.append(userId).append("'");

		Connection c = null;
		try {
			c = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_RELATION_BLOCK);
			Statement s = null;
			try {
				s = c.createStatement();
				ResultSet rs = null;
				try {
					rs = s.executeQuery(sql.toString());
					while (rs.next()) {
						result.add(new BuddyRelation(userId, rs.getInt("guest"),
								BuddyRelation.BuddyDesc.Block));
					}

				} catch (Exception e) {
					e.printStackTrace();
				} finally {
					if (rs != null) {
						rs.close();
					}
					if (s != null) {
						s.close();
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				if (c != null) {
					c.close();
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		for (int i = 0; i < result.size(); i++) {
			if (begin-- > 0) {
				continue;
			}
			if (limit > 0 && (int) result.size() >= limit) {
				break;
			}
			finalresult.add(result.get(i));
		}

		return finalresult;
	}

	/**
	 * @param userId
	 * @param begin
	 * @param limit
	 * @return
	 */
	public List<BuddyRelation> getBlockedList(int userId, int begin, int limit) {
		List<BuddyRelation> result = new ArrayList<BuddyRelation>();
		List<BuddyRelation> finalresult = new ArrayList<BuddyRelation>();
		StringBuffer sql = new StringBuffer();
		sql.append("SELECT host FROM relation_block WHERE guest = '")
				.append(userId).append("'");

		Connection c = null;
		try {
			c = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_RELATION_BLOCK);
			Statement s = null;
			try {
				s = c.createStatement();
				ResultSet rs = null;
				try {
					rs = s.executeQuery(sql.toString());
					while (rs.next()) {
						result.add(new BuddyRelation(userId, rs.getInt("host"),
								BuddyRelation.BuddyDesc.Blocked));
					}

				} catch (Exception e) {
					e.printStackTrace();
				} finally {
					if (rs != null) {
						rs.close();
					}
					if (s != null) {
						s.close();
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				if (c != null) {
					c.close();
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		for (int i = 0; i < result.size(); i++) {
			if (begin-- > 0) {
				continue;
			}
			if (limit > 0 && (int) result.size() >= limit) {
				break;
			}
			finalresult.add(result.get(i));
		}

		return finalresult;
	}

}
