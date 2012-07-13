package com.renren.xce.socialgraph;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import com.xiaonei.xce.XceAdapter;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

import xce.socialgraph.util.Clogging;

/**
 * Load data of applying from db to cache
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class LoadApplyStats {
//	private static int bitsetSize = 31;		//set 31 as size of bitset
	private static String biz = "USERAPPLY";		//business name
	
	public static void loadUserApplyStats() {
		final String dbSourceName = "socialgraph_recommend_apply";
		String time = getCurrentTime();
		String tableName = "user_apply_" + time;
		Clogging.Debug("tableName:" + tableName);
		int lowBoundary = 0;
		
		Connection applyConn = null;
		Statement stmt = null;
		ResultSet result = null;
		try {
			applyConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = applyConn.createStatement();
			while (true) {
				Clogging.Debug("[loadUserApplyStats] lowBounday : " + lowBoundary);
				String sql = "SELECT userid, apply_days FROM " + tableName
						+ " WHERE userid > " + lowBoundary + " LIMIT 1000";
				result = stmt.executeQuery(sql);
				if (null == result) {
					break;
				}
				while (result.next()) {
					int applyDays = result.getInt("apply_days");
					int userId = result.getInt("userid");
					lowBoundary = userId;
					insertIntoCache(userId, applyDays);			//insert into cache named USERAPPLY
				}
			}
		} catch (SQLException e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != applyConn)			//clean
					applyConn.close();			
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * Insert data into USERAPPLY cache
	 * @param userId
	 * @param applyFrequence
	 * @return
	 */
	private static boolean insertIntoCache(int userId, int applyFrequence) {
		boolean insert = true;
		Items.Builder result = Items.newBuilder();
		Item.Builder item = Item.newBuilder();
		item.setId(userId);		//store userId in Item.id session
		item.setField(applyFrequence);		//store applyFrequence in Item.field session
		result.addItems(item);		//add into result
		byte[] resultData = result.build().toByteArray();
		insert = BusinessCacheAdapter.getInstance().set(biz, userId, resultData);
		
		if (insert) {
			Clogging.Debug("Success " + userId + " " + Integer.toBinaryString(applyFrequence) + " " + resultData.length);
		} else {
			Clogging.Debug("Failed " + userId + " " + Integer.toBinaryString(applyFrequence) + " " + resultData.length);
		}
		
		return insert;
	}
	
	/**
	 * Get postfix of user_appyl table
	 * @return
	 */
	private static String getCurrentTime() {
		String time = "";
		Calendar cal = Calendar.getInstance();
		long millisTime = cal.getTimeInMillis();
		Date currentTime = new Date();
		currentTime.setTime(millisTime);
		
		SimpleDateFormat formater = new SimpleDateFormat("yyyyMM");
		time = formater.format(currentTime);
		return time;
	}
	
//	/**
//	 * For test
//	 * @param result
//	 */
//	private static void printItems(Items.Builder result) {
//	for (Item item : result.getItemsList()) {
//		Clogging.Debug(item.getMessage());
//		Clogging.Debug("" + item.getId());
//		Clogging.Debug("" + item.getField());
//		String messagesList = "";
//		for (String mes : item.getMessagesList()) {
//			messagesList += mes + " ";
//		}
//		Clogging.Debug(messagesList);
//		String fieldsList = "";
//		for (Integer fid : item.getFieldsList()) {
//			fieldsList += fid + " ";
//		}
//		Clogging.Debug(fieldsList);
//		
//		Clogging.Debug("---------------------------------------------------------------------");
//	}
//}
	
	public static void main(String[] args) {
		Clogging.InitLogger("LoadApplyStats");
		LoadApplyStats.loadUserApplyStats();
		System.exit(0);
	}
}
