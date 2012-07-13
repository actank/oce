package xce.tools.objectcache;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.BlockingQueue;

import com.xiaonei.xce.XceAdapter;

public class UserIdAllAction extends Producer {

	protected UserIdAllAction(BlockingQueue<Long[]> queue, int batchSize) {
		super(queue);
		this.batchSize = batchSize;
	}

	private final int batchSize;
	private int max = -1;

	@Override
	protected boolean produce(List<Long> emptyData) {
		Connection conn = null;
		ResultSet rs = null;
		try {
			conn = XceAdapter.getInstance().getReadConnection(
					XceAdapter.DB_USER_COMMON);
			PreparedStatement stmt = conn
					.prepareStatement("SELECT id FROM users WHERE id > " + max
							+ " LIMIT " + batchSize);
			rs = stmt.executeQuery();
			while (rs.next()) {
				Integer id = rs.getInt("id");
				max = Math.max(max, id);
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
		System.out.println("Produced " + emptyData.size() + " userIds, start at " + max);
		return !emptyData.isEmpty();
	}

}
