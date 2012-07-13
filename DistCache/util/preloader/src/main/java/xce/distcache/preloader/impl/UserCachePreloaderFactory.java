package xce.distcache.preloader.impl;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Map.Entry;

import com.xiaonei.xce.XceAdapter;

import oce.usercache.UserCache;
import xce.distcache.preloader.PreloaderFactory;
import xce.distcache.preloader.PreloaderLogger;
import xce.distcache.preloader.ProducerData;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-06-13
 */
public class UserCachePreloaderFactory implements PreloaderFactory {

    @Override
    public Map<Long, byte[]> CreateFromDB(ProducerData data) {
	PreloaderLogger
		.debug("UserCachePreloaderFactory.CreateFromDB, ids.size(): "
			+ data.getIds().size());
	System.out.println("UserCachePreloader.CreateFromDB, size " + data.getIds().size());
	Map<Long, UserCache.UserCacheData.Builder> resultTmp = new HashMap<Long, UserCache.UserCacheData.Builder>();

	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "usercache_passport_preloader");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id, status FROM user_passport WHERE id > "
			    + data.getMin() + " AND id <=" + data.getMax();

		    ResultSet passportInfo = s.executeQuery(sql);
		    try {
			while (passportInfo.next()) {
			    UserCache.UserCacheData.Builder user_cache_data_builder = UserCache.UserCacheData
				    .newBuilder();
			    Long id = passportInfo.getLong("id");
			    int status = passportInfo.getInt("status");
			    user_cache_data_builder.setId(id.intValue());
			    user_cache_data_builder.setStatus(status);

			    resultTmp.put(id, user_cache_data_builder);
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			passportInfo.close();
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

	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "usercache_state_preloader");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id, state,star,level FROM user_state WHERE id > "
			    + data.getMin() + " AND id <= " + data.getMax();

		    ResultSet stateInfo = s.executeQuery(sql);
		    try {
			while (stateInfo.next()) {
			    Long id = stateInfo.getLong("id");

			    UserCache.UserCacheData.Builder user_cache_data_builder = resultTmp
				    .get(id);
			    if (user_cache_data_builder != null) {
				int state = stateInfo.getInt("state");
				// int star = stateInfo.getInt("star");
				int level = stateInfo.getInt("level");
				user_cache_data_builder.setState(state);
				user_cache_data_builder.setLevel(level);
			    }
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			stateInfo.close();
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

	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "usercache_stage_preloader");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id, univ FROM user_stage WHERE id > "
			    + data.getMin() + " AND id <= " + data.getMax();

		    ResultSet stageInfo = s.executeQuery(sql);
		    try {
			while (stageInfo.next()) {
			    Long id = stageInfo.getLong("id");

			    UserCache.UserCacheData.Builder user_cache_data_builder = resultTmp
				    .get(id);
			    if (user_cache_data_builder != null) {

				int univ = stageInfo.getInt("univ");
				user_cache_data_builder.setUniv(univ);
			    }
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			stageInfo.close();
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

	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "usercache_born_preloader");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id, gender FROM user_born WHERE id > "
			    + data.getMin() + " AND id <= " + data.getMax();

		    ResultSet bornInfo = s.executeQuery(sql);

		    try {
			while (bornInfo.next()) {
			    Long id = bornInfo.getLong("id");

			    UserCache.UserCacheData.Builder user_cache_data_builder = resultTmp
				    .get(id);
			    if (user_cache_data_builder != null) {
				String gender = bornInfo.getString("gender");
				if (gender.matches("男生")) {
				    user_cache_data_builder.setGender(1);
				} else if (gender.matches("女生")) {
				    user_cache_data_builder.setGender(2);
				} else {
				    user_cache_data_builder.setGender(0);
				}
			    }
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			bornInfo.close();
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
	    PreloaderLogger.debug("user_born is error! , Exception: "
		    + e.toString());
	}

	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "usercache_names_preloader");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id, name FROM user_names WHERE id > "
			    + data.getMin() + " AND id <= " + data.getMax();

		    ResultSet namesInfo = s.executeQuery(sql);
		    try {
			while (namesInfo.next()) {
			    Long id = namesInfo.getLong("id");
			    UserCache.UserCacheData.Builder user_cache_data_builder = resultTmp
				    .get(id);
			    if (user_cache_data_builder != null) {
				String name = namesInfo.getString("name");
				user_cache_data_builder.setName(name);
			    }
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			namesInfo.close();
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
	    PreloaderLogger.debug("user_names is error! , Exception: "
		    + e.toString());
	}

	try {
	    Connection c = XceAdapter.getInstance().getReadConnection(
		    "usercache_url_preloader");
	    try {
		Statement s = c.createStatement();
		try {
		    String sql = "SELECT id, tinyurl FROM user_url WHERE id > "
			    + data.getMin() + " AND id <=" + data.getMax();

		    ResultSet urlInfo = s.executeQuery(sql);
		    try {
			while (urlInfo.next()) {
			    Long id = urlInfo.getLong("id");

			    UserCache.UserCacheData.Builder user_cache_data_builder = resultTmp
				    .get(id);
			    if (user_cache_data_builder != null) {
				String tinyUrl = urlInfo.getString("tinyurl");
				user_cache_data_builder.setTinyUrl(tinyUrl);
			    }
			}
		    } catch (Exception e) {
			PreloaderLogger.warn(e);
		    } finally {
			urlInfo.close();
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
	    PreloaderLogger.debug("user_url is error! , Exception: "
		    + e.toString());
	}

	PreloaderLogger.debug("UserCachePreloaderFactory.CreateFromDB ,size: "
		+ resultTmp.size());
	Map<Long, byte[]> result = new TreeMap<Long, byte[]>();
	for (Entry<Long, UserCache.UserCacheData.Builder> entry : resultTmp
		.entrySet()) {

	    try {
		result.put(entry.getKey(), entry.getValue().build()
			.toByteArray());
	    } catch (Exception e) {
		PreloaderLogger
			.warn("UserCacheData.build().toByteArray, Exception: "
				+ e.toString());
	    }
	}

	// test HOTO read!
	// PreloaderLogger.debug("UserCachePreloaderFactory.CreateFromDB test "
	// + result.size());
	// for (Entry<Long, byte[]> entry : result.entrySet()) {
	// String info;
	// UserCache.UserCacheData data;
	// try {
	// data = UserCache.UserCacheData.parseFrom(entry.getValue());
	// info = "id: " + entry.getKey() + " name: " + data.getName() +
	// " tinyUrl: " + data.getTinyUrl();
	// info = info + " Gender: " + data.getGender() + " level: " +
	// data.getLevel() + " univ: " + data.getUniv();
	// PreloaderLogger.debug("parseData: " + info);
	// } catch (InvalidProtocolBufferException e) {
	// // TODO Auto-generated catch block
	// }
	// }
	return result;
    }

    @Override
    public Map<Long, byte[]> CreateFromNode(List<Long> ids) {
	// TODO Auto-generated method stub
	return null;
    }

}
