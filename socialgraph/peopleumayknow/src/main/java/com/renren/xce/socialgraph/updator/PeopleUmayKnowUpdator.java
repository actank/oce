package com.renren.xce.socialgraph.updator;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.xiaonei.xce.XceAdapter;

import xce.socialgraph.util.Clogging;

/**
 * 推荐数据更新类
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class PeopleUmayKnowUpdator {
	/**
	 * 从文件中获取id列表
	 * @param filePath
	 * @return
	 */
	public static List<Integer> getIdListFromFile(String filePath) {
		List<Integer> idList = new ArrayList<Integer>();
		
		File file = new File(filePath);
		if (!file.exists()) {
			Clogging.Error(filePath + " doesn't exist!");
			return idList;
		}
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			String line = reader.readLine();
			while (null != line) {
				int id = Integer.valueOf(line);
				idList.add(id);
				line = reader.readLine();
			}
			reader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
		return idList;
	}
	
	/**
	 * 获取昨天登录用户的id列表
	 * @return
	 */
	public static List<Integer> getIdListFromLogin() {
		List<Integer> idList = new ArrayList<Integer>();
		
		Calendar cal = Calendar.getInstance();
		cal.set(Calendar.HOUR_OF_DAY, 0);
		cal.set(Calendar.MINUTE, 0);
		cal.set(Calendar.SECOND, 0);
		cal.set(Calendar.MILLISECOND, 0);
		long millisTime = cal.getTimeInMillis() - 24 * 3600 * 1000; // 昨天的时间秒数	
		
		SimpleDateFormat formater = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		int interval = 10 * 1000;				//以10秒为时间间隔
		for (int i = 10 * 1000; i < 24 * 3600 * 1000; i = i + interval) {
			Date startTime = new Date();
			startTime.setTime(millisTime);
			String startTimeStr = formater.format(startTime);
			
			millisTime += interval;
			Date endTime = new Date();
			endTime.setTime(millisTime);
			String endTimeStr = formater.format(endTime);
			try {
				Connection userTimeConn = XceAdapter.getInstance().getReadConnection("user_time");
				Statement stmt = userTimeConn.createStatement();
				String sql = "SELECT * FROM user_time WHERE lastlogintime BETWEEN \""
						+ startTimeStr + "\" AND \"" + endTimeStr + "\"";
				Clogging.Debug(sql + " idlist_size:" + idList.size());
				ResultSet result = stmt.executeQuery(sql);
				if (result != null) {
					while (result.next()) {
						idList.add(result.getInt("id"));
					}
				}
				userTimeConn.close();
				stmt.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
		return idList;
	}
	
	/**
	 * 创建id列表中用户推荐数据并存储
	 * @param idList id列表
	 */
	public static void createRecommendData(List<Integer> idList) {
		ThreadPoolExecutor exec = new ThreadPoolExecutor(100, 200, 10,
				TimeUnit.SECONDS, new LinkedBlockingDeque<Runnable>());
		
		for (Integer id : idList) {
			exec.execute(new CreateRecommendDataThread(id));
		}
		
		while (exec.getActiveCount() > 0) {
			try {
				Thread.sleep(20000);
				Clogging.Debug("activeCount : " + exec.getActiveCount());
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		try {
			Thread.sleep(40000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		exec.shutdown();
 	}
	
	public static void main(String[] args) {
		if (1 != args.length) {
			Clogging.Error("PeopleUmayKnowUpdator error args!");
			System.exit(0);
		}
		Clogging.InitLogger("PeopleUmayKnowUpdator");
		List<Integer> idList = PeopleUmayKnowUpdator.getIdListFromFile(args[0]);
		Clogging.Debug("PeopleUmayKnowUpdator idList size:" + idList.size());
		if (!idList.isEmpty()) {
			PeopleUmayKnowUpdator.createRecommendData(idList);
		}
		System.exit(0);
	}

}
