/*
 * =====================================================================================
 *
 *       Filename:  FeedItemCacheByAppidI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月22日 10时51分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "AppFeedsI.h"
#include "ServiceI.h"
#include "IceLogger.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::feed;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&AppFeedsI::instance(), service.createIdentity("M", ""));

	//Controller
	int interval = 5;
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedItemCache.Mod", 0);
	string controller = service.getCommunicator()->getProperties()->getPropertyWithDefault("FeedItemCache.Controller", "ControllerFeedItemCacheByAppid");
	ServerStateSubscriber::instance().initialize(controller, &AppFeedsI::instance(), mod, interval, new XceFeedControllerChannel());

	//初始化
	AppFeedsI::instance().initialize();
}

AppFeedsI::AppFeedsI() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	checker_limit_ = 400000000;
	checker_interval_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".CheckerInterval", 10);

	build_history_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".BuildHistory", 1);//单位为天
	build_interval_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".BuildInterval", 20);//单位为分钟

	context_prefix_ = props->getPropertyWithDefault("Service." + service.getName() + ".ContextPrefix", "AppId");

	checker_ = new CheckerTimerTask(fs_, checker_limit_);
	timer_.scheduleRepeated(checker_, IceUtil::Time::seconds(checker_interval_));
}

AppFeedsI::~AppFeedsI() {
}

void AppFeedsI::initialize() {
	TaskManager::instance().execute(new BuildFeedStreamTask(build_history_, build_interval_, context_prefix_));
}

void AppFeedsI::put(const FeedMeta& fm, const ::Ice::Current& current) {

	ostringstream oss;
	oss << "uid: " << fm.userid << " feedid: " << fm.feedid << " type: " << fm.type << " time: " << fm.time;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int appid = 0;
	//TODO 解析current规则
	map<string, string> context = current.ctx;
	map<string, string>::const_iterator it = context.find(context_prefix_);
	if (it != context.end()) {
		appid = boost::lexical_cast<int>((*it).second);
		if (appid > 0) {
			MCE_DEBUG("AppFeedsI::put, appid: " << appid << " feedid: " << fm.feedid << " userid: " << fm.userid);
			fs_.put(appid, fm);
		}
	}
}

FeedMetaSeq AppFeedsI::get(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "ids.size: " << ids.size() << " begin: " << begin << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return getSegment(fs_.gets(ids, begin + limit), ids, begin, limit);
}

FeedMetaSeq AppFeedsI::getByTypes(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "ids.size: " << ids.size() << " begin: " << begin << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	if(types.empty())
		return getSegment(fs_.gets(ids, begin + limit), ids, begin, limit);
	return getSegment(fs_.gets_with_filter(ids, PassFilter(types), PreventFilter(std::vector<string>()), begin + limit), ids, begin, limit);
}

FeedMetaSeq AppFeedsI::getWithFilter(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, const StrSeq& filter, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "ids.size: " << ids.size() << " begin: " << begin << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	if(filter.empty())
		return getSegment(fs_.gets(ids, begin + limit), ids, begin, limit);
	return getSegment(fs_.gets_with_filter(ids, PassFilter(std::vector<int>()), PreventFilter(filter), begin + limit), ids, begin, limit);
}

FeedMetaSeq AppFeedsI::getByTypesWithFilter(const IntSeq& ids, ::Ice::Int begin, ::Ice::Int limit, 
		const IntSeq& types, const StrSeq& filter, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "ids.size: " << ids.size() << " begin: " << begin << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	if(filter.empty() && types.empty())
		return getSegment(fs_.gets(ids, begin + limit), ids, begin, limit);
	return getSegment(fs_.gets_with_filter(ids, PassFilter(types), PreventFilter(filter), begin + limit), ids, begin, limit);
}

FeedMetaSeq AppFeedsI::getGreaterThanFeedid(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "appid: " << appid << " feedid: " << feedid << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	std::vector<FeedMetaData> tmp = fs_.getGreaterThanFeedid(appid, feedid, limit);

	std::vector<FeedMeta> result;
	result.reserve(tmp.size());
	for (vector<FeedMetaData>::iterator it = tmp.begin();
			it != tmp.end(); ++it) {
		FeedMeta meta = it->createFrom(it->userid);
		result.push_back(meta);
	}
	return result;
}

FeedMetaSeq AppFeedsI::getLessThanFeedid(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "appid: " << appid << " feedid: " << feedid << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	std::vector<FeedMetaData> tmp = fs_.getLessThanFeedid(appid, feedid, limit);

	std::vector<FeedMeta> result;
	result.reserve(tmp.size());
	for (vector<FeedMetaData>::iterator it = tmp.begin();
			it != tmp.end(); ++it) {
		FeedMeta meta = it->createFrom(it->userid);
		result.push_back(meta);
	}
	return result;
}

FeedMetaSeq AppFeedsI::getGreaterThanFeedidWithUseridByType(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, 
		::Ice::Int userid, const IntSeq& types, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "appid: " << appid << " feedid: " << feedid << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	IntSeq friends;
	if (userid >= 0) {
		friends = xce::buddy::adapter::BuddyByOnlineTimeCacheAdapter::instance().getFriendList(userid, -1);
		//if(friends.empty()) return std::vector<FeedMeta>(); 
	}


	ostringstream oss_friends;
	for (IntSeq::iterator it = friends.begin();
			it != friends.end(); ++it) {
		oss_friends << *it << " ";
	}

	MCE_DEBUG("AppFeedsI::getGreaterThanFeedidWithUseridByType, appid: " << appid << " userid: " << userid << "friends.size: " << friends.size() << " friends: " << oss_friends.str());
	std::vector<FeedMetaData> tmp = fs_.getGreaterThanFeedidWithFriendsByTypes(appid, feedid, limit, FriendFilter(friends), PassFilter(types));
	MCE_DEBUG("AppFeedsI::getGreaterThanFeedidWithUseridByType, appid: " << appid << " userid: " << userid << " tmp.size: " << tmp.size());

	std::vector<FeedMeta> result;
	result.reserve(tmp.size());
	for (vector<FeedMetaData>::iterator it = tmp.begin();
			it != tmp.end(); ++it) {
		FeedMeta meta = it->createFrom(it->userid);
		result.push_back(meta);
	}
	MCE_DEBUG("AppFeedsI::getGreaterThanFeedidWithUseridByType, appid: " << appid << " userid: " << userid << " result.size: " << result.size());
	return result;
}

void AppFeedsI::setLimit(::Ice::Long limit, const ::Ice::Current& current) {
	checker_->setLimit(limit);
}

::Ice::Int AppFeedsI::getUserCount(::Ice::Int appid, const ::Ice::Current& current) {
	return fs_.item_count(appid);
}

::Ice::Long AppFeedsI::getTotalCount(const ::Ice::Current& current) {
	return fs_.total_count();
}

void AppFeedsI::check_order() {
	fs_.check_order();
}

void AppFeedsI::add_item_count(int count) {
	fs_.add_count(count);
}

bool AppFeedsI::put_without_add_count(int id, const FeedMeta& meta) {
	MCE_DEBUG("AppFeedsI::put_without_add_count, appid: " << id << " feedid: " << meta.feedid << " userid: " << meta.userid);
	return fs_.put(id, meta, false);
}

FeedMetaSeq AppFeedsI::getSegment(const std::vector<std::vector<FeedMetaData> >& data, const IntSeq& userids, int begin, int limit) {
	int sum_size = 0;
	for(size_t i = 0; i < data.size(); ++i) {
		sum_size+=data[i].size();
	}

	FeedMetaSeq seq;
	seq.reserve(sum_size);

	for(size_t i = 0; i < userids.size(); ++i)
	{
		const std::vector<FeedMetaData>& v = data[i];
		int uid = userids[i];
		for(size_t j = 0; j < v.size(); ++j)
			seq.push_back(v[j].createFrom(uid));
	}
	if(begin >= (int)seq.size())
		return FeedMetaSeq();
	if(begin + limit > (int)seq.size())
		limit = seq.size() - begin;

	FeedMetaSeq::iterator it = seq.begin();
	std::nth_element(it, it + (begin + limit) - 1, seq.end(), FeedMetaCompare());
	if(limit > 1)
		std::nth_element(it, it + begin, it + (begin + limit) - 1, FeedMetaCompare());
	std::sort(it + begin, it + begin + limit, FeedMetaCompare());

	return FeedMetaSeq(it +begin, it + (begin+limit));
}

void CheckerTimerTask :: runTimerTask() {
	MCE_DEBUG("CheckerTimerTask::runTimerTask user: " << fs_.user_count()
			<< " total_count: " << fs_.total_count() << " limit: " << limit());

	while(fs_.total_count() > limit()) {
		FeedMetaData data;
		if(fs_.get_min(data)) {
			data.time += 60 * 60; //一次释放一个小时的数据，而不是一个一个释放，提高逐出效率
			long pre_total = fs_.total_count();
			fs_.free(data);
			MCE_INFO("CheckerTimeTask[evict the Item] timeline: " << data.time << " pre_total: " << pre_total <<
					" after free total: " << fs_.total_count());
		} else {
			MCE_WARN("can not get min");
		}
	}
}

void BuildFeedStreamTask::handle() {

	int history_time = time_now_ - history_*24*60*60;
	int interval = interval_*60;

	MCE_DEBUG("BuildFeedStreamTask::handle() is starting ..., time_now_: " << time_now_
			<< " history_: " << history_ << " history_time: " << history_time
			<< " interval_: " << interval_ << " interval: " << interval);

	std::vector<BuildFeedStreamThreadPtr> threads;
	threads.reserve(DB_TABLE_COUNT);

	int cpu_size = (int)(sysconf(_SC_NPROCESSORS_CONF) * 1.5);
	MCE_INFO("machine cpu size: " << cpu_size);

	for (int index = 0; index < DB_TABLE_COUNT; index++) {
		BuildFeedStreamThreadPtr thread = new BuildFeedStreamThread(history_time, interval, index, time_now_, cpu_size, context_prefix_);
		threads.push_back(thread);
		thread->start();
	}

	for (int index = 0; index < DB_TABLE_COUNT; index++) {
		threads[index]->getThreadControl().join();
	}

	//重整
	AppFeedsI::instance().check_order();

	//加载完成，设置可用状态
	ServerStateSubscriber::instance().setStatus(1);
}

void BuildFeedStreamThread::run() {
	MCE_DEBUG("BuildFeedStreamThread " << index_ << " is starting ..., history: " << history_
			<< " interval_: " << interval_);

	for (int begin = time_now_, end = begin - interval_; begin >= history_; begin = end, end = begin - interval_) {
		ostringstream oss;
		oss << "BuildFeedStreamThread " << index_ << " begin: " << begin << " end: " << end;
		TimeCost tm = TimeCost::create(oss.str());

		int sleep_second = 0;
		while (run_count() > cpu_count_) {
			sleep_second ++;
			sleep(1);
		}

		if (sleep_second > 0) {
			MCE_DEBUG("BuildFeedStreamThread sleep count: " << sleep_second);
		}

		run_count_inc();

		std::map<int, std::vector<FeedMeta> > data = PersistenceHelper::read(index_, begin, end, context_prefix_) -> get_metas();

		int count = 0;
		for (std::map<int, std::vector<FeedMeta> >::const_iterator it = data.begin();
				it != data.end(); ++it) {
			
			for(std::vector<FeedMeta>::const_iterator ir = it->second.begin();
					ir != it->second.end(); ++ir) {
				if (AppFeedsI::instance().put_without_add_count(it->first, (*ir)) ) {
					count ++;
				}
			}
		}

		AppFeedsI::instance().add_item_count(count);

		run_count_dec();
	}
}
