#ifndef __IPRECORD_ADAPTER_H__
#define __IPRECORD_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IPRecord.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>

namespace xce {
namespace iprecord {
namespace adapter {

class IPRecordAdapter : public MyUtil::ReplicatedClusterAdapterI<IPRecordManagerPrx>, public MyUtil::Singleton<IPRecordAdapter> {
public:
	IPRecordAdapter() :
		MyUtil::ReplicatedClusterAdapterI <IPRecordManagerPrx>("ControllerIPRecord", 120, 300) {
	}
	MyUtil::IntSeq getUsers(Ice::Long ip,Ice::Int limit);
	MyUtil::IntSeq getUsersWithMask(Ice::Long ip,Ice::Int limit,const MyUtil::IntSeq& mask);
	Ice::Int getCount(Ice::Long ip);
	void addUser(Ice::Long ip,Ice::Int userId);
	void removeUser(Ice::Long ip,Ice::Int userId);
	void reload(Ice::Long ip);
	void updateToDB(Ice::Long ip);
};

};
};
};

#endif
