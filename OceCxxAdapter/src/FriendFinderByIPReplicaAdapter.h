#ifndef __FRIENDFINDER_BYIP_REPLICA_ADAPTER_H__
#define __FRIENDFINDER_BYIP_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <FriendFinderByIP.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>

namespace xce {
namespace friendfinder {
namespace adapter {

class FriendFinderByIPAdapter : public MyUtil::ReplicatedClusterAdapterI<FriendFinderByIPManagerPrx>, public MyUtil::Singleton<FriendFinderByIPAdapter> {
public:
	FriendFinderByIPAdapter() :
		MyUtil::ReplicatedClusterAdapterI <FriendFinderByIPManagerPrx>("ControllerFriendFinderByIP", 120, 300) {
	}

	IPNoteSeq getFreqIP(Ice::Int userId,Ice::Int limit);
	IPNoteSeq getRecIP(Ice::Int userId,Ice::Int limit);

	void accessIP(Ice::Int userId,Ice::Long ip);
	void accessIPStr(Ice::Int userId,const std::string& ip);
	void accessIPs(Ice::Int userId,const MyUtil::LongSeq& ips);
	void accessIPsStr(Ice::Int userId,const MyUtil::StrSeq& ips);

};

};
};
};

#endif
