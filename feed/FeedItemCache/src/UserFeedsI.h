/*
 * UserFeedsI.h
 *
 *  Created on: Aug 31, 2011
 *      Author: happy, guanghe.ge@renren-inc.com
 */

#ifndef __USERFEEDS_I_H__
#define __USERFEEDS_I_H__

#include <set>
#include "UserFeeds.h"
#include "FeedStream.h"
#include "Singleton.h"
#include "TaskManager.h"
#include <boost/lexical_cast.hpp>
#include "../persistence/PersistenceHelper.h"
#include "util/cpp/String.h"

namespace xce {
namespace feed {

using namespace std;
using namespace xce::feed;
using namespace MyUtil;

const int TASK_LEVEL_BUILDUSERMAP = 1;
const long ITEM_COUNT_LIMIT = 300000000;

struct WeiredType {
  unsigned type       : 16;
  unsigned version    : 8;
  unsigned reversed0  : 2;
  unsigned real_type  : 2;  
  unsigned mini_merge : 2;
  unsigned news_merge : 2;

  WeiredType(unsigned int v) {
    BOOST_STATIC_ASSERT(sizeof(WeiredType) == sizeof(int));
     type = v&0xffff;
     v >>= 16;
     version = v & 0xff;
     v >>= 8;
     reversed0 = v & 0x3;
     v >>= 2;
     real_type = v & 0x3;
     v >>= 2;
     mini_merge = v & 0x3;
     v >>= 2;
     news_merge = v & 0x3;
  }
};

struct FeedMetaData {
	FeedMetaData() : userid(0), feedid(0), type(0), time(0), weight(0), mergeid(0) {}
	FeedMetaData(const FeedMeta& fm) : userid(fm.userid), feedid(fm.feedid), type(fm.type), time(fm.time)
   							,weight(fm.weight), mergeid(fm.mergeid) {}

	FeedMeta createFrom(int id) const {
		FeedMeta fm;
		fm.userid = userid;
		fm.feedid = feedid;
		fm.type = type;
		fm.time = time;
		fm.weight = weight;
		fm.mergeid = mergeid;
		return fm;
	}

	bool operator<(const FeedMetaData& other) const {
		int t = time - other.time;
		if(t == 0)
			return feedid < other.feedid;
		return t < 0;
	}

	bool operator==(const FeedMetaData& other) const {
		return 0 == memcmp(this, &other, sizeof(FeedMetaData));
	}

	::Ice::Int userid;
	::Ice::Long feedid;
	::Ice::Int type;
	::Ice::Int time;
	::Ice::Int weight;
	::Ice::Long mergeid;
};

struct TypesFilter {
	std::set<int> types;
	TypesFilter(const std::vector<int>& _types) : types(_types.begin(), _types.end()) {};

	inline bool operator()(const FeedMetaData& data){
		return types.empty() || 
			types.find(data.type) != types.end() || 
			types.find(WeiredType(data.type).type) != types.end() ||
			types.find(int(WeiredType(data.type).type/100)*100) != types.end();
	}
};

struct IdsFilter {
	std::map<int, std::set<long> > ids;
	IdsFilter(const std::vector<std::string>& _ids) {
		for (std::vector<std::string>::const_iterator it = _ids.begin();
				it != _ids.end(); ++it) {
			std::vector<string> tmp = MyUtil::StrUtil::split((*it), ":");
			if (tmp.size() == 2) {
				int type_id = boost::lexical_cast<int>(tmp[0]);
				int filter_id = boost::lexical_cast<long>(tmp[1]);
				std::map<int, std::set<long> >::iterator it = ids.find(type_id);
				if (it == ids.end()) {
					std::set<long> filter;
					ids[type_id] = filter;
				}
				ids[type_id].insert(filter_id);
			}
		}
	};

