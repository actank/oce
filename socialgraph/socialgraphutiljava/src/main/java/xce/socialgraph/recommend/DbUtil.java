package xce.socialgraph.recommend;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.XceAdapter;

/**
 * 获取sg_block表中的block_id
 * @author zhangnan
 *
 */
public class DbUtil {
	private static DbUtil instance = null;
	private static String dbSourceName = "socialgraph_block";
	private static String sgBlockTableName = "sg_block";
	private static String defaultBiz = "NON";
	
	private DbUtil() {
	}
	
	public static DbUtil getInstance() {
		if (null == instance) {
			synchronized (DbUtil.class) {
				try {
					instance = new DbUtil();
				} catch (Exception e) {
					e.printStackTrace();
					instance = null;
				}
			}
		}
		return instance;
	}
	
	/**
	 * 获取指定类型的block_id
	 * @param host 用户id
	 * @param biz 业务id
	 * @return
	 */
	public List<Long> getSgBlockListByBiz(int host, String biz) {
		List<Long> blockList = new ArrayList<Long>();
		Connection sgBlockConn = null;
		Statement stmt = null;
		ResultSet result = null;
		
		try {
			sgBlockConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = sgBlockConn.createStatement();
			String sql = "";
			if (null == biz) {
				sql = "SELECT block_id FROM " + sgBlockTableName
						+ " WHERE source_id = " + host + " AND business = \"" + defaultBiz + "\"";
			} else {
				sql = "SELECT block_id FROM " + sgBlockTableName
						+ " WHERE source_id = " + host + " AND business = \"" + biz + "\"";
			}
			result = stmt.executeQuery(sql);
			if (null == result) {
				return blockList;
			}
			
			while (result.next()) {
				blockList.add(result.getLong("block_id"));
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != sgBlockConn)		//clean
					sgBlockConn.close();
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}
		return blockList;
	}
	
	/**
	 * 获取指定用户的全部block数据
	 * @param host 用户id
	 * @return
	 */
	public List<Long> getSgBlockList(int host) {
		List<Long> blockList = new ArrayList<Long>();
		Connection sgBlockConn = null;
		Statement stmt = null;
		ResultSet result = null;
		
		try {
			sgBlockConn = XceAdapter.getInstance().getReadConnection(dbSourceName);
			stmt = sgBlockConn.createStatement();
			String sql = "SELECT block_id FROM " + sgBlockTableName
					+ " WHERE source_id = " + host;
		
			result = stmt.executeQuery(sql);
			if (null == result) {
				return blockList;
			}
			
			while (result.next()) {
				blockList.add(result.getLong("block_id"));
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if (null != sgBlockConn)		//clean
					sgBlockConn.close();
				if (null != stmt)
					stmt.close();
				if (null != result)
					result.close();
			} catch (Exception e2) {
				e2.printStackTrace();
			}
		}
		return blockList;
	}
	
//	public static void main(String[] args) {
//		Long startTime = System.currentTimeMillis();
//		int host = 2;
//		List<Long> idList = DbUtil.getInstance().getSgBlockList(host);
//		for (Long id : idList) {
//			System.out.println("id:" + id);
//		}
//		Long endTime = System.currentTimeMillis();
//		System.out.println("total time:" + (endTime - startTime) + "毫秒");
//		
////		List<Long> blockList = DbUtil.getInstance().getSgBlockListByBiz(1000, "FFW");
////		for (Long id : blockList) {
////			System.out.println("bid:" + id);
////		}
//		System.exit(0);
//	}
}
