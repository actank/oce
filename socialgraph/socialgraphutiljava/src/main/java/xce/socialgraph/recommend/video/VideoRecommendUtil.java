/*
 * Function: Get user's recommended video data 
 * Author:   XiongJunWu
 * Email:    junwu.xiong@renren-inc.com
 * Date :	 2011-11-02
 * */

package xce.socialgraph.recommend.video;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringEscapeUtils;

import com.xiaonei.xce.XceAdapter;

public class VideoRecommendUtil {
	private static VideoRecommendUtil instance = null;
	private static String DbInstance = "socialgraph_recommend_video";
	
	private VideoRecommendUtil() {
	}
	
	public static VideoRecommendUtil getInstance() {
		if(null == instance) {
			synchronized (VideoRecommendUtil.class) {
				if(null == instance) {
					try {
						instance = new VideoRecommendUtil();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}					
				}				
			}			
		}
		return instance;
	}
	
	/* Fetch video recommended list
	 * @param user_id 
	 * @param limit_number
	 * */
	public List<VideoRecommendData> getVideoRecommend(int user_id, int limit_number) {
	        user_id = 0;
		List<VideoRecommendData> video_recommend_list = new ArrayList<VideoRecommendData>();
		Connection video_conn = null;
		Statement st = null;
		ResultSet result = null;
		try {
			
			video_conn = XceAdapter.getInstance().getReadConnection(DbInstance);
			st = video_conn.createStatement();
			String sql = "";
			if (limit_number <= 0) {
				sql = "SELECT * FROM recommend_by_video WHERE user_id = " + user_id + " LIMIT " + 100;
			} else {
				sql = "SELECT * FROM recommend_by_video WHERE user_id = " + user_id + " LIMIT " + limit_number;
			}
			result = st.executeQuery(sql);
			if (null == result) {
				return video_recommend_list;
			}
			
			while (result.next()) {
				
				VideoRecommendData video_data = new VideoRecommendData();
				
				video_data.setUserId(result.getInt("user_id"));
				video_data.setVideoId(result.getLong("video_id"));
				video_data.setShareId(result.getLong("share_id"));
				video_data.setShareUserId(result.getInt("share_user_id"));
//				video_data.setStage(result.getInt("stage"));
//				video_data.setType(result.getInt("type"));
				video_data.setTitle(result.getString("title"));
				video_data.setUrl(result.getString("url"));
//				video_data.setSummary(result.getString("Summary"));
				video_data.setSummary(StringEscapeUtils.unescapeHtml( result.getString("Summary") ));
				video_data.setUrlMd5(result.getString("url_md5"));
				video_data.setShareUserName(result.getString("share_user_name"));
				video_data.setThumbUrl(result.getString("thumb_url"));
				video_data.setShareCount(result.getInt("Share_count"));				
				
				video_recommend_list.add(video_data);
			}
		} catch (Exception e) {
			e.printStackTrace();			
		} finally {
			try {
				if(null != video_conn) { 
					video_conn.close();			
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
		
		return video_recommend_list;
	}
	
	public static void main(String[] args) {
		System.out.println("video test");
 		int user_id = 99;
 		List<VideoRecommendData> result = VideoRecommendUtil.getInstance().getVideoRecommend(user_id,30);
 		for (VideoRecommendData video_data : result) {
 			System.out.println("user_id: " + video_data.getUserId() + "\n" +
 							   "video_id: " + video_data.getVideoId() + "\n" +
 							   "share_id: " + video_data.getShareId() + "\n" +
 							   "share_user_id: " + video_data.getShareUserId() + "\n" +
// 							   "stage: " + video_data.getStage() + "\n" +
//   						   "type: " + video_data.getType() + "\n" +
 							   "title: " + video_data.getTitle() + "\n" +
    						   "url: " + video_data.getUrl() + "\n" +
//   						   "summary: " + video_data.getSummary() + "\n" +
 							   "url_md5: " + video_data.getUrlMd5() + "\n" +
 							   "share_user_name: " + video_data.getShareUserName() + "\n" +
 							   "thumb_url: " + video_data.getThumbUrl() + "\n" + 
 							   "share_count: " + video_data.getShareCount()  + "\n");
 		}
		
    	System.out.println("result size: " + result.size());
		System.exit(0);
	}
	
}