	inline bool operator()(const FeedMetaData& data){
		if (ids.empty()) return false;
		bool result = false;
		for (std::map<int, std::set<long> >::iterator it = ids.begin();
				it != ids.end(); ++it) {
				std::set<long> feedids = ids[FeedidPrefix];
				std::set<long> userids = ids[UseridPrefix];
				std::set<long> types = ids[TypePrefix];
			switch(it->first) {
				case ::xce::feed::FeedidPrefix:
					result = result || feedids.find(data.feedid) != feedids.end();
					break;
				case ::xce::feed::UseridPrefix:
					result = result || userids.find(data.userid) != userids.end();
					break;
				case ::xce::feed::TypePrefix:
					result = result || types.find(data.type) != types.end() || 
						types.find(WeiredType(data.type).type) != types.end() || 
						types.find(int(WeiredType(data.type).type/100)*100) != types.end() ||
						types.find(int(WeiredType(data.type).type/100)) != types.end();
					break;
				default:
					result = result || false;
			}
		}
	return result;
	}
};

struct FeedMetaCompare {
	inline bool operator()(const FeedMeta& a, const FeedMeta& b) {
		int d = a.time - b.time;
		if(d == 0)
			return a.feedid > b.feedid;
		return d > 0;
	}
};

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

class BuildUserMapThread : virtual public IceUtil::Thread {
public:
	BuildUserMapThread(int history, int interval, int index, int time_now, int cpu_count) : history_(history), interval_(interval), index_(index), time_now_(time_now), cpu_count_(cpu_count) {}

protected:
	virtual void run();
private:
	int history_;
	int interval_;
	int index_;
	int time_now_;
	int cpu_count_;

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
typedef IceUtil::Handle<BuildUserMapThread> BuildUserMapThreadPtr;
atomic_t BuildUserMapThread::run_count_ = ATOMIC_INIT(0); 

class BuildUserMapTask : virtual public MyUtil::Task {
public:
	BuildUserMapTask(int history, int interval) :
	   MyUtil::Task(TASK_LEVEL_BUILDUSERMAP), history_(history), interval_(interval), time_now_(time(NULL)) {}
	~BuildUserMapTask() {}
protected:
	virtual void handle();
private:
	int history_;
	int interval_;

	int time_now_;
};

class UserFeedsI : public UserFeeds, public MyUtil::Singleton<UserFeedsI> {

public:
	UserFeedsI();
	virtual ~UserFeedsI();
	void initialize();

    virtual void put(const FeedMeta& meta, const ::Ice::Current& = ::Ice::Current());
    virtual FeedMetaSeq get(const IntSeq& userIds, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& = ::Ice::Current());
    virtual FeedMetaSeq getByTypes(const IntSeq& userIds, ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq getWithFilter(const IntSeq& userIds, ::Ice::Int begin, ::Ice::Int limit, const StrSeq& filter, const ::Ice::Current& = ::Ice::Current());
	virtual FeedMetaSeq getByTypesWithFilter(const IntSeq& userIds, ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const StrSeq& filter, const ::Ice::Current& = ::Ice::Current());

	virtual FeedMetaSeq getLessThanFeedidByTypesWithFilter(const IntSeq& userIds, ::Ice::Long feedid, ::Ice::Int limit, const IntSeq& types, const StrSeq& filter, const ::Ice::Current& = ::Ice::Current());

	virtual void setLimit(::Ice::Long limit, const ::Ice::Current& = ::Ice::Current());
	virtual ::Ice::Int getUserCount(::Ice::Int userId, const ::Ice::Current& = ::Ice::Current());
	virtual ::Ice::Long getTotalCount(const ::Ice::Current& = ::Ice::Current());

	void check_order();
	void add_item_count(int count);
	bool put_without_add_count(const FeedMeta& meta);
	bool puts_without_add_count(int mod, const std::vector<FeedMetaData>& feeds);

private:
	FeedMetaSeq getSegment(const std::vector< std::vector<FeedMetaData> >& data, const IntSeq& userids, int begin, int limit);

    CheckerTimerTaskPtr checker_;
	IceUtil::Timer timer_;
    FeedStream<FeedMetaData> fs_;

	int checker_limit_;
	int checker_interval_;

	int build_history_;
	int build_interval_;
};

}//end feed
}//end xce

#endif /* USERFEEDSI_H_ */
