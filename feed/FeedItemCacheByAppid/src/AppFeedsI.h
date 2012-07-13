/*
 * =====================================================================================
 *
 *       Filename:  FeedItemCacheByAppidI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月21日 18时40分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __APPFEEDS_I_H__
#define __APPFEEDS_I_H__

#include "UserFeeds.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "../include/FeedFilter.hpp"
#include "../include/FeedStream.hpp"
#include "../include/DBHelper.hpp"

namespace xce {
namespace feed {

using namespace MyUtil;

const int TASK_LEVEL_BUILDFEEDSTREAM = 1;

class CheckerTimerTask : public IceUtil::TimerTask {

public:
	CheckerTimerTask(FeedStream<FeedMetaData>& fs,	long limit) : fs_(fs), limit_(limit) {}

	virtual void runTimerTask();

	long limit() {
		IceUtil::RWRecMutex::RLock lock(limit_mutex_);
		return limit_;
	}

	void setLimit(long limit) {
		IceUtil::RWRecMutex::WLock lock(limit_mutex_);
		limit_ = limit;
	}
private:
	FeedStream<FeedMetaData>& fs_;
	IceUtil::RWRecMutex limit_mutex_;
	long limit_;
};
typedef IceUtil::Handle<CheckerTimerTask> CheckerTimerTaskPtr;

class BuildFeedStreamThread : virtual public IceUtil::Thread {
public:
	BuildFeedStreamThread(int history, int interval, int index, int time_now, int cpu_count, string context_prefix) : history_(history), interval_(interval), index_(index), time_now_(time_now), cpu_count_(cpu_count), context_prefix_(context_prefix) {}

protected:
	virtual void run();
private:
	int history_;
	int interval_;
	int index_;
	int time_now_;
	int cpu_count_;
	string context_prefix_;

	static atomic_t run_count_;
	static int run_count() {
		return atomic_read(&run_count_);
	}

	static void run_count_inc() {
		atomic_inc(&run_count_);
	}

	static void run_count_dec() {
		atomic_dec(&run_count_);
	}
};
typedef IceUtil::Handle<BuildFeedStreamThread> BuildFeedStreamThreadPtr;
atomic_t BuildFeedStreamThread::run_count_ = ATOMIC_INIT(0); 

class BuildFeedStreamTask : virtual public MyUtil::Task {
public:
	BuildFeedStreamTask(int history, int interval, string context_prefix) :
	   MyUtil::Task(TASK_LEVEL_BUILDFEEDSTREAM), history_(history), interval_(interval),context_prefix_(context_prefix), time_now_(time(NULL)) {}
	~BuildFeedStreamTask() {}
protected:
	virtual void handle();
private:
	int history_;
	int interval_;
	string context_prefix_;

	int time_now_;

};

class AppFeedsI : public AppFeeds, public MyUtil::Singleton<AppFeedsI> {
public:
	AppFeedsI();
	virtual ~AppFeedsI();
	void initialize();

	virtual void put(const FeedMeta& meta, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq get(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq getByTypes(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq getWithFilter(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const StrSeq& filter, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq getByTypesWithFilter(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const StrSeq& filter, const ::Ice::Current& = ::Ice::Current());

	virtual FeedMetaSeq getGreaterThanFeedid(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq getLessThanFeedid(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, const ::Ice::Current& = ::Ice::Current());

	virtual FeedMetaSeq getGreaterThanFeedidWithUseridByType(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit,
			::Ice::Int userid, const IntSeq& types, const ::Ice::Current& = ::Ice::Current());

	virtual void setLimit(::Ice::Long limit, const ::Ice::Current& = ::Ice::Current());
	virtual ::Ice::Int getUserCount(::Ice::Int appId, const ::Ice::Current& = ::Ice::Current());
	virtual ::Ice::Long getTotalCount(const ::Ice::Current& = ::Ice::Current());

	
	void check_order();
	void add_item_count(int count);
	bool put_without_add_count(int id, const FeedMeta& meta);
private:
	FeedMetaSeq getSegment(const std::vector< std::vector<FeedMetaData> >& data, const IntSeq& ids, int begin, int limit);

    CheckerTimerTaskPtr checker_;
	IceUtil::Timer timer_;
    FeedStream<FeedMetaData> fs_;

	int checker_limit_;
	int checker_interval_;

	int build_history_;
	int build_interval_;

	string context_prefix_;
};

}
}

#endif
