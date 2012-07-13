package mop.hi.svc.personalmessage;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Timestamp;

import com.xiaonei.xce.XceAdapter;

public class EmailHelper {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(EmailHelper.class);

	private static EmailHelper _instance = new EmailHelper();

	public static EmailHelper getInstance() {
		return _instance;
	}

	public EmailHelper() {

	}

	private static final String saveEmailSql = "INSERT INTO email (email_from, email_to, title, body, addtime) VALUES (?,?,?,?,?)";

	public int sendEmail(Email mail) {
		if (_logger.isInfoEnabled()) {
			_logger.info("sendEmail from:" + mail.from + " subject:" + mail.subject);
		}
		int result = -1;
		Connection conn = null;
		PreparedStatement stmt = null;
		try {
			conn = XceAdapter.getInstance().getWriteConnection("email");
			stmt = conn.prepareStatement(saveEmailSql);
			stmt.setString(1, mail.from);
			stmt.setString(2, mail.to[0]);
			stmt.setString(3, mail.subject);
			stmt.setString(4, mail.body);
			stmt.setTimestamp(5, new Timestamp(System.currentTimeMillis()));
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
