#include "StateUpdater.h"
#include <math.h>
#include "ServiceI.h"
#include "ConnectionQuery.h"
#include "QueryRunner.h"

using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::xce;
/*
StateUpdater::StateUpdater(
	string database,
	string table,
	string valueColumn,
	StrSeq& idColumns) :
	BasicUpdater<Ice::Int, string>(database, table, valueColumn, idColumns)
{

}

string
StateUpdater::calc(const string& orig, const string& inc)
{
	return inc;
}

void
StateUpdater::trim(IdSetMap& idSets)
{
	// If _count2SetMapSwap is empty that means we have no jobs to now,
	// just sleep n ms and do next loop
	if (idSets.empty()) {
		//sleep(5);
		return;
	}
	MCE_DEBUG("CountDistributing: " << idSets.size());

	// Get max loop count for handling all view counts
	int loopCount = idSets.size();
	// Limit loop count as n, that means we update db n times in one
	// secon at most
	if (loopCount > 10) {
		loopCount = 10;
	}
	// Get every looping time
	int loopInterval = 1000000 / loopCount;
   	for(IdSetMap::iterator it = idSets.begin(); it != idSets.end(); ++it) {
   		fixSave(it->second, it->first);
   		usleep(loopInterval);
   	};
   	idSets.clear();
}

void
StateUpdater::fixSave(IdSet& ids, const string& value)
{
	if (ids.size() > 500) {
		MCE_WARN("Too many ids updated in one time: " << ids.size()
			<< ", but we just do it");
	}
    Statement sql;
    sql << "UPDATE " << _table <<
    	" SET " << _valueColumn << " = '" << value <<
    	"' WHERE " << _idColumns.at(0) << " IN (" ;
    for(IdSet::iterator it = ids.begin() ; it != ids.end(); ++it)
    {
        if (it == ids.begin())  {
            sql<< (*it);
        } else {
            sql<<" , " << (*it);
        }
    }
    sql << " )";
    try {
		ConnectionQuery conn(_database, CDbWServer);
    	conn.doUpdate( sql);
    } catch (Ice::Exception& e) {
    	MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
    }
}
*/

StateUpdater::StateUpdater(
	string database,
	string table,
	string valueColumn,
	StrSeq& idColumns,
	int tabCluster,
	bool useInsertDuplicate) :
		BasicUpdater<vector<Ice::Long> , string>(database, table, valueColumn, idColumns),
		_tabCluster(tabCluster),
		_useInsertDuplicate(useInsertDuplicate),
		_patternPrefix(table)
{
}

string StateUpdater::calc(const string& orig, const string& inc) {
	return inc;
}

void
StateUpdater::trim(IdSetMap& idSets)
{
	// If _count2SetMapSwap is empty that means we have no jobs to now,
	// just sleep n ms and do next loop
	if (idSets.empty()) {
		//sleep(5);
		return;
	}
	MCE_DEBUG("CountDistributing: " << idSets.size());

	// Get max loop count for handling all view counts
	int loopCount = idSets.size();
	// Limit loop count as n, that means we update db n times in one
	// secon at most
	if (loopCount > 10) {
		loopCount = 10;
	}
	// Get every looping time
	int loopInterval = 1000000 / loopCount;
   	for(IdSetMap::iterator it = idSets.begin(); it != idSets.end(); ++it) {
   		fixSave(it->second, it->first);
   		usleep(loopInterval);
   	};
   	idSets.clear();
}

void StateUpdater::fixSave(IdSet& ids, const string& value) {
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

void StateUpdater::save(IdSet& ids, const string& value, const string& table,
		const string& pattern) {

	if (ids.size() > 500) {
		MCE_WARN("Too many ids updated in one time: " << ids.size()
			<< ", but we just do it");
	}

	Statement sql;

	if(_useInsertDuplicate && value != "" ){
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

		sql << " ON DUPLICATE KEY UPDATE " << _valueColumn << "= VALUES(" << _valueColumn << ")";


	} else {
		sql << "UPDATE " << table << " SET " << _valueColumn << " = '" << value <<
		    	"' WHERE ";

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
			for (size_t col_pos = 0; col_pos < _idColumns.size(); ++col_pos) {
				sql << boost::lexical_cast<string>(_idColumns.at(col_pos)) << "=" << boost::lexical_cast<string>(idSeq.at(col_pos));
				if (col_pos != _idColumns.size() - 1) {
					sql << " AND ";
				}
			}
			sql << ")";
		}
	}

	try {
		QueryRunner(_database, CDbWServer, pattern).schedule(sql);
	} catch (Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	}

}

