#ifndef _FRIEND_RANK_UPDATE_I_H_
#define _FRIEND_RANK_UPDATE_I_H_

#include <Ice/Ice.h>
#include <ServiceI.h>
#include <AdapterI.h>
#include <TaskManager.h>
#include "FriendRankUpdate.h"

namespace xce {
	namespace socialgraph	{

		const int TASK_LEVEL_USERMODEL = 1;
		const int TASK_LEVEL_USERMODEL_WORKER = 2;
		const int TASK_LEVEL_FRIENDRANK = 3;

		// class FriendRankUpdateI {{{
		class FriendRankUpdateI :
			virtual public FriendRankUpdate,
			virtual public MyUtil::Singleton<FriendRankUpdateI>
		{
			public:
				virtual ~FriendRankUpdateI() {}
				virtual int TriggerUserModel(const Ice::Current& cur);
				virtual int TriggerFriendRank(const Ice::Current& cur);

				int GetUserModel();
				int GetFriendRank();
				void IncrFriendRank();
				void DescFriendRank();
				void IncrUserModel();
				void DescUserModel();

				void Bootstrap(const string& endpoint, int mod);
				int split(const char* s, vector<string>& list, const char *delim);
				long GetFreeMemory();

			private:
				FriendRankUpdateI() {}

			private:
				friend class MyUtil::Singleton<FriendRankUpdateI>;
				string _endpoint;
				int _mod;
				int _usermodel;   // 0表示空闲，n表示n个工作者正在运行
				int _friendrank;  // 0表示空闲，1表示忙
				IceUtil::RWRecMutex _usermodelMutex;
				IceUtil::RWRecMutex _friendrankMutex;

		};// }}}

		class UserModelTask : public MyUtil::Task
		{
			public:
				UserModelTask(FriendRankUpdateI* pUpdate, 
						::Ice::Int level=TASK_LEVEL_USERMODEL):
					MyUtil::Task(level), 
					_pUpdate(pUpdate) {}
				virtual void handle();
			private:
				void SavePassport(FILE* fp);
			private:
				FriendRankUpdateI* _pUpdate;
		};
		class UserModelWorkerTask : public MyUtil::Task
		{
			public:
				UserModelWorkerTask(FriendRankUpdateI* pUpdate, long WORKER, long WORKNUM, long STEP, 
						::Ice::Int level=TASK_LEVEL_USERMODEL_WORKER):
					MyUtil::Task(level), 
					_pUpdate(pUpdate), _WORKER(WORKER), _WORKNUM(WORKNUM), _STEP(STEP) {}
				virtual void handle();
			private:
				FriendRankUpdateI* _pUpdate;
				long _WORKER;
				long _WORKNUM;
				long _STEP;
		};
		class FriendRankTask : public MyUtil::Task
		{
			public:
				FriendRankTask(FriendRankUpdateI* pUpdate, ::Ice::Int level=TASK_LEVEL_FRIENDRANK):
					MyUtil::Task(level), _pUpdate(pUpdate)	{}
				virtual void handle();
			private:
				FriendRankUpdateI* _pUpdate;
		};
	}
}

#endif

