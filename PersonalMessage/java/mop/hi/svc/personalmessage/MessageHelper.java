package mop.hi.svc.personalmessage;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Timestamp;

import com.xiaonei.xce.XceAdapter;

public class MessageHelper {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(MessageHelper.class);

	private static MessageHelper _instance = new MessageHelper();

	public static MessageHelper getInstance() {
		return _instance;
	}

	public MessageHelper() {

	}

	private static final String[] saveMessageFrom = new String[100];
	private static final String[] saveMessageFromBody = new String[100];
	private static final String[] saveMessageTo = new String[100];
	private static final String[] saveMessageToBody = new String[100];

	static {
		for (int initMessageSql = 0; initMessageSql < 100; ++initMessageSql) {
			saveMessageFrom[initMessageSql] = "INSERT INTO message_out_" + initMessageSql
					+ " (id, owner, guest, guestname, subject, readflag, time) VALUES (?,?,?,?,?,?,?)";
			saveMessageTo[initMessageSql] = "INSERT INTO message_in_" + initMessageSql
					+ " (id, owner, guest, guestname, subject, readflag, time, msg_type) VALUES (?,?,?,?,?,?,?,?)";
			saveMessageFromBody[initMessageSql] = "INSERT INTO message_out_body_" + initMessageSql
					+ " (id, body) VALUES (?,?)";
			saveMessageToBody[initMessageSql] = "INSERT INTO message_in_body_" + initMessageSql
					+ " (id, body) VALUES (?,?)";
		}
	}

	public int sendMessage(Message msg) {
		if (_logger.isInfoEnabled()) {
			_logger.info("sendMessage: source:" + msg.source + " sourceName:" + msg.sourceName + " target:"
					+ msg.target + " targetName:" + msg.targetName + " subject:" + msg.subject + " body:" + msg.body);
		}
		int result = -1;
		if (msg.saveInSourceOutbox) { // save message_out
			int id = IdSequenceManager.getInstance().getNext("message_out_id_seq");
			if (id <= 0) {
				throw new NullPointerException("Cannot get id from message_out_id_seq.");
			}
			Connection conn = null;
			PreparedStatement stmtSubject = null;
			PreparedStatement stmtBody = null;
			try {
				conn = XceAdapter.getInstance().getWriteConnection("message", "message_" + (msg.source % 100));
				stmtSubject = conn.prepareStatement(saveMessageFrom[msg.source % 100]);
				stmtSubject.setInt(1, id);
				stmtSubject.setInt(2, msg.source);
				stmtSubject.setInt(3, msg.target);
				stmtSubject.setString(4, msg.targetName);
				stmtSubject.setString(5, msg.subject);
				stmtSubject.setInt(6, 0);
				stmtSubject.setTimestamp(7, new Timestamp(1000L * msg.time));
				stmtSubject.execute();

				stmtBody = conn.prepareStatement(saveMessageFromBody[msg.source % 100]);
				stmtBody.setInt(1, id);
				stmtBody.setString(2, msg.body);
				stmtBody.execute();
				result = 1;
			} catch (SQLException e) {
				e.printStackTrace();
				result = 0;
			} finally {
				try {
					stmtSubject.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
				try {
					stmtBody.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
				try {
					conn.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		if (msg.saveInTargetInbox) { // save messge_in
			int id = IdSequenceManager.getInstance().getNext("message_in_id_seq");
			if (id <= 0) {
				throw new NullPointerException("Cannot get id from message_in_id_seq.");
			}
			Connection conn = null;
			PreparedStatement stmtSubject = null;
			PreparedStatement stmtBody = null;
			try {
				conn = XceAdapter.getInstance().getWriteConnection("message", "message_" + (msg.target % 100));
				stmtSubject = conn.prepareStatement(saveMessageTo[msg.target % 100]);
				stmtSubject.setInt(1, id);
				stmtSubject.setInt(2, msg.target);
				stmtSubject.setInt(3, msg.source);
				stmtSubject.setString(4, msg.sourceName);
				stmtSubject.setString(5, msg.subject);
				stmtSubject.setInt(6, 0);
				stmtSubject.setTimestamp(7, new Timestamp(1000L * msg.time));
				stmtSubject.setInt(8, msg.type);
				stmtSubject.execute();

				stmtBody = conn.prepareStatement(saveMessageToBody[msg.target % 100]);
				stmtBody.setInt(1, id);
				stmtBody.setString(2, msg.body);
				stmtBody.execute();
				result = 2;
			} catch (SQLException e) {
				e.printStackTrace();
				result = 0;
			} finally {
				try {
					stmtSubject.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
				try {
					stmtBody.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
				try {
					conn.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		return result;

	}
}
