#ifndef __FRIENDFINDER_NEWCOMER_LOADER_I_H__
#define __FRIENDFINDER_NEWCOMER_LOADER_I_H__

#include "FriendFinderNewComer.h"
#include "newcomercachedataI.h"
#include "socialgraph/socialgraphutil/socialgraphworkerI.h"
#include "socialgraph/socialgraphutil/mcclient.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <QueryRunner.h>

namespace xce {
namespace socialgraph {
namespace friendfindernewcomer {

	const static int TASK_LEVEL_REPORT = 1;
	const static int TASK_LEVEL_REBUILD = 2;
	const static int TASK_LEVEL_PRELOAD = 3;

	const static int TASK_LEVEL_APPLY = 4;
	const static int TASK_LEVEL_ACCEPT = 5;
	const static int TASK_LEVEL_DENY = 6;
	const static int TASK_LEVEL_IGNORE = 7;

	const static int TASK_LEVEL_ADD = 8;
	const static int TASK_LEVEL_SHOOT = 9;
	const static int TASK_LEVEL_UPDATE_INFO = 10;

	//**************************************************************************************************************

	class FriendFinderNewComerWorkerI:
			//virtual public FriendFinderNewComerWorker,
			virtual public xce::socialgraph::SocialGraphWorkerI,
			public MyUtil::Singleton<FriendFinderNewComerWorkerI>{
	private:
		friend class MyUtil::Singleton<FriendFinderNewComerWorkerI>;

	public:
		virtual void Report(const Info&, const Ice::Current&);
		virtual void Rebuild(int, const Ice::Current&);
		virtual void PreLoad(int,bool, const Ice::Current&);

	public:
                void apply(::Ice::Int, ::Ice::Int);
                void accept(::Ice::Int, ::Ice::Int);
                void deny(::Ice::Int, ::Ice::Int);
                void ignore(::Ice::Int, const MyUtil::IntSeq&);

		void addNewComer(::Ice::Int);
		void addNewComerWithTime(::Ice::Int,::Ice::Int);

		void updateInfo(::Ice::Int);
	public:
		RmInfo remove(::Ice::Int hostId, ::Ice::Int guestId);
		void addDb(int userId,int type,const NewComerDbData& obj);
		void removeDb(int userId,const MyUtil::Int2IntSeqMap& ids);
		NewComerDbDataMap readDb(int userId);
		void writeMc(int userId,const vector<unsigned char>& objs);
		NewComerCacheDataIPtr locateNewComerCacheDataI(int userId);
	};

	//**************************************************************************************************************

	class ReportTask: public MyUtil::Task{
	public:
		ReportTask(Info info, int level=TASK_LEVEL_REPORT):
			Task(level), _info(info){
			};
		virtual void handle();

	private:
		Info _info;
	};

	class RebuildNewComerTask: public MyUtil::Task{
	public:
		RebuildNewComerTask(Ice::Int userId, int level=TASK_LEVEL_REBUILD):
			Task(level), _userId(userId){
			};
		virtual void handle();

	private:
		Ice::Int _userId;
	};

	class PreloadTask: public MyUtil::Task{
	public:
		PreloadTask(bool signal, int level=TASK_LEVEL_PRELOAD):
			Task(level), _signal(signal){
			};
		virtual void handle();

	private:
		MyUtil::IntSeq _signal;
	};

	//**************************************************************************************************************

	class ApplyTask: public MyUtil::Task{
	public:
		ApplyTask(Ice::Int applicant, Ice::Int acceptor, int level=TASK_LEVEL_APPLY):
			Task(level), _applicant(applicant), _acceptor(acceptor){
			};
		virtual void handle();

	private:
		Ice::Int _applicant;
		Ice::Int _acceptor;
	};

	class AcceptTask: public MyUtil::Task{
	public:
		AcceptTask(Ice::Int acceptor, Ice::Int applicant, int level=TASK_LEVEL_ACCEPT):
			Task(level), _acceptor(acceptor), _applicant(applicant){
			};
		virtual void handle();

	private:
		Ice::Int _acceptor;
		Ice::Int _applicant;
	};

	class DenyTask: public MyUtil::Task{
	public:
		DenyTask(Ice::Int hostId, Ice::Int guestId, int level=TASK_LEVEL_DENY):
			Task(level), _hostId(hostId), _guestId(guestId){
			};
		virtual void handle();

	private:
		Ice::Int _hostId;
		Ice::Int _guestId;
	};

	class IgnoreTask: public MyUtil::Task{
	public:
		IgnoreTask(Ice::Int userId, const MyUtil::IntSeq& ids, int level=TASK_LEVEL_IGNORE):
			Task(level), _userId(userId), _ids(ids){
			};
		virtual void handle();

	private:
		Ice::Int _userId;
		MyUtil::IntSeq _ids;
	};

	class AddNewComerTask: public MyUtil::Task{
	public:
		AddNewComerTask(Ice::Int userId, int registerTime, int level=TASK_LEVEL_ADD):
			Task(level), _userId(userId), _registerTime(registerTime){
			};
		virtual void handle();

	private:
		Ice::Int _userId;
		int _registerTime;
	};

	class ShootNewComerTask: public MyUtil::Task{
	public:
		ShootNewComerTask(NewComerDbData& obj, MyUtil::Int2IntSeqMap& hosts, int level=TASK_LEVEL_SHOOT):
			Task(level), _hosts(hosts){
				_obj.userId = obj.userId;
				_obj.time = obj.time;
			};
		virtual void handle();

	private:
		NewComerDbData _obj;
		MyUtil::Int2IntSeqMap _hosts;
	};

	class UpdateInfoTask: public MyUtil::Task{
	public:
		UpdateInfoTask(Ice::Int userId, int level=TASK_LEVEL_UPDATE_INFO):
			Task(level), _userId(userId){
			};
		virtual void handle();

	private:
		Ice::Int _userId;
	};

	//**************************************************************************************************************

	class RegisterTimeHandler : public com::xiaonei::xce::ResultHandler {
		public:
			RegisterTimeHandler(int& registerTime): _registerTime(registerTime) {}
		protected:
			virtual bool handle(mysqlpp::Row& row) const;
		private:
			int& _registerTime;
	};

	class NewComerCacheIResultHandler : virtual public com::xiaonei::xce::ResultHandler {
		public:
			NewComerCacheIResultHandler(NewComerDbDataMap& res): _res(res){}
		protected:
			virtual bool handle(mysqlpp::Row& row ) const;
		private:
			NewComerDbDataMap& _res;
	};

	//**************************************************************************************************************

	class ServiceSchemaI: public MyUtil::Descriptor {
		public:
			virtual void configure(const Ice::PropertiesPtr& props);
	};

	//**************************************************************************************************************

}
}
}

#endif
