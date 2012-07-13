#ifndef __GETDATA_STATUS_BUDDYBYIDLOADER_H__
#define __GETDATA_STATUS_BUDDYBYIDLOADER_H__

#include "../common.h"
#include "BuddyByIdReplicaAdapter.cpp"

namespace xce {
namespace generalcheck {
class GetDataStatusBuddyByIdLoaderHandler : public GetDataHandler {
public:
	GetDataStatusBuddyByIdLoaderHandler(const string& name) : GetDataHandler(name) {
	}
	virtual MyUtil::ObjectMap getData(const MyUtil::LongSeq& ids) {
		MCE_INFO("debug GetDataStatusBuddyByIdLoaderHandler::getData ids size = " << ids.size() << " begins");
		MyUtil::ObjectMap res;
		try {
			//MCE_INFO("define StatusDataPtr in BuddyByIdLoader");
			StatusDataPtr ptr = new StatusData;
			//MCE_INFO("define StatusDataPtr in BuddyByIdLoader finished");
			bool success = true;
			xce::buddy::adapter::BuddyByIdLoaderAdapter::instance().getAllStatus(ptr->status_, success);
			//MCE_INFO("BuddyByIdLoaderAdapter::getAllStatus finished");
			if(!success) {
				MCE_INFO("BuddyByIdLoaderAdapter::getAllStatus failed");
				return res;
			}
			res.insert(make_pair(0, ptr));	
		} catch(const exception& e) {
			//std::cout << "GetDataStatusBuddyByIdLoaderHandler::getData Exception " << e.what() << endl;
			MCE_INFO("GetDataStatusBuddyByIdLoaderHandler::getData Exception " << e.what());
		} catch(...) {
			MCE_INFO("GetDataStatusBuddyByIdLoaderHandler::getData unknonwn Exception");
		}
		MCE_INFO("debug GetDataStatusBuddyByIdLoaderHandler::getData ids size = " << ids.size() << " end");
		return res;
	}
};
}
}

#endif
