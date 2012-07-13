package xce.distcache.preloader.impl;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Map.Entry;

import oce.buddyapplycache.BuddyApplyCacheData;
import oce.buddyapplycache.BuddyApplyCacheData.ApplyData;
import oce.buddyapplycache.BuddyApplyCacheData.ApplyDataSeq;
import oce.buddyapplycache.BuddyApplyCacheData.ApplyData.Builder;
import com.xiaonei.xce.XceAdapter;
import xce.distcache.preloader.PreloaderFactory;
import xce.distcache.preloader.PreloaderLogger;
import xce.distcache.preloader.ProducerData;

public class BuddyApplyCachePreloaderFactory implements PreloaderFactory {

	@Override
	public Map<Long, byte[]> CreateFromDB(ProducerData data) {
		// TODO Auto-generated method stub
		PreloaderLogger.debug("BuddyApplyCachePreloaderFactory.CreateFromDB "
				+ data.getIds().size());
		Map<Long, BuddyApplyCacheData.ApplyDataSeq.Builder> resultTmp = new HashMap<Long, BuddyApplyCacheData.ApplyDataSeq.Builder>();
		for (int i = 0; i < data.getIds().size(); i++) {
			Long id = data.getIds().get(i);
			ApplyDataSeq.Builder buddyApplyCacheBuilder = ApplyDataSeq.newBuilder();

			try {
				Connection c = XceAdapter.getInstance().getReadConnection(
						"buddyrelation_accepter_preloader");
				try {
					Statement s = c.createStatement();
					try {
						String sql = "SELECT applicant,UNIX_TIMESTAMP(time) AS unix_time,why,stat_from,group_name"
								+ " FROM buddyapply_accepter_"
								+ Math.abs(id % 100)
								+ " WHERE accepter = '" + id + "' ORDER BY time DESC LIMIT 100";

						ResultSet applyInfo = s.executeQuery(sql);
						try {
							while (applyInfo.next()) {
								Builder entry = ApplyData.newBuilder();
								entry.setApplicant(applyInfo.getLong("applicant"));
								entry.setTime(applyInfo.getInt("unix_time"));
								entry.setWhy(applyInfo.getString("why"));
								entry.setStatfrom(applyInfo.getString("stat_from"));
								entry.setGroupname(applyInfo.getString("group_name"));
								buddyApplyCacheBuilder.addItem(entry);
							}
						} catch (Exception e) {
							PreloaderLogger.warn(e);
						} finally {
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
			}// for one sql;

			resultTmp.put(id, buddyApplyCacheBuilder);
		}

		Map<Long, byte[]> result = new TreeMap<Long, byte[]>();
		for (Entry<Long, oce.buddyapplycache.BuddyApplyCacheData.ApplyDataSeq.Builder> entry : resultTmp
				.entrySet()) {
			try {
				result.put(entry.getKey(), entry.getValue().build().toByteArray());
			} catch (Exception e) {
				PreloaderLogger.warn("BuddyApplyCache.build().toByteArray, Exception: "
						+ e.toString());
			}
		}

		return result;
	}

	@Override
	public Map<Long, byte[]> CreateFromNode(List<Long> ids) {
		// TODO Auto-generated method stub
		return null;
	}

}
