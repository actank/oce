package xce.socialgraph.recommend.commonfriends;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.XceAdapter;
/**
 * 获取二度好友推荐数据java类
 * @author zhangnan
 */
public class CommonFriendsUtil {
	private static CommonFriendsUtil instance = null;
	private static String dbSourceName = "friendfinder_common_friends_new";
	
	/**
	 * 根据散表，初始化100个db连接
	 */
	private CommonFriendsUtil() {
	}
	
	public static CommonFriendsUtil getInstance() {
		if (null == instance) {
			synchronized (CommonFriendsUtil.class) {
				try {
					instance = new CommonFriendsUtil();
				} catch (Exception e) {
					e.printStackTrace();
					instance = null;
				}
			}
		}
		return instance;
	}
	
	/**
	 * 从数据库中获取二度好友数据
	 * @param host 用户id
	 * @param limit 返回的个数，-1时取全部100个共同好友数据
	 * @return
	 */
	public List<CommonFriend> getCommonFriends(int host, int limit) {
		List<CommonFriend> commList = new ArrayList<CommonFriend>();
		Connection commonFriendsConn = null;
		Statement stmt = null;
		ResultSet result = null;
		try {
			String pattern = getPattern("common_friends_", host, 100);
			commonFriendsConn = XceAdapter.getInstance().getReadConnection(dbSourceName, pattern);
			stmt = commonFriendsConn.createStatement();
			String sql = "";
			if (limit <= 0) {
				sql = "SELECT friend_id, weight, shares FROM "
						+ getCommonFriendsTableName(host) + " WHERE owner_id = " + host
						+ " ORDER BY position DESC" + " LIMIT " + 100;
			} else {
				sql = "SELECT friend_id, weight, shares FROM "
					+ getCommonFriendsTableName(host) + " WHERE owner_id = " + host
					+ " ORDER BY position DESC" + " LIMIT " + limit;
			}
			result = stmt.executeQuery(sql);
			if (null == result) {
				return commList;
			}
			while (result.next()) {
				CommonFriend obj = new CommonFriend();
				if (obj.Initialize(result)) {
					commList.add(obj);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != commonFriendsConn) 		//clean
					commonFriendsConn.close();
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
		return commList;
	}

	/**
	 * 向db中插入二度好友数据(未实现)
	 * @param host 用户id
	 * @param commList 二度好友数据
	 */
	public void setCommonFriends(int host, List<CommonFriend> commList) {
		System.out.println("CommonFriendsUtil setCommonFriends is not available.");
	}
	
	/**
	 * 根据散表获取相应的表名
	 * @param host
	 * @return
	 */
	private String getCommonFriendsTableName(int host) {
		return "common_friends_" + (host %100);
	}
	
	/**
	 * 获取pattern
	 * @param prefix表名前缀
	 * @param id 用户id
	 * @param cluster 散的份数
	 * @return
	 */
	private String getPattern(String prefix, long id, int cluster) {
		StringBuffer buff = new StringBuffer();
		buff.append(prefix);
		buff.append(Math.abs(id) % cluster);
		return buff.toString();
	}
	
	public static void main(String[] args) {
		Long startTime = System.currentTimeMillis();
		int host = 238111132;
		List<CommonFriend> result = CommonFriendsUtil.getInstance().getCommonFriends(host, -1);
		Long endTime = System.currentTimeMillis();
		System.out.println("result size:" + result.size());
		System.out.println("CommonFriendsUtil total time:" + (endTime - startTime) + "毫秒");
		System.exit(0);
	}
}
