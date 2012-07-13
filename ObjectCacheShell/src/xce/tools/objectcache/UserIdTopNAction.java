package xce.tools.objectcache;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.BlockingQueue;

import com.xiaonei.xce.XceAdapter;

public class UserIdTopNAction extends Producer {

	protected UserIdTopNAction(BlockingQueue<Long[]> queue, int topN) {
		super(queue);
		this.topN = topN;
	}

	private final int topN;

	@Override
	protected boolean produce(List<Long> emptyData) {
		Connection conn = null;
		ResultSet rs = null;
		try {
			conn = XceAdapter.getInstance().getReadConnection(
					"objectcacheshell_userdesc_ab");
			PreparedStatement stmt = conn
					.prepareStatement("SELECT id FROM user_desc_ab ORDER BY lastlogintime LIMIT " + topN);
			rs = stmt.executeQuery();
			while (rs.next()) {
				Integer id = rs.getInt("id");
				emptyData.add((long)id);
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (conn != null) {
				try {
					conn.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		System.out.println("Produced top " + emptyData.size() + " userIds");
		return false;
	}

}
