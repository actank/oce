package com.renren.xce.socialgraph;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.BitSet;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import com.xiaonei.xce.XceAdapter;

import xce.socialgraph.util.Clogging;

/**
 * Create table used for stats of user's action of applying
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class UserApplyStats {
	private static int bitsetSize = 31;		//set 31 as size of bitset
	/**
	 * Get login ids from file
	 * @param filePath
	 * @return
	 */
	public static Map<Integer, BitSet> getIdListFromFile(String filePath) {
		Map<Integer, BitSet> id2BitSet = new HashMap<Integer, BitSet>();
		
		File file = new File(filePath);
		if (!file.exists()) {
			Clogging.Error(filePath + " doesn't exist!");
			return id2BitSet;
		}
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			String line = reader.readLine();
			while (null != line) {
				int id = Integer.valueOf(line);
				BitSet item = new BitSet(bitsetSize);
				id2BitSet.put(id, item);
				line = reader.readLine();
			}
			reader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
		return id2BitSet;
	}
	
	/**
	 * Add apply day in id2BitSet
	 * @param id2BitSet
	 * @param addFriends
	 * @param dataNum
	 */
	public static void insertUserApplyDay(Map<Integer, BitSet> id2BitSet,
			String addFriends, int dateNum) {
		File file = new File(addFriends);
		if (!file.exists()) {
			Clogging.Error(addFriends + " doesn't exist!");
			return;
		}
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			String line = reader.readLine();
			while (null != line) {
				String item[] = line.split("\t");		//401974127       384049881       0
				if (item.length == 3) {
					int id = Integer.valueOf(item[0]);
					if (id2BitSet.containsKey(id)) {
						id2BitSet.get(id).set(dateNum - 1);
					}
				}
				line = reader.readLine();
			}
			reader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	
	/**
	 * Insert data of applying into db
	 * @param id2BitSet
	 */
	public static void insertIntoDb(Map<Integer, BitSet> id2BitSet) {
		String time = getCurrentTime();
		final String dbSourceName = "socialgraph_recommend_apply";
		String tableName = "user_apply_" + time;
		Clogging.Debug("tableName:" + tableName);
		
		Connection applyConn = null;
		Statement stmt = null;
		
		try {
			applyConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = applyConn.createStatement();
			for (Entry<Integer, BitSet> item : id2BitSet.entrySet()) {
				int userId = item.getKey();
				String bitValue = "";
				for (int i = bitsetSize - 1; i >= 0; --i) {		//order from right to left, first day of month is located in right side
					if (item.getValue().get(i)) {
						bitValue += '1';
					} else {
						bitValue += '0';
					}
				}
				int decValue = Integer.parseInt(bitValue, 2);
				Clogging.Debug("userId:" + userId + " bitValue:" + bitValue + "  decValue:" + decValue);
				
				String sql = "INSERT INTO "
						+ tableName
						+ " (userid, apply_days) VALUES ("
						+ userId
						+ ", "
						+ decValue
						+ ") ON DUPLICATE KEY UPdATE apply_days=apply_days|" + decValue;
				stmt.execute(sql);
			}
		} catch (SQLException e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != applyConn)			//clean
					applyConn.close();			
				if (null != stmt)
					stmt.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * Create User Apply Map
	 * @param loginFile
	 * @param addFriends
	 */
	public static void createUserApplyMap(String loginFile, String addFriends) {
		Clogging.Debug("createUserApplyMap loginFile:" + loginFile + " addFriends:" + addFriends);
		int strLength = addFriends.length();
		String date = addFriends.substring(strLength - 2, strLength);		//get data
		Clogging.Debug("date : " + date);			
		int dateNum = 0;
		try {
			dateNum = Integer.valueOf(date);		//exit if get error number
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(0);
		}
		
		Map<Integer, BitSet> id2BitSet = getIdListFromFile(loginFile);		//create raw bitset map from login file
		Clogging.Debug("id2BitSet size:" + id2BitSet.size());
		insertUserApplyDay(id2BitSet, addFriends, dateNum);			//insert user apply day into bitset map
		
		insertIntoDb(id2BitSet);		//insert into database
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
	
	public static void main(String[] args) {
		Clogging.InitLogger("UserApplyStats");
		if (args.length != 2) {
			Clogging.Error("Error argument!");
			System.exit(0);
		}
		UserApplyStats.createUserApplyMap(args[0], args[1]);
		System.exit(0);
	}
}
