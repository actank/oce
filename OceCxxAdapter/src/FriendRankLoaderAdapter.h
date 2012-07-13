#ifndef _FRIENDRANK_LOADER_ADAPTER_H_
#define _FRIENDRANK_LOADER_ADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include "TopicI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include "FriendRank.h"

namespace xce {
	namespace socialgraph {

		class FriendRankLoaderAdapter: 
			virtual public MyUtil::AdapterI,	
			virtual public MyUtil::AdapterISingleton<MyUtil::SocialGraphChannel, FriendRankLoaderAdapter>
		{
			public:
				FriendRankLoaderAdapter();
				virtual ~FriendRankLoaderAdapter();
				void reload(int userId);

			protected:
				bool isValid(const Ice::ObjectPrx& proxy);

				friend class MyUtil::Singleton<FriendRankLoaderAdapter>;

			private:
				vector<FriendRankLoaderPrx> _managers;

				virtual string name()
				{
					return "FriendRankLoader";
				}
				virtual string endpoints()
				{
					return "@FriendRankLoader";
				}
				virtual size_t cluster()
				{
					return 0;  // 部署服务的份数，必须对应，因为服务命名规则使用了这个数字
				}
		};
	}
}

#endif

