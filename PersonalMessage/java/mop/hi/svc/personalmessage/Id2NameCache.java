package mop.hi.svc.personalmessage;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.XceAdapter;

public class Id2NameCache {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(Id2NameCache.class);
	private static Id2NameCache _instance = new Id2NameCache();

	public static Id2NameCache getInstance() {
		return _instance;
	}

	public Id2NameCache() {
		_nameCache = new HashMap<Integer, String>();
		initDb();
	}

	private void initDb() {
		_logger.debug("Initializing NameCache");
		String[] sqls = new String[] { "SELECT id, name FROM high_school", "SELECT id, name FROM university",
				"SELECT id, name FROM workplace" };
		for (String sql : sqls) {
			Connection conn = null;
			PreparedStatement stmt = null;
			try {
				conn = XceAdapter.getInstance().getReadConnection(XceAdapter.DB_COMMON);
				stmt = conn.prepareStatement(sql);
				ResultSet rs = stmt.executeQuery();
				while (rs.next()) {
					int key = rs.getInt(1);
					String value = rs.getString(2);
					_nameCache.put(key, value);
				}
			} catch (SQLException e) {
				e.printStackTrace();
			} finally {
				try {
					stmt.close();
					conn.close();
				} catch (SQLException e) {
					e.printStackTrace();
				}
			}
		}
		_logger.debug("Initialized NameCache");
	}

	private Map<Integer, String> _nameCache;

	public String get(String key) {
		if (null == key) {
			return "未知学校";
		}
		String result = null;
		try {
			int id = Integer.valueOf(key);
			if (!_nameCache.containsKey(id)) {
				return "未知名称";
			}
			result = _nameCache.get(id);
		} catch (Exception e) {
			return "未知-" + key;
		}
		return null == result ? "未知" : result;
	}
}
