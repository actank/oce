#include <boost/lexical_cast.hpp>
#include "base/logging.h"
#include "base/asyncall.h"
#include "dbpool/queryrunner.h"



using namespace com::xiaonei::xce;
using namespace std;

Statement& Statement::operator<<(const mysqlpp::quote_type0& quote) {
	_segments.push_back(std::vector<std::string>());
	return *this;
}

/*template<typename T>
Statement& Statement::operator<<(const T& segment) {
	if(_sql_segments.empty()) _sql_segments.push_back(std::vector<std::string>());
	_sql_segments.rbegin()->push_back(boost::lexical_cast<string>(segment));
	return *this;
}*/

void Statement::query(mysqlpp::Query& query) const{
		if (_segments.empty()) return;
		build_query(query, _segments.at(0), false);
		for(size_t i=1;i<_segments.size();++i){
			build_query(query, _segments.at(i), true);
		}
	}

void Statement::build_query(mysqlpp::Query& query, const std::vector<std::string>& segs, bool is_quote_first) const{
	for(std::vector<std::string>::const_iterator it = segs.begin();	it != segs.end(); ++it) {
		if (it==segs.begin() && is_quote_first)
			query << mysqlpp::quote << *it;
		else
			query << *it;
	}
}


Statement Statement::buildSelect(const string& table,
		const MyUtil::Str2StrMap& filter) {
	Statement sql;
	sql << "SELECT * FROM " << table;
	if (filter.size()) {
		sql << " WHERE ";
		for (MyUtil::Str2StrMap::const_iterator it = filter.begin();;) {
			sql << it->first << "=" << mysqlpp::quote << it->second;
			if (++it == filter.end()) {
				break;
			} else {
				sql << " AND ";
			}
		}
	}
	return sql;
}

Statement Statement::buildUpdate(const string& table,
		const MyUtil::Str2StrMap& filter, const MyUtil::Str2StrMap& properties) {
	Statement sql;
	if (properties.empty()) {
		return sql;
	}

	sql << "UPDATE " << table << " SET ";
	for (MyUtil::Str2StrMap::const_iterator it = properties.begin();;) {
		sql << it->first << "=" << mysqlpp::quote << it->second;
		if (++it == properties.end()) {
			break;
		} else {
			sql << ", ";
		}
	}
	if (filter.size()) {
		sql << " WHERE ";
		for (MyUtil::Str2StrMap::const_iterator it = filter.begin();;) {
			sql << it->first << "=" << mysqlpp::quote << it->second;
			if (++it == filter.end()) {
				break;
			} else {
				sql << " AND ";
			}
		}
	}
	return sql;
}

Statement Statement::buildInsert(const string& table,
		const MyUtil::Str2StrMap& properties) {
	return buildInsert("INSERT INTO ", table, properties);
}

Statement Statement::buildInsertIgnore(const string& table,
		const MyUtil::Str2StrMap& properties) {
	return buildInsert("INSERT IGNORE INTO ", table, properties);
}

Statement Statement::buildReplace(const string& table,
		const MyUtil::Str2StrMap& properties) {
	return buildInsert("REPLACE INTO ", table, properties);
}

Statement Statement::buildDelete(const string& table,
		const MyUtil::Str2StrMap& filter) {
	Statement sql;
	sql << "DELETE FROM " << table;
	if (filter.size()) {
		sql << " WHERE ";
		for (MyUtil::Str2StrMap::const_iterator it = filter.begin();;) {
			sql << it->first << "=" << mysqlpp::quote << it->second;
			if (++it == filter.end()) {
				break;
			} else {
				sql << " AND ";
			}
		}
	}
	return sql;
}

Statement Statement::buildInsert(const string& prefix, const string& table,
		const MyUtil::Str2StrMap& properties) {
	Statement sql;
	if (properties.empty()) {
		return sql;
	}
	sql << prefix << table << " (";
	for (MyUtil::Str2StrMap::const_iterator it = properties.begin();;) {
		sql << it->first;
		if (++it == properties.end()) {
			break;
		} else {
			sql << ", ";
		}
	}
	sql << ") VALUES (";
	for (MyUtil::Str2StrMap::const_iterator it = properties.begin();;) {
		sql << mysqlpp::quote << it->second;
		if (++it == properties.end()) {
			break;
		} else {
			sql << ", ";
		}
	}
	sql << ")";
	return sql;
}

