package xce.socialgraph.recommend;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.XceAdapter;

/**
 * 获取用户好友列表工具类
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class FriendListUtil {
	private static FriendListUtil instance = null;
	private static String dbSourceName = "relation_friendrank";
	
	private FriendListUtil() {
	}
	
	public static FriendListUtil getInstance() {
		if (null == instance) {
			synchronized (FriendListUtil.class) {
				try {
					instance = new FriendListUtil();
				} catch (Exception e) {
					e.printStackTrace();
					instance = null;
				}
			}
		}
		return instance;
	}
	
	/**
	 * 获取好友列表
	 * @param userId 用户id
	 * @param limit 好友列表长度，<0时获取全部好友列表
	 * @return
	 */
	public List<Integer> getFriendList(int userId, int limit) {
		List<Integer> friendList = new ArrayList<Integer>();
		Connection conn = null;
		Statement stmt = null;
		ResultSet result = null;
		
		try {
			String pattern = getPattern("relation_", userId, 100);
			conn = XceAdapter.getInstance().getReadConnection(dbSourceName, pattern);
			stmt = conn.createStatement();
			
			String sql = "";
			if (limit <= 0) {
				sql = "SELECT guest FROM " + getRelationTableName(userId)
						+ " WHERE host = " + userId + " LIMIT " + 10000;		//好友数不能超过10000
			} else {
				sql = "SELECT guest FROM " + getRelationTableName(userId)
						+ " WHERE host = " + userId + " LIMIT " + limit;
			}
			result = stmt.executeQuery(sql);
			if (null == result) {
				return friendList;
			}
			while (result.next()) {
				friendList.add(result.getInt("guest"));
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != conn)		//clean
					conn.close();
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
		return friendList;
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
	
	/**
	 * 根据散表获取相应的表名
	 * @param host
	 * @return
	 */
	private String getRelationTableName(int userId) {
		return "relation_" + (userId % 100);
	}
	
//	public static void main(String[] args) {
//		System.out.println("FriendListUtil");
//		int userId = 238111132;
//		List<Integer> friendList = FriendListUtil.getInstance().getFriendList(userId, -1);
//		System.out.println(friendList.size());
//		String output = "";
//		for (int each : friendList) {
//			output += each + " ";
//		}
//		System.out.println(output);
//		System.exit(0);
//	}
}
