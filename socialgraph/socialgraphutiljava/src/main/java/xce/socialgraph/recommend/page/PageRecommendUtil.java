package xce.socialgraph.recommend.page;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import xce.util.tools.IntSeqSerialize;

import com.xiaonei.xce.XceAdapter;

/**
 * 获取page推荐数据类
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class PageRecommendUtil {
	private static PageRecommendUtil instance = null;
	private static String dbSourceName = "socialgraph_recommend_page";
	
	private PageRecommendUtil() {
	}

	public static PageRecommendUtil getInstance() {
		if (null == instance) {
			synchronized (PageRecommendUtil.class) {
				if (null == instance) {
					try {
						instance = new PageRecommendUtil();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	/**
	 * 获取推荐page列表
	 * @param hostId 用户id
	 * @param limit 数据个数 <=0时，取100个，其他取真实limit
	 * @return
	 */
	public List<PageRecommendData> getPageRecommend(int hostId, int limit) {
		List<PageRecommendData> pageRecommendList = new ArrayList<PageRecommendData>();
		Connection pageConn = null;
		Statement stmt = null;
		ResultSet result = null;
		try {
			pageConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = pageConn.createStatement();
			String sql = "";
			if (limit <= 0) {
				sql = "SELECT * FROM recommend_by_page WHERE user_id = " + hostId + " LIMIT " + 100;
			} else {
				sql = "SELECT * FROM recommend_by_page WHERE user_id = " + hostId + " LIMIT " + limit;
			}
			result = stmt.executeQuery(sql);
			if (null == result) {
				return pageRecommendList;
			}
			
			while (result.next()) {
				PageRecommendData pageData = new PageRecommendData();
				
				pageData.setUserId(result.getInt("user_id"));
				pageData.setPageId(result.getInt("page_id"));
				pageData.setPageValue(result.getDouble("page_value"));
				pageData.setPageName(result.getString("page_name"));
				pageData.setDescription(result.getString("description"));
				pageData.setFansNum(result.getInt("fans_counter"));
				pageData.setCommonSize(result.getInt("common_friend_number"));
				int commSize = result.getInt("common_friend_number");
				
				List<Integer> tmp = IntSeqSerialize.unserialize(result.getBytes("common_friend_ids"));
				int realSize = commSize > tmp.size() ? tmp.size() : commSize;		//判断共同好友的截断位置
				pageData.setCommonFriends(tmp.subList(0, realSize));
				
				pageRecommendList.add(pageData);
			}
		} catch (SQLException e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != pageConn)			//clean
					pageConn.close();			
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
		
		return pageRecommendList;
	}
	
	public static void main(String[] args) {
		int userId = 1764;
		List<PageRecommendData> result = PageRecommendUtil.getInstance().getPageRecommend(userId, -1);
		for (PageRecommendData data : result) {
			System.out.println(data.getUserId() + " : " + data.getPageId() + " : "
					+ data.getPageName() + " : " + data.getPageValue() + " : "
					+ data.getFansNum() + " : " + data.getDescription() + " : "
					+ data.getCommonSize());
			String detail = "";
			for (Integer ii : data.getCommonFriends()) {
				detail += ii + " ";
			}
			System.out.println(detail);
		}
		System.out.println("result size:" + result.size());
		System.exit(0);
	}
}