//---------------------------------------------------------------------------
//#ifndef NEWARCH
//size_t ResultHandler::handle(mysqlpp::ResUse& res) const {
//	size_t count = 0;
//	try {
//		mysqlpp::Row row;
//		while (row = res.fetch_row()) {
//			if (!handle(row)) {
//				break;
//			}
//			++count;
//		}
//	} catch (mysqlpp::EndOfResults& e) {
//		// This exception is expected.
//	} catch (mysqlpp::Exception& e) {
//		LOG(WARNING) << "ResultHandler::handle mysqlpp::Exception -> " << e.what() );
//	} catch (std::exception& e) {
//		LOG(WARNING) << "ResultHandler::handle std::exception -> " << e.what() );
//	} catch (...) {
//		LOG(WARNING) << "ResultHandler::handle -> Unknown exception ");
//	}
//	finish();
//	return count;
//}
//#else
size_t ResultHandler::handle(mysqlpp::UseQueryResult& res) const {
	size_t count = 0;
	try {
		while (mysqlpp::Row row = res.fetch_row()) {
			if (!handle(row)) {
				break;
			}
			++count;
		}
	} catch (mysqlpp::Exception& e) {
		LOG(WARNING) << "ResultHandler::handle mysqlpp::Exception -> " << e.what();
	} catch (std::exception& e) {
		LOG(WARNING) << "ResultHandler::handle std::exception -> " << e.what();
	} catch (...) {
		LOG(WARNING) << "ResultHandler::handle -> Unknown exception ";
	}
	finish();
	return count;
}
//#endif

//---------------------------------------------------------------------------
QueryRunner::QueryRunner(const string& name, int wrflag, const string& pattern) :
	_name(name), _wrflag(wrflag), _pattern(pattern) {
}

size_t QueryRunner::query(const Statement& statement,
		const ResultHandler& handler) {
	ConnectionHolder conn(_name, _wrflag, _pattern);
	mysqlpp::Query query = conn.query();
	statement.query(query);
//#ifndef NEWARCH
//	MCE_DEBUG("QueryRunner::query -> " + query.preview());
//	mysqlpp::ResUse res = query.use();
//#else
	LOG(INFO) << "QueryRunner::query -> " + query.str();
	mysqlpp::UseQueryResult res = query.use();
//#endif
	if (!res) {
		return 0;
	}
	size_t count = handler.handle(res);
	// Check for error: can't distinguish "end of results" and
	// error cases in return from fetch_row() otherwise.
	if (conn.errnum()) {
		LOG(WARNING) << "ResultHandler::handle -> exception when fetching: " << conn.error();
		return 0;
	}

	return count;
}

//#ifndef NEWARCH
//mysqlpp::Result QueryRunner::store(const Statement& statement) {
//#else
mysqlpp::StoreQueryResult QueryRunner::store(const Statement& statement) {
//#endif
	ConnectionHolder conn(_name, _wrflag, _pattern);
	mysqlpp::Query query = conn.query();
	statement.query(query);
	LOG(INFO)  << "QueryRunner::query -> " + query.str();
	return query.store();
}

void foo(Statement _statement, string _name, int _wrflag, string _pattern, string _log) {
  try {
    ConnectionHolder conn(_name, _wrflag, _pattern);
    mysqlpp::Query query = conn.query();
    _statement.query(query);
    if (_log.empty()) {
      LOG(INFO) << "QueryScheduler::run -> " << query.str();
    } else {
      LOG(INFO) << "QueryScheduler::run -> " << _log;
    }
    query.execute();
  } catch (Ice::Exception& e) {
    LOG(WARNING) << "QueryScheduler::run exception -> " << e;
  } catch (mysqlpp::Exception& e) {
    LOG(WARNING) << "QueryScheduler::run exception -> " << e.what();
  } catch (std::exception& e) {
    LOG(WARNING) << "QueryScheduler::run exception -> " << e.what();
  } catch (...) {
    LOG(WARNING) << "QueryScheduler::run exception ";
  }
}

void QueryRunner::schedule(const Statement& statement, const std::string& log) {
#if 0
	MyUtil::TaskManager::instance().execute(new QueryHandler(statement,
			_name, _wrflag, _pattern, log));
#endif

  ::xce::Post(boost::bind(foo, statement, _name, _wrflag, _pattern, log));
}

//#ifndef NEWARCH
//mysqlpp::ResNSel QueryRunner::execute(const Statement& statement,
//#else
mysqlpp::SimpleResult QueryRunner::execute(const Statement& statement,
//#endif
		const std::string& log) {
	ConnectionHolder conn(_name, _wrflag, _pattern);
	mysqlpp::Query query = conn.query();
	statement.query(query);
	if (log.empty()) {
		LOG(INFO) << "QueryRunner::execute "<<__LINE__<<" -> " << query.str();
	} else {
		LOG(INFO) << "QueryRunner::execute -> " << log;
	}
	return query.execute();
}

//---------------------------------------------------------------------------
#if 0
void QueryHandler::handle() {
	try {
		ConnectionHolder conn(_name, _wrflag, _pattern);
		mysqlpp::Query query = conn.query();
		_statement.query(query);
		if (_log.empty()) {
			LOG(INFO) << "QueryScheduler::run -> " << query.str());
		} else {
			LOG(INFO) << "QueryScheduler::run -> " << _log);
		}
		query.execute();
	} catch (Ice::Exception& e) {
		LOG(WARNING) << "QueryScheduler::run exception -> " << e);
	} catch (mysqlpp::Exception& e) {
		LOG(WARNING) << "QueryScheduler::run exception -> " << e.what());
	} catch (std::exception& e) {
		LOG(WARNING) << "QueryScheduler::run exception -> " << e.what());
	} catch (...) {
		LOG(WARNING) << "QueryScheduler::run exception ");
	}
}
#endif

