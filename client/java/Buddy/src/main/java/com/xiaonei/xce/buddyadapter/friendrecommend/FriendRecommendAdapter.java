package com.xiaonei.xce.buddyadapter.friendrecommend;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

import com.xiaonei.xce.XceAdapter;

/**
 * 用户主动好友推荐接口
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public class FriendRecommendAdapter {

    private static FriendRecommendAdapter instance = null;

    private static final String DATA_SOURCE_NAME = XceAdapter.DB_GOING_INFO;

    private FriendRecommendAdapter() {

    }

    public static FriendRecommendAdapter getInstance() {
	if (instance == null) {
	    synchronized (FriendRecommendAdapter.class) {
		if (instance == null) {
		    try {
			instance = new FriendRecommendAdapter();
		    } catch (Exception e) {
			e.printStackTrace();
		    }
		}
	    }
	}
	return instance;
    }

    /**
     * 删除好友推荐数据
     * 
     * @param user1
     * @param user2
     * @param isOneWay
     *            true 是删除单方向, false 双方向删除
     * @return
     */
    public boolean deleteRecommend(int user1, int user2, boolean isOneWay) {
	try {
	    Connection conn = XceAdapter.getInstance().getWriteConnection(
		    DATA_SOURCE_NAME);
	    String sql = String
		    .format("DELETE FROM recommended_friend WHERE recommended_id = %d and berecommended_id = %d",
			    user1, user2);

	    Statement stmt = conn.createStatement();
	    stmt.executeUpdate(sql);
	    if (!isOneWay) {
		String sql2 = String
			.format("DELETE FROM recommended_friend WHERE recommended_id = %d and berecommended_id = %d",
				user2, user1);

		stmt.executeUpdate(sql2);
	    }
	    conn.close();
	    return true;
	} catch (SQLException e) {
	    e.printStackTrace();
	}
	return false;
    }
}
