#include "CountUpdater.h"
#include <math.h>
#include "ServiceI.h"
#include "util/cpp/String.h"
#include "QueryRunner.h"
#include <boost/lexical_cast.hpp>

using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::xce;

CountUpdater::CountUpdater(string database, string table, string valueColumn,
		StrSeq& idColumns, int timeSlice, int tabCluster,
		bool useInsertDuplicate,
		int beginIdColumnsInSql) :
			BasicUpdater<vector<Ice::Long> , Ice::Int> (database, table, valueColumn,
				idColumns), 
			_timeSlice(timeSlice), 
			_tabCluster(tabCluster),
			_patternPrefix(table), 
			_useInsertDuplicate(useInsertDuplicate),
			_beginIdColumnsInSql(beginIdColumnsInSql) {
	
	_buildSqlThread = new BuildSqlVector;
	_buildSqlThread->start(65535).detach();
}

CountUpdater::CountUpdater(
		const DescDatabase& db,
		const DescSqlbuilder& sqlbuilder,
		time_t timeSlice,
		int beginIdColumnsInSql) :
			BasicUpdater<vector<Ice::Long> , Ice::Int> (db.database,
				sqlbuilder.tablePrefix, sqlbuilder.valueColumn,
				sqlbuilder.idColumns), 
			_timeSlice(timeSlice), 
			_tabCluster(db.tableCluster), 
			_patternPrefix(db.patternPrefix),
			_useInsertDuplicate(sqlbuilder.useInsertDuplicate),
			_beginIdColumnsInSql(beginIdColumnsInSql) {
	
//	BuildSqlVector::instance().start(65535).detach();

         _buildSqlThread = new BuildSqlVector;
         _buildSqlThread->start(65535).detach();

}

Ice::Int CountUpdater::calc(const Ice::Int& orig, const Ice::Int& inc) {
	return orig + inc;
}

void CountUpdater::trim(IdSetMap& idSets) {
	// If idSets is empty that means we have no jobs to now,
	// just sleep n ms and do next loop
	if (idSets.empty()) {
		//sleep(5);
		return;
	}
	MCE_DEBUG("CountDistributing: " << idSets.size());
	// Get max loop count for handling all view counts
	int loopCount = ((int) (log(idSets.size()) / log(2))) + 1;
	// Limit loop count as n, that means we update db n times in one
	// secon at most
	if (loopCount > 100) {
		loopCount = 100;
	}
	// Get every looping time
	int loopInterval = _timeSlice * 1000000 / loopCount;

	IdSetMap negativeIdSets;
	IdSetMap positiveIdSets;
	for (IdSetMap::iterator it = idSets.begin(); it != idSets.end(); ++it) {
		if (it->first < 0) {
			negativeIdSets[abs(it->first)] = it->second;
		} else if (it->first == 0) {
			// fixSave all ids with count 0
			fixSave(it->second, 0);
		} else {
			positiveIdSets[it->first] = it->second;
		}
	}
	trim(negativeIdSets, -1, loopInterval);
	trim(positiveIdSets, 1, loopInterval);
}

void CountUpdater::trim(IdSetMap& idSets, int sign, int loopInterval) {
	// If idSets is empty that means we have no jobs to now,
	// just sleep n ms and do next loop
	if (idSets.empty()) {
		//sleep(5);
		return;
	}
	MCE_DEBUG("CountDistributing: " << idSets.size());

	// Begin loop,
	// The order of idSets is from small to big
	// We just handle the half behind of idSets every loop.
	// Pick thisCount of the start of half behind of idSets,
	// increase thisCount to all objects whose count is >= thisCount,
	// decrease all objects's count by thisCount,
	// erase all objects whose count downto 0
	while (true) {
		// Get the iterator pointing half of idSets
		IdSetMap::iterator it = idSets.begin();
		for (size_t i = 0; it != idSets.end(); ++it) {
			if (++i >= idSets.size() / 2 + 1) {
				break;
			}
		}
		// If idSets is empty, break looping
		if (it == idSets.end()) {
			break;
		}
		// Get thisCount
		int thisCount = it->first;
		IdSet ids;
		for (; it != idSets.end();) {
			// Put all objects into tmp set for saving
			assert(!it->second.empty());
			ids.insert(it->second.begin(), it->second.end());
			int subCount = it->first - thisCount;
			if (subCount != 0) {
				// copy all objects from it to new position with count = subCount
				idSets[subCount].insert(it->second.begin(), it->second.end());
			}
			// erase current it
			idSets.erase(it++);
		}
		fixSave(ids, thisCount * sign);
		// sleep n ms to assure lower load pressure on db
		usleep(loopInterval);
		//sleep(5);
	}
}

void CountUpdater::fixSave(IdSet& ids, const Ice::Int& value) {
	MCE_DEBUG("call fixSave:  _tableCluster" << _tabCluster);
	if (_tabCluster > 0) {
		map<int, IdSet> tmp;
		for (IdSet::iterator it = ids.begin(); it != ids.end(); ++it) {
			tmp[(int) (*it).at(0) % _tabCluster].insert(*it);
		}
		MCE_DEBUG(" tmp map.size=" << tmp.size());
		for (map<int, IdSet>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
			ostringstream table;
			table << _table << it->first;
			ostringstream pattern;
			pattern << _patternPrefix << it->first;
			save(it->second, value, table.str(), pattern.str());
		}
	} else {
		save(ids, value, _table, _patternPrefix);
	}

}

