#ifndef __ICETHREAD_H__
#define __ICETHREAD_H__
#include <Ice/Ice.h>
#include <IceUtil/Thread.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/RWRecMutex.h>
#include "XceLoggerAdapter.h"
#include <vector>

using std::string;

namespace xce {
namespace eva {
namespace collect {

const int BUCKET_SIZE = 10;
const int BATCH_SIZE = 10000;

/* ----------------------------------------------
 * @Part I. 结构体定义
 * -------------------------------------------- */
struct LogTail {
public:
	LogTail(const std::string& n, long t, int r, bool is):
                 name(n),time(t),responseTime(r),isException(is) {
        };

  	std::string name;
	long time;
	int responseTime;
	bool isException;
};

struct LogCount {
	LogCount(const LogTail& p)
		:time(p.time),responseTime(p.responseTime),isException(p.isException) {
	};
	long time;
	int responseTime;
	bool isException;
};

/* ---------------------------------------------  
 * @Part II. 各类统计需求
 * ------------------------------------------- */
class StatObject : virtual public Ice::Object {
public:
	StatObject() {
	};
	void output(time_t timeDur, const string& serviceName, const string& operaName, const std::vector<LogCount>& logVector, com::xiaonei::service::XceLoggerPtr logger, const std::vector<std::string>& loggerCategories);

	virtual bool stat(string& statMsg, std::map<std::string, std::vector<int> >& statMsgMap, const std::vector<LogCount>& logVector)=0;
};
typedef IceUtil::Handle<StatObject> StatObjectPtr;

class StatAvgByTime : virtual public StatObject {
public:
	StatAvgByTime() {
	};
	virtual bool stat(string& statMsg, std::map<std::string, std::vector<int> >& statMsgMap, const std::vector<LogCount>& logVector);
};

class StatTopPart : virtual public StatObject {
public:
	StatTopPart(){
	};
	virtual bool stat(string& statMsg, std::map<std::string, std::vector<int> >& statMsgMap, const std::vector<LogCount>& logVector);
	int getIndex(int record);
	int getRecord(int index);
	
private:
//	static int _recordSeq[34];  //0....20,30,40,50,60,70,80,90,100,150,200,250,300+,logSize
	static std::map<string, std::vector<int> > _recordMap;
	static int _timeLast;
//	static int _logSize;
	static IceUtil::Mutex _mapMutex;
	static IceUtil::Mutex _timeMutex;
};

//int StatTopPart::_logSize = 0;
std::map<string, std::vector<int> > StatTopPart::_recordMap;
int StatTopPart::_timeLast = time(NULL);
//int StatTopPart::_recordSeq[33]={0};


/*class StatThreadPool : virtual public StatObject {
public:
        StatThreadPool(){
        };
        // void output(const string& serviceName, const std::vector<LogCount>& logVector, com::xiaonei::service::XceLoggerPtr logger, const std::vector<std::string>& loggerCategories);
        virtual bool stat(string& statMsg, const std::vector<LogCount>& logVector);
};*/

/* ------  总的统计类 ---------------------- */
class Stat {
public:
	Stat(const std::vector<LogTail>& tail) 
		: tails_(tail) 	{
		// I. Average/Max/Min/Timeout
		StatObjectPtr statAvgByTime = new StatAvgByTime();
		statObjects_.push_back(statAvgByTime);
		// II. ThreadPool
		//StatObjectPtr statThreadPool = new StatThreadPool();
                //statObjects_.push_back(statThreadPool);
		// III. TopPart(暂不开放)
		StatObjectPtr statTopPart = new StatTopPart();
		statObjects_.push_back(statTopPart);
	};
	void merge();
	void output(time_t timeDur, com::xiaonei::service::XceLoggerPtr logger, const std::vector<std::string>& loggerCategories);

private:
	const std::vector<LogTail>& tails_;
	std::map<std::string, std::vector<LogCount> > logMap_;
	std::vector<StatObjectPtr> statObjects_;
};

/* ------------------------------------------
 * @Part III. 统计线程
 * --------------------------------------- */
class EvaCollector;
typedef IceUtil::Handle<EvaCollector> EvaCollectorPtr;

class LogHolder;
typedef IceUtil::Handle<LogHolder> LogHolderPtr;

class LogHolder : public IceUtil::Shared {
public:
  LogHolder(com::xiaonei::service::XceLoggerPtr log) : logger_(log) {}
  void initialize(int suffix);
  void add(const LogTail&);
  size_t size();
  void swap(std::vector<LogTail>& );

private:
  std::vector<LogTail> buffer_;
 // std::vector<std::string> blackwords_;	// 无效的服务特征字串组
  // IceUtil::Mutex mutex_;
  IceUtil::Monitor<IceUtil::Mutex> mutex_;
  com::xiaonei::service::XceLoggerPtr logger_;

  class OutputThread : public IceUtil::Thread {
    public:
      OutputThread(LogHolder& holder, int suffix, IceUtil::Monitor<IceUtil::Mutex>& mutex) :
        holder_(holder), suffix_(suffix), mutex_(mutex) {}
      
      virtual void run();

    private:
      LogHolder& holder_;
      int suffix_;
      IceUtil::Monitor<IceUtil::Mutex>& mutex_;
  };
  typedef IceUtil::Handle<OutputThread> OutputThreadPtr;

  OutputThreadPtr output_;
};

class EvaCollector : virtual public Ice::Object {
public : 
  	EvaCollector() {
 	}
	static EvaCollector& instance();
  	void initialize(string loggerEndpoints="M@EvaLogger");  
	void collect(const std::string& serviceName, const std::string& operaName, const long time, const int responseTime, bool isException);	
  
private :
  	LogHolderPtr findHolder();  

//	std::vector<std::string> blackwords_; // 无效的服务特征字串组
	std::vector<LogHolderPtr> holders_;
	com::xiaonei::service::XceLoggerPtr logger_;
//  	com::xiaonei::service::XceLoggerAdapter logAdapter_;

	static IceUtil::Handle<EvaCollector>  collector_;
	static IceUtil::Mutex mutex_;
	//static IceUtil::Mutex _mapMutex;
	//static IceUtil::Mutex _timeMutex;
};  
typedef IceUtil::Handle<EvaCollector> EvaCollectorPtr;

}
}
}

#endif
