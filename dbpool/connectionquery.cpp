#include <boost/lexical_cast.hpp>
#include "base/logging.h"

// #include <fnmatch.h>
#include "dbpool/connectionquery.h"

using namespace com::xiaonei::xce;

//---------------------------------------------------------------------------
ConnectionQuery::ConnectionQuery(const string& name, int wrflag,
		const string& pattern) :
	ConnectionHolder(name, wrflag, pattern) {

}

ConnectionQuery::~ConnectionQuery() {

}

ResSetPtr ConnectionQuery::doQuery(const string& sql,
		const ResSetFactoryPtr factory) {
	ResSetPtr rs = factory->create();
	try {
		mysqlpp::Query query = ConnectionHolder::query();
		query << sql;
//#ifndef NEWARCH
//		mysqlpp::ResUse res = query.use();
//#else
		mysqlpp::UseQueryResult res = query.use();
//#endif
		if (res) {
			rs->buildResult(res);
		}
		LOG(INFO) << "ConnectionQuery::doQuery -> " << sql << ", Count: " << rs->size();
		return rs;
	} catch (mysqlpp::Exception& e) {
		LOG(WARNING) << "ConnectionQuery::doQuery -> " << sql << ", Exception: " << e.what();
		throw e;
	} catch (XceException& e) {
		LOG(WARNING) << "ConnectionQuery::doQuery -> " << sql << ", Exception: " << e;
		throw e;
	} catch (Ice::Exception& e) {
		LOG(WARNING) << "ConnectionQuery::doQuery -> " << sql << ", Exception: " << e;
		throw e;
	} catch (...) {
		LOG(WARNING) << "ConnectionQuery::doQuery -> " << sql << ", UnknownException";
		throw "UnkownException";
	}
}

ResSetPtr ConnectionQuery::doSelect(const string& sql,
		const ResSetFactoryPtr factory) {
	return doQuery(sql, factory);
}

ResSetPtr ConnectionQuery::doSelect(const string& table,
		const StringMap& filter, const ResSetFactoryPtr factory) {
	ostringstream sql;
	sql << "SELECT * FROM " << table;
	if (filter.size()) {
		sql << " WHERE ";
		for (StringMap::const_iterator it = filter.begin();;) {
//#ifndef NEWARCH
//			// sql << it->first << "=" << mysqlpp::quote << it->second;
//			sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#else
			sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#endif
			if (++it == filter.end()) {
				break;
			} else {
				sql << " AND ";
			}
		}
	}
	return doQuery(sql.str(), factory);
}

Ice::Long ConnectionQuery::doExecute(const string& sql) {
	try {
		LOG(INFO) << "ConnectionQuery::doExecute -> " << sql;
		mysqlpp::Query query = ConnectionHolder::query();
		query << sql;
//#ifndef NEWARCH
//		mysqlpp::ResNSel res = query.execute();
//#else
		mysqlpp::SimpleResult res = query.execute();
//#endif

//#ifndef NEWARCH
//		return res.insert_id;
//#else
		return res.insert_id();
//#endif
	} catch (mysqlpp::Exception& e) {
		LOG(WARNING) << "ConnectionQuery::doExecute -> " << sql << ", Exception: " << e.what();
		throw e;
	} catch (Ice::Exception& e) {
		LOG(WARNING) << "ConnectionQuery::doExecute -> " << sql << ", Exception: " << e;
		throw e;
	} catch (...) {
		LOG(WARNING) << "ConnectionQuery::doExecute -> " << sql << ", UnknownException";
		throw "UnkownException";
	}
}

void ConnectionQuery::doUpdate(const string& sql) {
	doExecute(sql);
}

void ConnectionQuery::doUpdate(const string& table, const StringMap& filter,
		const StringMap& properties) {
	if (!properties.size()) {
		return;
	}

	ostringstream sql;
	sql << "UPDATE " << table << " SET ";
	for (StringMap::const_iterator it = properties.begin();;) {
//#ifndef NEWARCH
//		// sql << it->first << "=" << mysqlpp::quote << it->second;
//		sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#else
		sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#endif
		if (++it == properties.end()) {
			break;
		} else {
			sql << ", ";
		}
	}
	if (filter.size()) {
		sql << " WHERE ";
		for (StringMap::const_iterator it = filter.begin();;) {
//#ifndef NEWARCH
//			// sql << it->first << "=" << mysqlpp::quote << it->second;
//			sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#else
			sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#endif
			if (++it == filter.end()) {
				break;
			} else {
				sql << " AND ";
			}
		}
	}
	doExecute(sql.str());
}

Ice::Long ConnectionQuery::doInsert(const string& sql) {
	return doExecute(sql);
}

Ice::Long ConnectionQuery::doInsert(const string& table,
		const StringMap& properties) {
	return doInsert("INSERT INTO ", table, properties);
}

Ice::Long ConnectionQuery::doInsertIgnore(const string& table,
		const StringMap& properties) {
	return doInsert("INSERT IGNORE INTO ", table, properties);
}

Ice::Long ConnectionQuery::doReplace(const string& table,
		const StringMap& properties) {
	return doInsert("REPLACE INTO ", table, properties);
}

Ice::Long ConnectionQuery::doInsert(const string& strPrefix,
		const string& table, const StringMap& properties) {
	if (!properties.size()) {
		return -1;
	}
	ostringstream sql;
	sql << strPrefix << table << " (";
	for (StringMap::const_iterator it = properties.begin();;) {
		sql << it->first;
		if (++it == properties.end()) {
			break;
		} else {
			sql << ", ";
		}
	}
	sql << ") VALUES (";
	for (StringMap::const_iterator it = properties.begin();;) {
//#ifndef NEWARCH
//		// sql << mysqlpp::quote << it->second;
//		sql << "'" << mysqlpp::quote << it->second << "'";
//#else
		sql << "'" << mysqlpp::quote << it->second << "'";
//#endif
		if (++it == properties.end()) {
			break;
		} else {
			sql << ", ";
		}
	}
	sql << ")";
	return doExecute(sql.str());
}

void ConnectionQuery::doDelete(const string& sql) {
	doExecute(sql);
}

void ConnectionQuery::doDelete(const string& table, const StringMap& filter) {
	ostringstream sql;
	sql << "DELETE FROM " << table;
	if (filter.size()) {
		sql << " WHERE ";
		for (StringMap::const_iterator it = filter.begin();;) {
//#ifndef NEWARCH
//			// sql << it->first << "=" << mysqlpp::quote << it->second;
//			sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#else
			sql << it->first << "=" << "'" << mysqlpp::quote << it->second << "'";
//#endif
			if (++it == filter.end()) {
				break;
			} else {
				sql << " AND ";
			}
		}
	}
	doExecute(sql.str());
}
