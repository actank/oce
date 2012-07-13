#ifndef _FRIEND_RANK_LOADER_I_H_
#define _FRIEND_RANK_LOADER_I_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoaderFactory.h"
#include "ServiceI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include "FriendRank.h"

namespace xce {
	namespace socialgraph {

		const int TASK_LEVEL_FRIEND_RANK_LOADER_LOAD = 1;

		class FriendRankLoaderFactory : 
			virtual public xce::cacheloader::MinMaxFactory, 
			virtual public MyUtil::Singleton<FriendRankLoaderFactory>
		{
			public:
				Ice::ObjectPtr create(Ice::Int userId);
				virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
				virtual MyUtil::ObjectResultPtr create(Ice::Int minId, Ice::Int maxId);
			protected:
				friend class MyUtil::Singleton<FriendRankLoaderFactory>;
		};

		class FriendRankBatchResultHandler : virtual public com::xiaonei::xce::ResultHandler
		{
			public:
				FriendRankBatchResultHandler(MyUtil::ObjectResultPtr& results) : _results(results) {}
				virtual bool handle(mysqlpp::Row& row) const;
			private:
				MyUtil::ObjectResultPtr _results;
		};

		//***************************************************************************

		class FriendRankLoaderI : virtual public FriendRankLoader, virtual public MyUtil::Singleton<FriendRankLoaderI>
		{
			public:
				virtual void reload(::Ice::Int, const Ice::Current&);
			private:
				friend class MyUtil::Singleton<FriendRankLoaderI>;
		};

		class LoadTask : virtual public MyUtil::Task
		{
			public:
				LoadTask( ::Ice::Int id, ::Ice::Int level=TASK_LEVEL_FRIEND_RANK_LOADER_LOAD ) :
					Task(level),_id(id){
					}
			protected:
				virtual void handle();
			private:
				::Ice::Int _id;
		};
	}
}

#endif

