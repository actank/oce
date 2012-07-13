#ifndef _FRIENDRANK_UPDATE_ADAPTER_H_
#define _FRIENDRANK_UPDATE_ADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include "TopicI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include "FriendRankUpdate.h"

namespace xce {
	namespace socialgraph {

		class FriendRankUpdateAdapter: 
			virtual public MyUtil::AdapterI,	
			virtual public MyUtil::AdapterISingleton<MyUtil::SocialGraphChannel, FriendRankUpdateAdapter>
		{
			public:
				FriendRankUpdateAdapter();
				virtual ~FriendRankUpdateAdapter();
				int TriggerUserModel();
				int TriggerFriendRank();

			protected:
				friend class MyUtil::Singleton<FriendRankUpdateAdapter>;
				bool isValid(const Ice::ObjectPrx& proxy);

			private:
				vector<FriendRankUpdatePrx> _managers;

				virtual string name()
				{
					return "FriendRankUpdate";
				}
				virtual string endpoints()
				{
					return "@FriendRankUpdate";
				}
				virtual size_t cluster()
				{
					return 0;  // 部署服务的份数，必须对应，因为服务命名规则使用了这个数字
				}
		};
	}
}

#endif