void CountUpdater::save(IdSet& ids, Ice::Int value, const string& table,
		const string& pattern) {
	if (_idColumns.empty()) {
		MCE_FATAL("idColumns cannot be empty " << __FILE__ << " " << __LINE__);
		return;
	}
	if (ids.empty()) {
		MCE_WARN("ids is empty, nothing to do ");
		return;
	}
	if (ids.size() > 500) {
		MCE_WARN("Too many ids updated in one time: " << ids.size()
				<< ", but we just do it");
	}
	
	ostringstream sql;
	if (_useInsertDuplicate && value > 0) {
		sql << "INSERT INTO " << table << " (";
		for (StrSeq::const_iterator it = _idColumns.begin(); it
				!= _idColumns.end(); ++it) {
			sql << *it << ", ";
		}
		sql << _valueColumn;
		sql << ") VALUES ";

		for (IdSet::iterator it = ids.begin(); it != ids.end(); ++it) {
			const LongSeq& idSeq = *it;
			if (idSeq.size() != _idColumns.size()) {
				MCE_WARN("IdSeq.size != IdColumns.size -> " << idSeq.size()
						<< " != " << _idColumns.size());
				continue;
			}
			if (it != ids.begin()) {
				sql << ", ";
			}
			sql << "(";
			for (size_t col_pos = 0; col_pos < _idColumns.size(); ++col_pos) {
				sql << boost::lexical_cast<string>(idSeq.at(col_pos)) << ", ";
			}
			sql << boost::lexical_cast<string>(value) << ")";
		}

		sql << " ON DUPLICATE KEY UPDATE " << _valueColumn << "="
				<< _valueColumn << "+VALUES(" << _valueColumn << ")";
	} else {
		if (_idColumns.size() <= (_beginIdColumnsInSql) ) {
			MCE_WARN("[CountUpdater::save] can't update for no enough useful-idcolumns in sql; " << _idColumns.size()
					<< " begin with " << _beginIdColumnsInSql);
			return;	
		}
		sql << "UPDATE " << table << " SET " << _valueColumn;
		if (value < 0) {
			sql << " = if ((" << _valueColumn << boost::lexical_cast<string>(value) << " ) > 0 , "
					<< _valueColumn << boost::lexical_cast<string>(value) << ", 0) ";
		} else if (value == 0) {
			sql << " = 0 ";
		} else {
			sql << " = " << _valueColumn << " + " << boost::lexical_cast<string>(value);
		}
		sql << " WHERE ";

		for (IdSet::iterator it = ids.begin(); it != ids.end(); ++it) {
			//StrSeq idSeq = StrUtil::split(*it, ",");
			const LongSeq& idSeq = *it;
			if (idSeq.size() != _idColumns.size()) {
				MCE_WARN("IdSeq.size != IdColumns.size -> " << idSeq.size()
						<< " != " << _idColumns.size());
				continue;
			}

			if (it != ids.begin()) {
				sql << " OR ";
			}
			sql << "(";

			// 在update时, 由必须以所有_idColumns的成员为条件，改为只用_beginIdColumnsInSql开始的成员才作为条件
			// 100126 -- by han.zhang
			// for (size_t col_pos = 0; col_pos < _idColumns.size(); ++col_pos) {
			MCE_DEBUG("_beginIdColumnsInSql : " << _beginIdColumnsInSql);
			for (size_t col_pos = _beginIdColumnsInSql; col_pos < _idColumns.size(); ++col_pos) {
				sql << boost::lexical_cast<string>(_idColumns.at(col_pos)) << "=" << boost::lexical_cast<string>(idSeq.at(col_pos));
				if (col_pos != _idColumns.size() - 1) {
					sql << " AND ";
				}
			}
			sql << ")";
		}
	}
	
	//MCE_DEBUG("save : " << sql.str());
	
	_buildSqlThread->build(sql.str(), _database, pattern);


/*	try {
		QueryRunner(_database, CDbWServer, pattern).schedule(sql);
	} catch (Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	}*/

}

void BuildSqlVector::build(std::string sqlStr, std::string database, std::string pattern){
	MCE_DEBUG("build : " << sqlStr);
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	SqlNodePtr sqlNode = new SqlNode;
	sqlNode->database = database;
	sqlNode->pattern = pattern;
	sqlNode->sql = sqlStr;
	_sqlVector.push_back(sqlNode);
	if(_sqlVector.size()>VECTOR_SIZE){
		_mutex.notify();
	}
}

void BuildSqlVector::run(){
	while(true){
		try{
			std::vector<SqlNodePtr> sqlVector;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if(_sqlVector.size()<=VECTOR_SIZE) {
					 _mutex.timedWait(IceUtil::Time::seconds(WAIT_TIME));
				}
				
				if(!_sqlVector.empty()){
					sqlVector.swap(_sqlVector);
				} else {
					continue;
				}				
			}
			TaskManager::instance().execute(new UpdaterTask(sqlVector));
			
		} catch (...) {
			cout << "Something causes BuildSqlVector loop crash" << endl;
		}
	}
}

void UpdaterTask::handle(){
	for(std::vector<SqlNodePtr>::iterator it = _sqlVector.begin(); it != _sqlVector.end(); ++it){
		SqlNodePtr sqlNode = *it;
		std::string sqlStr = sqlNode->sql;
		std::string pattern = sqlNode->pattern;
		std::string database = sqlNode->database;
		Statement sql ;
		sql << sqlStr;

	//	MCE_DEBUG("handle: " << "sql->"<< sqlStr << " pattern:" << pattern << " database:" << database);
		try {
			QueryRunner(database, CDbWServer, pattern).execute(sql);
		} catch (Ice::Exception& e) {
			MCE_WARN(e << "  exception:" << sqlStr );
		}
	}
}

