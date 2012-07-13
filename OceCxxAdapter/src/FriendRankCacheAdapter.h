#ifndef _FRIENDRANK_CACHE_ADAPTER_H_
#define _FRIENDRANK_CACHE_ADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include "TopicI.h"
#include "AdapterI.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "FriendRank.h"

namespace xce {
	namespace socialgraph {

		//class FriendRankCacheAdapter: virtual public MyUtil::AdapterI, 
		//	virtual public MyUtil::AdapterISingleton<MyUtil::OceChannel, FriendRankCacheAdapter>
		// HA化的服务的Adapter，须继承自ReplicatedClusterAdapterI
		class FriendRankCacheAdapter: 
			virtual public MyUtil::ReplicatedClusterAdapterI<FriendRankCachePrx>,
			virtual public MyUtil::Singleton<FriendRankCacheAdapter>
		{
			public:
				FriendRankCacheAdapter() :
					MyUtil::ReplicatedClusterAdapterI<FriendRankCachePrx>(
							"ControllerFriendRankCache", 120, 300) {
						_managers.resize(cluster() > 0 ? cluster() : 1);
						//MCE_INIT("./log", "INFO");
					}
				virtual ~FriendRankCacheAdapter();

				RankSeq GetRank(int userId);
				RankMap GetRevRank(int userId);
				void SetRank(int userId, const FriendRankDataPtr& list);
				void Load(const map<int,vector<int> >& rankdatas);

			protected:
				bool isValid(const Ice::ObjectPrx& proxy);
				friend class MyUtil::Singleton<FriendRankCacheAdapter>;

			private:
				vector<FriendRankCachePrx> _managers;

				virtual string name()
				{
					return "FriendRankCache";
				}
				virtual string endpoints()
				{
					return "@FriendRankCache";
				}
				virtual size_t cluster()
				{
					return 10;  // 部署服务的份数，必须对应，因为服务命名规则使用了这个数字
				}
		};
	}
}

#endif

