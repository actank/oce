#ifndef _FRIEND_RANK_CACHE_I_H_
#define _FRIEND_RANK_CACHE_I_H_

#include <Ice/Ice.h>
#include <ServiceI.h>
#include <AdapterI.h>
#include <TaskManager.h>
#include "QueryRunner.h"
#include "FriendRank.h"

namespace xce {
	namespace socialgraph	{

		const int FRIEND_RANK_DATA = 0;
		const int TASK_LEVEL_FRIEND_RANK_CACHE_FILL = 1;
		const int TASK_LEVEL_FRIEND_RANK_CACHE_LOAD = 2;

		// struct RankDataComp {{{
		struct RankDataComp
		{
			bool operator()(const RankData& p1, const RankData& p2)
			{
				return p1.userid < p2.userid;
			}
		};// }}}

		// class FriendRankCacheI {{{
		class FriendRankCacheI :
			virtual public FriendRankCache,
			virtual public MyUtil::Singleton<FriendRankCacheI>
		{
			public:
				virtual RankSeq GetRank(int userId, const Ice::Current& cur);
				virtual map<int,int> GetRevRank(int hostId, const MyUtil::IntSeq& guestIds,
					 	const Ice::Current& cur);

				virtual void SetRank(int userId, const FriendRankDataPtr& data, 
						const Ice::Current& cur);
				virtual void SetRanks(const ::MyUtil::ObjectResultPtr& datas, 
						const Ice::Current& cur);

				virtual void Load(const RankSeqMap& datas, 
						const Ice::Current& cur);

				virtual ~FriendRankCacheI() {}
				virtual bool isValid(const Ice::Current&);
				virtual void setValid(bool valid,const Ice::Current& = Ice::Current());
				virtual void setData(const ::MyUtil::ObjectResultPtr& rankData, const Ice::Current&);
				void Bootstrap(const string& endpoint, int mod);

			private:
				FriendRankCacheI() {}

			private:
				friend class MyUtil::Singleton<FriendRankCacheI>;
				string _endpoint;
				int _mod;
				bool _isValid;
				IceUtil::RWRecMutex _validMutex;
		};// }}}

		// 这个类与Loader重复，目的是避免远程调用
		// class FriendRankSingleResultHandler {{{
		class FriendRankSingleResultHandler : virtual public com::xiaonei::xce::ResultHandler
		{
			public:
				FriendRankSingleResultHandler(const FriendRankDataPtr& result):_result(result) {}
				virtual ~FriendRankSingleResultHandler() {}
			protected:
				virtual bool handle(mysqlpp::Row& row) const;
			private:
				FriendRankDataPtr _result;
		};// }}}

		// class FriendRankFactoryI {{{
		class FriendRankFactoryI: public MyUtil::ServantFactory
		{
			public:
				virtual Ice::ObjectPtr create(Ice::Int userId);
		};// }}}

		// class FillTask; class LoadTask {{{
		class FillTask : public MyUtil::Task
		{
			public:
				FillTask(const MyUtil::ObjectResultPtr& rankData, 
						::Ice::Int level=TASK_LEVEL_FRIEND_RANK_CACHE_FILL):
					MyUtil::Task(level), _rankData(rankData) {}
				virtual void handle();
			private:
				MyUtil::ObjectResultPtr _rankData;
		};

		class LoadTask: virtual public MyUtil::Task
		{
			public:
				LoadTask(Ice::Int userId, Ice::Int level=TASK_LEVEL_FRIEND_RANK_CACHE_LOAD):
					MyUtil::Task(level), _userId(userId) {}
				virtual void handle();
			private:
				Ice::Int _userId;
		};// }}}

		// class ServiceSchemaI {{{
		class ServiceSchemaI: public MyUtil::Descriptor
		{
			public:
				virtual void configure(const Ice::PropertiesPtr& props);
		};// }}}
	}
}

#endif

