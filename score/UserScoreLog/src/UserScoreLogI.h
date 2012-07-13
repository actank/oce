#ifndef __USERSCORE_LOG_I_H__
#define __USERSCORE_LOG_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "UserScoreLog.h"

namespace xce {
namespace userscore {

const int LOGNOTE_DATA = 0;

const int TASK_LEVEL_ADDLOG = 1;
const int TASK_LEVEL_RECORD_BATCH = 2;

const int LIMIT_TIME = 60*24;

class LogNote : virtual public Ice::Object,
                virtual public IceUtil::Mutex {
public:
	void addLog(int userId,int type,int scoreAdd);
	void set(int type,int scoreAdd);
	MyUtil::Int2IntMap getLogs();
	MyUtil::Int2IntMap popLogs();
private:
	MyUtil::Int2IntMap _logs;
};
typedef IceUtil::Handle<LogNote> LogNotePtr;

class UserScoreResultHandler : virtual public com::xiaonei::xce::ResultHandler {
        public:
                UserScoreResultHandler( LogNotePtr& result) :
                        _result(result) {
                }
        protected:
                virtual bool handle(mysqlpp::Row&) const;
        private:
                LogNotePtr& _result;
};

class UserScoreLogFactory : virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

class UserScoreLogManagerI : virtual public UserScoreLogManager,
        virtual public MyUtil::Singleton<UserScoreLogManagerI> {
public:
	virtual void addLog(::Ice::Int userId,::Ice::Int type,::Ice::Int scoreAdd, const Ice::Current&);
private:
        friend class MyUtil::Singleton<UserScoreLogManagerI>;
};

class RecordHelper: public MyUtil::Singleton<RecordHelper> {
public:
        void initialize();
        void invokeDelay( Ice::Int, const Ice::ObjectPtr& );
        void invokeNow( Ice::Int, const Ice::ObjectPtr& );
        void swap(std::vector< pair<long,Ice::ObjectPtr> >& empty_vector);
	string getDate();
private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector< pair<long,Ice::ObjectPtr> > datas_;

        class RecordThread: virtual public IceUtil::Thread {
        public:
                RecordThread(RecordHelper& );
                virtual void run();
		string getDate();
        private:
                RecordHelper& helper_;
		string _today_str;
		long _today;
        };
        typedef IceUtil::Handle<RecordThread> RecordThreadPtr;

        RecordThreadPtr recorder_;
};

//***************************************************************************

class AddLogTask : virtual public MyUtil::Task {
public:
        AddLogTask( Ice::Int userId,
			Ice::Int type,
			Ice::Int scoreAdd,
			Ice::Int level=TASK_LEVEL_ADDLOG ) : Task(level),
		_userId(userId),
		_type(type),
		_scoreAdd(scoreAdd){
        }
protected:
        virtual void handle();
private:
	Ice::Int _userId;
	Ice::Int _type;
	Ice::Int _scoreAdd;
};

class RecordBatchTask: virtual public MyUtil::Task {
public:
        RecordBatchTask( const std::map<Ice::Int,MyUtil::Int2IntMap>& logMap,
		string& date, Ice::Int level=TASK_LEVEL_RECORD_BATCH ):
                	MyUtil::Task(level), _logMap(logMap), _date(date) {
        }
        virtual void handle();
private:
        std::map<Ice::Int,MyUtil::Int2IntMap> _logMap;
	string _date;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}
}

#endif
