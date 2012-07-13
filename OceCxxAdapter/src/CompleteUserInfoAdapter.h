#ifndef __COMPLETE_USER_INFO_H__
#define __COMPLETE_USER_INFO_H__

#include "Singleton.h"
#include <AdapterI.h>
#include "Channel.h"
#include <CompleteUserInfo.h>

namespace xce {
namespace adapter {
namespace completeuserinfo {
using namespace MyUtil;
using namespace xce::completeuserinfo;
class CompleteUserInfoAdapter : virtual public IceUtil::Thread, public MyUtil::ReplicatedClusterAdapterI<xce::completeuserinfo::CompleteUserInfoManagerPrx>, public MyUtil::Singleton<CompleteUserInfoAdapter> {
	public:
		CompleteUserInfoAdapter();
		void completeUserInfo(const MyUtil::IntSeq& userIds);
		void completeUserInfo(int userId);
		bool isValid();
		void setValid(bool valid);
		virtual void run();
	private:
		vector<int> userIds_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;
		CompleteUserInfoManagerPrx getCompleteUserInfoManager(int userId);
}; 
}
}
}

#endif
