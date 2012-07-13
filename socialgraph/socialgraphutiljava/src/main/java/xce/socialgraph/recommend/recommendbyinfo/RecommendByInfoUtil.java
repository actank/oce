package xce.socialgraph.recommend.recommendbyinfo;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import xce.util.tools.IntSeqSerialize;

import com.xiaonei.xce.XceAdapter;

/**
 * 获取按用户资料推荐数据java类
 * @author zhangnan
 *
 */
public class RecommendByInfoUtil {
	private static RecommendByInfoUtil instance = null;
	private static String dbSourceName = "socialgraph_block";
	
	private static int UNIV_TYPE = 0;
	private static int HIGHSCHOOL_TYPE = 1;
	private static int JUNIORSCHOOL_TYPE = 2;
	private static int ELEMENTARYSCHOOL_TYPE = 3;
	
	private RecommendByInfoUtil() {
	}
	
	public static RecommendByInfoUtil getInstance() {
		if (null == instance) {
			synchronized (RecommendByInfoUtil.class) {
				try {
					instance = new RecommendByInfoUtil();
				} catch (Exception e) {
					e.printStackTrace();
					instance = null;
				}
			}
		}
		return instance;
	}
	
	/**
	 * 获取大学同资料推荐数据
	 * @param userId 用户id
	 * @return
	 */
	public List<Integer> getUnivFriends(int userId) {
		List<Integer> idList = new ArrayList<Integer>();
		idList = getFriendsByType(userId, UNIV_TYPE);			//获取大学同资料推荐用户
		return idList;
	}
	
	/**
	 * 获取高中同资料推荐数据
	 * @param userId 用户id
	 * @return
	 */
	public List<Integer> getHighSchoolFriends(int userId) {
		List<Integer> idList = new ArrayList<Integer>();
		idList = getFriendsByType(userId, HIGHSCHOOL_TYPE);			//获取高中同资料推荐用户
		return idList;
	}
	
	/**
	 * 获取初中同资料推荐数据
	 * @param userId 用户id
	 * @return
	 */
	public List<Integer> getJuniorSchoolFriends(int userId) {
		List<Integer> idList = new ArrayList<Integer>();
		idList = getFriendsByType(userId, JUNIORSCHOOL_TYPE);			//获取初中同资料推荐用户
		return idList;
	}
	
	/**
	 * 获取小学同资料推荐数据
	 * @param userId	用户id
	 * @return
	 */
	public List<Integer> getElementarySchoolFriends(int userId) {
		List<Integer> idList = new ArrayList<Integer>();
		idList = getFriendsByType(userId, ELEMENTARYSCHOOL_TYPE);			//获取小学同资料推荐用户
		return idList;
	}
	
	/**
	 * 取数据库中信息
	 * @param userId 用户id
	 * @param type 所取数据的类型
	 * @return
	 */
	private List<Integer> getFriendsByType(int userId, int type) {
		List<Integer> idList = new ArrayList<Integer>();
		String tableName = "recommend_by_info";
		Connection recommendByInfoConn = null;
		Statement stmt = null;
		ResultSet result = null;
		try {
			recommendByInfoConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = recommendByInfoConn.createStatement();
			String sql = "SELECT recommend FROM " + tableName + " WHERE userid = "
					+ userId + " AND stage = " + type;
			result = stmt.executeQuery(sql);
			if (null == result) {
				return idList;
			}
			while (result.next()) {
				byte[] data = result.getBytes("recommend");
				if (data.length % 2 != 0) {			//handle exception
					break;
				}
				idList = IntSeqSerialize.unserialize(data);
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != recommendByInfoConn)		//clean
					recommendByInfoConn.close();		
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
		return idList;
	}
	
	public static void main(String[] args) {
		System.out.println("RecommendByInfoUtil");
//		int userId = 253917687;
		int userId = 238111132;
//		List<Integer> idList = RecommendByInfoUtil.getInstance().getJuniorShoolFriends(userId);
		List<Integer> idList = RecommendByInfoUtil.getInstance().getUnivFriends(userId);
//		List<Integer> idList = RecommendByInfoUtil.getInstance().getHighSchoolFriends(userId);
//		List<Integer> idList = RecommendByInfoUtil.getInstance().getElementarySchoolFriends(userId);
		System.out.println(idList.size());
		String result = "";
		for (Integer one : idList) {
			result += one + " ";
		}
		System.out.println(result);
		System.exit(0);
	}
}
