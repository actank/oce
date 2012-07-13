package xce.distcache.preloader;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.XceAdapter;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-06-17
 */
public class IdsProducer {

    private static final int BATCHSIZE = 1000;

    private static Long maxId;

    private static final IdsProducer _instance = new IdsProducer();

    public static IdsProducer getInstance() {
	return _instance;
    }

    public synchronized static Long getMaxId() {
	PreloaderLogger.debug("IdsProducer.getMaxId: " + maxId);
	return maxId;
    }

    public synchronized void resetMaxId() {
	PreloaderLogger.info("IdsProducer.resetMaxId");
	maxId = Long.MIN_VALUE;
    }

    public ProducerData produce(Long beginId) {
	PreloaderLogger.debug("IdsProducer.produce");
	List<Long> ids = new ArrayList<Long>();
	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "user_passport");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id FROM user_passport WHERE id > "
			    + beginId + " ORDER BY id LIMIT " + BATCHSIZE;
		    ResultSet result = s.executeQuery(sql);
		    try {
			String info = null;
			while (result.next()) {
			    Long id = result.getLong("id");
			    ids.add(id);
			    info = String.valueOf(id);
			    maxId = Math.max(id, maxId);
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			result.close();
		    }
		} catch (Exception e) {
		    PreloaderLogger.warn(e);
		} finally {
		    s.close();
		}
	    } catch (Exception e) {
		PreloaderLogger.warn(e);
	    } finally {
		c.close();
	    }
	} catch (Exception e) {
	    PreloaderLogger.warn(e);
	}
	PreloaderLogger.debug("IdsProducer.produce begin: " + beginId
		+ " maxId: " + maxId);
	return new ProducerData(ids, beginId, maxId);
    }
}
