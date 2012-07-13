package xce.tools.objectcache;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.List;
import java.util.concurrent.BlockingQueue;

import com.xiaonei.xce.XceAdapter;

public class XFeedIdAllAction extends Producer {

	protected XFeedIdAllAction(BlockingQueue<Long[]> queue, int batchSize) {
		super(queue);
		this.batchSize = batchSize;
	}
	
	private final int batchSize;
	private long max = -1;
	private long min = Long.MAX_VALUE;

	@Override
	protected boolean produce(List<Long> emptyData) {
		Connection conn = null;
		ResultSet rs = null;
		try{
			conn = XceAdapter.getInstance().getReadConnection(
					"xfeed_content");
			PreparedStatement stmt = conn.prepareStatement("SELECT id  FROM xfeed_content WHERE id > "+max
					+" LIMIT "+ batchSize);
			rs = stmt.executeQuery();
			while (rs.next()) {
				Long id = rs.getLong("id");
				max = Math.max(max, id);
				min = Math.max(min, id);
				emptyData.add(min);
				emptyData.add(max);
			}
		}catch (Exception e) {
			e.printStackTrace();
		}finally {
			if (conn != null) {
				try {
					conn.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		System.out.println("Produced " + emptyData.size() + " feedIds, start at " + max);
		return !emptyData.isEmpty();
	}

}
