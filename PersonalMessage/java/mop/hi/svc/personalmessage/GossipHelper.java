package mop.hi.svc.personalmessage;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Timestamp;

import com.xiaonei.xce.XceAdapter;

public class GossipHelper {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(GossipHelper.class);
	private static GossipHelper _instance = new GossipHelper();

	public static GossipHelper getInstance() {
		return _instance;
	}

	public GossipHelper() {

	}

	private static final String[] saveGossip = new String[100];
	private static final String[] saveGossipBody = new String[100];

	static {
		for (int initLeaveword = 0; initLeaveword < 100; ++initLeaveword) {
			saveGossip[initLeaveword] = "INSERT INTO gossip_recent_" + initLeaveword
					+ " (id, owner, name, sender, tinyurl, univ_name) VALUES (?, ?, ?, ?, ?, ?)";
			saveGossipBody[initLeaveword] = "INSERT INTO gossip_body_recent_" + initLeaveword
					+ " (id, time, body) VALUES (?, ?, ?)";
		}
	}

	public int sendGossip(Gossip word) {
		if (_logger.isInfoEnabled()) {
			_logger.info("sendGossip: senderName:" + word.senderName + " senderUniv:" + word.senderUniv + " owner:"
					+ word.owner + " body:" + word.body);
		}
		int result = IdSequenceManager.getInstance().getNext("gossip_id_seq");
		if (result <= 0) {
			return result;
		}

		Connection conn = null;
		PreparedStatement stmt = null;
		try {
			conn = XceAdapter.getInstance().getWriteConnection("gossip", "gossip_" + String.valueOf(word.owner % 100));
			stmt = conn.prepareStatement(saveGossip[word.owner % 100]);
			stmt.setInt(1, result);
			stmt.setInt(2, word.owner);
			stmt.setString(3, word.senderName);
			stmt.setInt(4, word.sender);
			stmt.setString(5, word.tinyurl);
			stmt.setString(6, Id2NameCache.getInstance().get(word.senderUniv));
			stmt.execute();

			stmt = conn.prepareStatement(saveGossipBody[word.owner % 100]);
			stmt.setInt(1, result);
			stmt.setTimestamp(2, new Timestamp(1000L * word.time));
			stmt.setString(3, word.body);
			stmt.execute();
			result = 1;
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
		return result;
	}
}
