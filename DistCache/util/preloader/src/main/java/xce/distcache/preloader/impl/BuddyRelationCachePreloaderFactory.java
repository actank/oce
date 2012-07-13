package xce.distcache.preloader.impl;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Map.Entry;

import oce.buddyrelationcache.BuddyRelationCache;
import oce.buddyrelationcache.BuddyRelationCache.RelationEntry;

import com.xiaonei.xce.XceAdapter;

import xce.distcache.preloader.PreloaderFactory;
import xce.distcache.preloader.PreloaderLogger;
import xce.distcache.preloader.ProducerData;

public class BuddyRelationCachePreloaderFactory implements PreloaderFactory {

	@Override
	public Map<Long, byte[]> CreateFromDB(ProducerData data) {
		// TODO Auto-generated method stub
		PreloaderLogger.debug("BuddyRelationCachePreloaderFactory.CreateFromDB "
				+ data.getIds().size());
		Map<Long, BuddyRelationCache.RelationCacheData.Builder> resultTmp = new HashMap<Long, BuddyRelationCache.RelationCacheData.Builder>();

		for (int i = 0; i < data.getIds().size(); i++) {
			Long id = data.getIds().get(i);
			BuddyRelationCache.RelationCacheData.Builder buddyRelationBuilder = BuddyRelationCache.RelationCacheData
					.newBuilder();

			try {
				Connection c = XceAdapter.getInstance().getReadConnection(
						"buddyrelationcache_block");
				try {
					Statement s = c.createStatement();
					try {
						String sql = "SELECT guest FROM relation_block WHERE  host = '"
								+ id + "'";

						ResultSet blockInfo = s.executeQuery(sql);
						try {
							while (blockInfo.next()) {
								Long guest = blockInfo.getLong("guest");
								BuddyRelationCache.RelationEntry.Builder entry = BuddyRelationCache.RelationEntry
										.newBuilder();
								entry.setId(guest.intValue());
								entry.setDesc(BuddyRelationCache.BuddyDesc.Block);
								buddyRelationBuilder.addList(entry);
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

			try {
				Connection c = XceAdapter.getInstance().getReadConnection(
						"buddyrelationcache_block");
				try {
					Statement s = c.createStatement();
					try {
						String sql = "SELECT host FROM relation_block WHERE  guest = '"
								+ id + "'";

						ResultSet blockedInfo = s.executeQuery(sql);
						try {
							while (blockedInfo.next()) {
								Long host = blockedInfo.getLong("host");
								BuddyRelationCache.RelationEntry.Builder entry = BuddyRelationCache.RelationEntry
										.newBuilder();
								entry.setId(host.intValue());
								entry.setDesc(BuddyRelationCache.BuddyDesc.Blocked);
								buddyRelationBuilder.addList(entry);
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

			try {
				Connection c = XceAdapter.getInstance().getReadConnection(
						"buddyrelationcache_relation");
				try {
					Statement s = c.createStatement();
					try {
						String sql = "SELECT guest FROM relation_" + id % 100
								+ " WHERE host = '" + id + "' AND disc=0";

						ResultSet friendInfo = s.executeQuery(sql);
						try {
							while (friendInfo.next()) {
								Long guest = friendInfo.getLong("guest");
								BuddyRelationCache.RelationEntry.Builder entry = BuddyRelationCache.RelationEntry
										.newBuilder();
								entry.setId(guest.intValue());
								entry.setDesc(BuddyRelationCache.BuddyDesc.Friend);
								buddyRelationBuilder.addList(entry);
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

			try {
				Connection c = XceAdapter.getInstance().getReadConnection(
						"buddyrelationcache_accepter");
				try {
					Statement s = c.createStatement();
					try {
						String sql = "SELECT applicant FROM buddyapply_accepter_" + id
								% 100 + " WHERE accepter = '" + id + "'";

						ResultSet appliedInfo = s.executeQuery(sql);
						try {
							while (appliedInfo.next()) {
								Long applied = appliedInfo.getLong("applicant");
								BuddyRelationCache.RelationEntry.Builder entry = BuddyRelationCache.RelationEntry
										.newBuilder();
								entry.setId(applied.intValue());
								entry.setDesc(BuddyRelationCache.BuddyDesc.Applied);
								buddyRelationBuilder.addList(entry);
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

			try {
				Connection c = XceAdapter.getInstance().getReadConnection(
						"buddyrelationcache_applicant");
				try {
					Statement s = c.createStatement();
					try {
						String sql = "SELECT accepter FROM buddyapply_applicant_" + id
								% 100 + " WHERE applicant = '" + id + "'";

						ResultSet applyInfo = s.executeQuery(sql);
						try {
							while (applyInfo.next()) {
								Long apply = applyInfo.getLong("accepter");
								BuddyRelationCache.RelationEntry.Builder entry = BuddyRelationCache.RelationEntry
										.newBuilder();
								entry.setId(apply.intValue());
								entry.setDesc(BuddyRelationCache.BuddyDesc.Apply);
								buddyRelationBuilder.addList(entry);
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

			// 过滤，只存比自己大的id
			BuddyRelationCache.RelationCacheData.Builder buddyRelationBuilderResult = BuddyRelationCache.RelationCacheData
					.newBuilder();
			int loserCount = 0;
			int winnerCount = 0;
			for (int index = 0; index < buddyRelationBuilder.getListCount(); index++) {
				RelationEntry entry = buddyRelationBuilder.getList(index);
				if (entry.getId() > id) {
					buddyRelationBuilderResult.addList(entry);
					winnerCount++;
				} else {
					loserCount++;
				}
			}
			PreloaderLogger.debug("id: " + id + " loserCount: " + loserCount
					+ " winnerCount: " + winnerCount);
			resultTmp.put(id, buddyRelationBuilderResult);
		}// for for();

		// 过滤掉空的用户，并序列化
		Map<Long, byte[]> result = new TreeMap<Long, byte[]>();
		int emptyCount = 0;
		int validCount = 0;
		for (Entry<Long, BuddyRelationCache.RelationCacheData.Builder> entry : resultTmp
				.entrySet()) {
			if (0 != entry.getValue().getListCount()) {
				try {
					result.put(entry.getKey(), entry.getValue().build().toByteArray());
				} catch (Exception e) {
					PreloaderLogger
							.warn("BuddyRelationCache.build().toByteArray, Exception: "
									+ e.toString());
				}
				validCount++;
			} else {
				emptyCount++;
			}
		}

		PreloaderLogger.debug("BuddyRelationCache validCount: " + validCount
				+ " emptyCount: " + emptyCount + " ids.size: " + data.getIds().size());
		return result;
	}

	@Override
	public Map<Long, byte[]> CreateFromNode(List<Long> ids) {
		// TODO Auto-generated method stub
		return null;
	}

}