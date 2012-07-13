package com.renren.xce.socialgraph.updator;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import xce.socialgraph.util.Clogging;

import com.xiaonei.xce.XceAdapter;
/**
 * DbUtil class which get user's viewcount from db
 * @author zhangnan
 * @email zhangnan@renren-inc.com
 */
public class GetViewCountUtil {
	private static String dbSourceName = "user_view_count";
	private static Integer MAX_LENGTH = 200;
	
	public static Map<Integer, Integer> getViewCountMap(List<Integer> idList) {
		Map<Integer, Integer> viewCountMap = new HashMap<Integer, Integer>();
		if (null == idList ||			// add justify where idlist is empty
				idList.isEmpty() ||
				idList.size() > MAX_LENGTH) {
			return viewCountMap;
		}
		
		Connection viewcount_conn = null;
		Statement st = null;
		ResultSet result = null;
		try {
			viewcount_conn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			st = viewcount_conn.createStatement();
			String sql = "SELECT id, viewcount FROM view_count WHERE id in (";
			Integer listLength = idList.size();
			
			for (int i = 0; i < listLength - 1; ++i) {
				sql += String.valueOf(idList.get(i)) + ", ";
			}
			sql += String.valueOf(idList.get(listLength - 1)) + ")";
			Clogging.Debug("sql:" + sql);
			result = st.executeQuery(sql);	
			if (null == result) {
				return viewCountMap;
			}
			
			while (result.next()) {
				viewCountMap.put(result.getInt("id"), result.getInt("viewcount"));
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return viewCountMap;
	}
}
