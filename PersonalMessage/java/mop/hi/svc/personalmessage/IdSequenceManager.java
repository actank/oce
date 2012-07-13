package mop.hi.svc.personalmessage;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import com.xiaonei.xce.XceAdapter;

public class IdSequenceManager {
	private static IdSequenceManager _instance = new IdSequenceManager();

	public static IdSequenceManager getInstance() {
		return _instance;
	}

	public IdSequenceManager() {

	}

	public int getNext(String seq) {
		String sql = "SELECT nextval('" + seq + "')";
		Connection conn = null;
		Statement stmt = null;
		ResultSet rs = null;
		int result = -1;
		try {
			conn = XceAdapter.getInstance().getReadConnection(XceAdapter.DB_IDSEQUENCE);
			stmt = conn.createStatement();
			rs = stmt.executeQuery(sql);
			if (rs.next()) {
				result = rs.getInt(1);
			} else {
				throw new SQLException(seq + " does not return any value.");
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				rs.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
			try {
				stmt.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
			try {
				conn.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return result;
	}
}
