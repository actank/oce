#include "facadestatefilter.h"
#include <exception>
#include <IceUtil/IceUtil.h>

using namespace xce::socialgraph;
using namespace xce::socialgraph::friendfindernewcomer;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;

bool FacadeStateResultHandler::handle(mysqlpp::Row& res) const {
        int uid = res["id"];
	_large_id = uid > _large_id ? uid : _large_id;
	int state = (int) res["state"];

        if (state&1) {
		_star_result[uid] = true;
        } else {
		_star_result[uid] = false;
        }

        return true;
}

FacadeStateFilter::FacadeStateFilter() :
	_inited_flag(false) {

	MCE_INFO("FacadeStateFilter() -> reset every bit");
	for (int i=0;i<FACADESTATE_FILTER_SIZE;++i) {
		_facade_state.reset(i);
	}
	MCE_INFO("FacadeStateFilter() -> all bit setted!");
}

void FacadeStateFilter::initialize() {
	MCE_INFO("FacadeStateFilter::initialize -> start ...");

	int _max_id = 0;
	int large_id = 0;

	try {
		do {
			Statement sql;
			sql << "SELECT id, state FROM user_state WHERE id > " << large_id << " LIMIT 10000";

			map<int, bool> db_state_star;
			com::xiaonei::xce::QueryRunner("user_state_preloader",com::xiaonei::xce::CDbRServer ).query(
					sql, FacadeStateResultHandler( large_id, db_state_star )
					);
			{
				for (map<int, bool>::const_iterator it = db_state_star.begin(); it
						!= db_state_star.end(); ++it) {
					if( it->second ){
						_facade_state.set(it->first);
					}
				}
			}

                        if (large_id > _max_id) {
                                _max_id = large_id;
                        } else {
                                break;
                        }

			MCE_INFO("FacadeStateFilterManagetI::initialize -> initializing to " << large_id);
		} while (true);
	} catch (...) {
		MCE_WARN("FacadeStateFilterManagetI::initialize -> loading exit with unknown exception.");
	}
/*
*/

	MCE_INFO("FacadeStateFilterManagetI::initialize -> loading done");
	{
		IceUtil::RWRecMutex::WLock lock(_inited_flag_mutex);
		_inited_flag = true;
	}
}

bool FacadeStateFilter::isValid(){
        IceUtil::RWRecMutex::RLock lock(_inited_flag_mutex);
        return _inited_flag;
}

bool FacadeStateFilter::isFacadeStateTrue(int id){
	return _facade_state[id];
}

void FacadeStateFilter::set(int userId) {
	_facade_state.set(userId);
}

void FacadeStateFilter::reset(int userId) {
	_facade_state.reset(userId);
}

