package xce.socialgraph.recommend.recommendbyinfonew;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import xce.util.tools.IntSeqSerialize;

import com.xiaonei.xce.XceAdapter;

/**
 * RecommendUtil which include detail of school name 
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class RecommendByInfoNewUtil {
	private static RecommendByInfoNewUtil instance = null;
	private static String dbSourceName = "socialgraph_recommend_info";
	
	private static int UNIV_TYPE = 0;
	private static int HIGHSCHOOL_TYPE = 1;
	private static int JUNIORSCHOOL_TYPE = 2;
	private static int ELEMENTARYSCHOOL_TYPE = 3;
	
	private RecommendByInfoNewUtil() {
	}
	
	public static RecommendByInfoNewUtil getInstance() {
		if (null == instance) {
			synchronized (RecommendByInfoNewUtil.class) {
				try {
					instance = new RecommendByInfoNewUtil();
				} catch (Exception e) {
					e.printStackTrace();
					instance = null;
				}
			}
		}
		return instance;
	}
	
	/**
	 * Get recommendations of University
	 * @param userId
	 * @return
	 */
	public List<InfoRecommendItem> getUnivFriends(int userId) {
		return getFriendsByType(userId, UNIV_TYPE);
	}
	
	/**
	 * Get recommendations of High School
	 * @param userId
	 * @return
	 */
	public List<InfoRecommendItem> getHighSchoolFriends(int userId) {
		return getFriendsByType(userId, HIGHSCHOOL_TYPE);
	}
	
	/**
	 * Get recommendations of Junior School
	 * @param userId
	 * @return
	 */
	public List<InfoRecommendItem> getJuniorSchoolFriends(int userId) {
		return getFriendsByType(userId, JUNIORSCHOOL_TYPE);
	}
	
	/**
	 * Get recommendations of Elementary School
	 * @param userId
	 * @return
	 */
	public List<InfoRecommendItem> getElementarySchoolFriends(int userId) {
		return getFriendsByType(userId, ELEMENTARYSCHOOL_TYPE);
	}
	
	/**
	 * Get recommedations from db according to user's type
	 * @param userId
	 * @param type
	 * @return
	 */
	private List<InfoRecommendItem> getFriendsByType(int userId, int type) {
		List<InfoRecommendItem> itemList = new ArrayList<InfoRecommendItem>();
		String tableName = "recommend_by_info_new";
		Connection recommendByInfoNewConn = null;
		Statement stmt = null;
		ResultSet result = null;
		try {
			recommendByInfoNewConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = recommendByInfoNewConn.createStatement();
			String sql = "SELECT school_name, recommend FROM " + tableName
					+ " WHERE userid = " + userId + " AND stage = " + type;
			result = stmt.executeQuery(sql);
			if (null == result) {
				return itemList;
			}
			while (result.next()) {
				byte[] data = result.getBytes("recommend");
				if (data.length % 2 != 0) {			//handle exception
					break;
				}
				List<Integer> recommendIds = new ArrayList<Integer>();
				recommendIds = IntSeqSerialize.unserialize(data);
				String schoolName = result.getString("school_name");
				for (Integer id : recommendIds) {
					InfoRecommendItem item = new InfoRecommendItem(id, schoolName);
					itemList.add(item);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != recommendByInfoNewConn)		//clean
					recommendByInfoNewConn.close();		
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e2) {
				e2.printStackTrace();
			}
		}
		return itemList;
	}
	
	public static void main(String[] args) {
		System.out.println("RecommendByInfoNewUtil");
		int userId = 200535831;
		List<InfoRecommendItem> itemList = RecommendByInfoNewUtil.getInstance().getUnivFriends(userId);
//		List<InfoRecommendItem> itemList = RecommendByInfoNewUtil.getInstance().getHighSchoolFriends(userId);
//		List<InfoRecommendItem> itemList = RecommendByInfoNewUtil.getInstance().getJuniorSchoolFriends(userId);
//		List<InfoRecommendItem> itemList = RecommendByInfoNewUtil.getInstance().getElementarySchoolFriends(userId);
		System.out.println(itemList.size());
		for (InfoRecommendItem item : itemList) {
			System.out.println(item.getSchoolName() + " : " + item.getUserId());
		}
	}
}
