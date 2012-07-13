#ifndef __GETDATA_STATUSCACHE_H__
#define __GETDATA_STATUSCACHE_H__

#include "../common.h"
#include "StatusCacheAdapter.h"

namespace xce {
namespace generalcheck {
class GetDataStatusCacheHandler : public GetDataHandler {
public:
	GetDataStatusCacheHandler(const string& name) : GetDataHandler(name) {
	}
	virtual MyUtil::ObjectMap getData(const MyUtil::LongSeq& ids) {
		MCE_INFO("debug GetDataStatusCacheHandler::getData ids size = " << ids.size() << " begins");
		MyUtil::ObjectMap res;
		//return res;
		try { 
			//std::cout << "define StatusDataPtr" << std::endl;
			StatusDataPtr ptr = new StatusData();
			//std::cout << "define StatusDataPtr finished" << std::endl;
			xce::statuscache::StatusCacheAdapter::instance().getAllStatus(ptr->status_);
			res.insert(make_pair(0, ptr));	
		} catch(const exception& e) {
			//std::cout << "GetDataStatusCacheHandler::getData Exception " << e.what() << endl;
			MCE_INFO("GetDataStatusCacheHandler::getData Exception " << e.what());
		} catch(...) {
			//std::cout << "GetDataStatusCacheHandler::getData Exception unknown" << endl;
			MCE_INFO("GetDataStatusCacheHandler::getData Exception unknown");
		}
		MCE_INFO("debug GetDataStatusCacheHandler::getData ids size = " << ids.size() << " end");
		return res;
	}
};
}
}

#endif
