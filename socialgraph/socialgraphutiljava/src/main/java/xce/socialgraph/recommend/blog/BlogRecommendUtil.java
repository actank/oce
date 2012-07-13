/*
 * Function: Get user's recommended blog data 
 * Author:   XiongJunWu
 * Email:    junwu.xiong@renren-inc.com
 * Date :	 2011-11-01
 * */

package xce.socialgraph.recommend.blog;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringEscapeUtils;

import com.xiaonei.xce.XceAdapter;

public class BlogRecommendUtil {
	private static BlogRecommendUtil instance = null;
	private static String DbInstance = "socialgraph_recommend_blog";
	
	private BlogRecommendUtil() {
	}
	
	public static BlogRecommendUtil getInstance() {
		if(null == instance) {
			synchronized (BlogRecommendUtil.class) {
				if(null == instance) {
					try {
						instance = new BlogRecommendUtil();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}					
				}				
			}			
		}
		return instance;
	}
	
	/* Fetch blog recommended list
	 * @param user_id 
	 * @param limit_number
	 * */
	public List<BlogRecommendData> getBlogRecommend(int user_id, int limit_number) {
	        user_id = 0;
		List<BlogRecommendData> blog_recommend_list = new ArrayList<BlogRecommendData>();
		Connection blog_conn = null;
		Statement st = null;
		ResultSet result = null;
		try {
			
			blog_conn = XceAdapter.getInstance().getReadConnection(DbInstance);
			st = blog_conn.createStatement();
			String sql = "";
			if (limit_number <= 0) {
				sql = "SELECT * FROM recommend_by_blog WHERE user_id = " + user_id + " LIMIT " + 100;
			} else {
				sql = "SELECT * FROM recommend_by_blog WHERE user_id = " + user_id + " LIMIT " + limit_number;
			}
			result = st.executeQuery(sql);	
			if (null == result) {
				return blog_recommend_list;
			}
			
			while (result.next()) {
				
				BlogRecommendData blog_data = new BlogRecommendData();
				
				blog_data.setUserId(result.getInt("user_id"));
				blog_data.setBlogId(result.getLong("blog_id"));
				blog_data.setShareId(result.getLong("share_id"));
				blog_data.setShareUserId(result.getInt("share_user_id"));
//				blog_data.setStage(result.getInt("stage"));
//				blog_data.setType(result.getInt("type"));
				blog_data.setTitle(result.getString("title"));
				blog_data.setUrl(result.getString("url"));
				blog_data.setSummary(StringEscapeUtils.unescapeHtml( result.getString("Summary") ));
				blog_data.setUrlMd5(result.getString("url_md5"));
				blog_data.setShareUserName(result.getString("share_user_name"));
				blog_data.setShareCount(result.getInt("Share_count"));				
				
				blog_recommend_list.add(blog_data);
			}			
		} catch (Exception e) {
			e.printStackTrace();			
		} finally {
			try {
				if(null != blog_conn) { 
					blog_conn.close();			
				}				
				if(null != st) {
					st.close();
				}
				if(null != result) {
					result.close();
				}
			} catch (SQLException e) {
				e.printStackTrace();			
			}
		}
		
		return blog_recommend_list;
	}
	
	public static void main(String[] args) {
		System.out.println("blog test");
 		int user_id = 5;
 		List<BlogRecommendData> result = BlogRecommendUtil.getInstance().getBlogRecommend(user_id,30);
 		for (BlogRecommendData blog_data : result) {
 			System.out.println("user_id: " + blog_data.getUserId() + "\n" +
 					   "blog_id: " + blog_data.getBlogId() + "\n" +
 					   "share_id: " + blog_data.getShareId() + "\n" +
 					   "share_user_id: " + blog_data.getShareUserId() + "\n" +
// 					   "stage: " + blog_data.getStage() + "\n" +
//   					   "type: " + blog_data.getType() + "\n" +
 					   "title: " + blog_data.getTitle() + "\n" +
    					   "url: " + blog_data.getUrl() + "\n" +
   					   "summary: " + blog_data.getSummary() + "\n" +
 					   "url_md5: " + blog_data.getUrlMd5() + "\n" +
 					   "share_user_name: " + blog_data.getShareUserName() + "\n" +
 					   "share_count: " + blog_data.getShareCount()  + "\n");
 		}
		
    	System.out.println("result size: " + result.size());
		System.exit(0);
	}
	
}
