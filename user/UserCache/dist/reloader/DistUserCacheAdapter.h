#ifndef __DIST_USERCACHE_ADAPTER_H__
#define __DIST_USERCACHE_ADAPTER_H__

#include "util/cpp/TimeCost.h"
#include "ServiceI.h"
#include "distcache/DistCacheAdapter.h"
#include "UserCache.pb.h"
#include "UserCache.h"
#include "user/UserCache/share/UserCacheDataI.h"
#include <sys/time.h>
#include <set>
#include <map>
#include "UserBaseAdapter.h"
#include <vector> 
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "OnlineBitmapAdapter.h"
#include "WapOnlineAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>
#include "OnlineEventMediator.h"
#include "StatusCacheAdapter.h"

namespace xce
{
namespace distcache
{
namespace usercacheadapter
{

using namespace xce::statuscache;
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;
using namespace com::xiaonei::wap::adapter;
using namespace com::xiaonei::xce;
using xce::adapter::userbase::UserBaseFullViewPtr;
using xce::adapter::userbase::UserBaseAdapter;
using talk::onlinebitmap::OnlineBitmapAdapter;
using talk::online::UserOnlineTypeSeq;
using xce::serverstate::ServerStateSubscriber;
using namespace MyUtil;
using namespace xce::usercache;
using namespace xce::distcache::usercache;
using namespace com::xiaonei::xce;
using namespace mysqlpp;
using namespace std;
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;

using namespace std;
const int GROUP_SIZE = 100;
const int OPER_INTERVAL_TIME = 2000;		

const int WEBPAGER_ONLINE = 2;
const int TALK_ONLINE = 4;

class UserCacheMemData : public xce::distcache::usercache::UserCacheData, public Ice::Object {
public:
	UserCacheMemData() : _cas(0), in_tables_(0) {
	}
				
	UserCacheMemData(const Ice::ObjectPtr data) {
		xce::usercache::UserCacheDataIPtr source = xce::usercache::UserCacheDataIPtr::dynamicCast(data);
		set_id(source->id);
		set_status(source->status);
		set_univ(source->univ);
		set_state(source->state);
		set_level(source->level);
		set_gender(source->gender);
		set_online(source->online);
		set_name(source->name);
		set_tinyurl(source->tinyUrl);
		_cas = 0;
		in_tables_ = 0;
	}
	
	void setUserPassportProperties(mysqlpp::Row& row, const char* fieldStatus) {
		set_status((int)row[fieldStatus]);
	}

	void setStateProperties(mysqlpp::Row& row, const char* fieldState, const char* fieldLevel) {
		set_state((int)row[fieldState]);
		set_level((int) row[fieldLevel]);
		in_tables_++;
	}

	void setUrlProperties(mysqlpp::Row& row, const char* fieldTinyUrl) {
#ifndef NEWARCH
		set_tinyurl(row[fieldTinyUrl].get_string());
#else
		set_tinyurl(row[fieldTinyUrl].c_str());
#endif
		in_tables_++;
	}
			
	void setNamesProperties(mysqlpp::Row& row, const char* fieldName) {
#ifndef NEWARCH
		set_name(row[fieldName].get_string());
#else
		set_name(row[fieldName].c_str());
#endif
		in_tables_++;
	}

	void setBornProperties(mysqlpp::Row& row, const char* fieldGender) {
		string tmpgender;
#ifndef NEWARCH
		tmpgender = row[fieldGender].get_string();
#else
		tmpgender = row[fieldGender].c_str();
#endif
		if(tmpgender == "男生") {
			set_gender(0x0f & 1);
		} else if(tmpgender == "女生") {
			set_gender(0x0f & 2);
		} else {
			set_gender(0x0f & 0);
		}
		in_tables_++;
	}

	void setStageProperties(mysqlpp::Row& row, const char* fieldUniv) {
		set_univ((int) row[fieldUniv]);
		//stage = (int) row[fieldStage];
		in_tables_++;
	}

	uint64_t _cas;
	int in_tables_;
};

typedef IceUtil::Handle<UserCacheMemData> UserCacheMemDataPtr;
	
class DistUserCacheAdapter : public MyUtil::Singleton<DistUserCacheAdapter> {
public:
	void initialize() {
		//while(xce::distcache::DistCacheAdapter::connect(distCacheAdapterPtr, "TestUserCache", "xcetest1:2181,xcetest2:2181,xcetest3:2181,xcetest4:2181,xcetest5:2181/DistCache") == false) {
		while((distCacheAdapterPtr = xce::distcache::DistCacheAdapter::connect("UserCache", "")) == NULL) {
			MCE_WARN("DistUserCacheAdapter::initialize can not init DistCacheAdapter!");
		}
		last_oper_time_.resize(GROUP_SIZE);
		time_to_userid.resize(GROUP_SIZE);
	}

	static void logOut(const string& funStr, const UserCacheMemDataPtr data, bool isDebug = true ) {
		ostringstream os;
		os << funStr << " ";
		os << "Detail :" << " id = " << data->id() << " status = " << data->status() << " univ = " << data->univ()
			<< " state = " << data->state() << " level = " << data->level() << " gender = " << data->gender()
			<< " online = " << data->online() << " name = " << data->name() << " tinyurl = " << data->tinyurl()
			<< " cas = " << data->_cas;
		if( isDebug )
		{
			MCE_DEBUG(os.str());
		}else
		{
			MCE_INFO(os.str());
		}
	}
					
	vector<long> setData(MyUtil::ObjectResultPtr res) {
		map<long, UserCacheMemDataPtr> map_res;
		for(MyUtil::ObjectMap::const_iterator it = res->data.begin(); it != res->data.end(); ++it) {
			UserCacheMemDataPtr data = UserCacheMemDataPtr::dynamicCast(it->second);
			map_res.insert(make_pair(it->first, data));
		}
		return setData(map_res);
		/*map<long, vector< unsigned char> > result;
		for(MyUtil::ObjectMap::const_iterator it = res->data.begin(); it != res->data.end(); ++it) {
			UserCacheMemDataPtr data = UserCacheMemDataPtr::dynamicCast(it->second);
			logOut("Set", data);
			string dataStr;
			data->SerializeToString(&dataStr);
			result.insert(make_pair(it->first, vector<unsigned char>(dataStr.begin(), dataStr.end())));
		}
		return distCacheAdapterPtr->setBatch(result);*/
	}

	bool resetData(int userId) {
		MCE_INFO("[DistUserCacheAdapter::resetData] userid = " << userId);
		UserBaseFullViewPtr full_view_ptr;
		try {
			full_view_ptr = UserBaseAdapter::instance().getUserBaseFullView(userId);
		}catch(...) {
			MCE_WARN("[DistUserCacheAdapter::resetData] getUserBaseFullView Exception userid = " << userId);
			return false;
		}
		UserCacheMemDataPtr data_ptr = Base2Cache(full_view_ptr);
		string dataStr;
		data_ptr->SerializeToString(&dataStr);
		xce::distcache::DataPtr dataPtr(new xce::distcache::Data(dataStr.begin(), dataStr.end())); 
		return distCacheAdapterPtr->set(userId, dataPtr); 
	}

	vector<long> resetData(const vector<int> userIds) {
		std::ostringstream os;
		os<<"[DistUserCacheAdapter::resetData] user size = " << userIds.size() << " ; ids ";
		for( MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it ) {
			os << " , " << *it;
		}
		MCE_INFO(os.str());
		int size = userIds.size();
		map<long, DataPtr> result;
		vector<long> noIds;
		for(int i = 0; i < size; ++i) {
			UserBaseFullViewPtr full_view_ptr;
			try {
				full_view_ptr = UserBaseAdapter::instance().getUserBaseFullView(userIds[i]);
			}catch(...) {
				MCE_WARN("[DistUserCacheAdapter::resetData] getUserBaseFullView Exception userid = " << userIds[i]);
				noIds.push_back(userIds[i]);
				continue;
			}
			UserCacheMemDataPtr data_ptr = Base2Cache(full_view_ptr);
			string dataStr;
			data_ptr->SerializeToString(&dataStr);
			xce::distcache::DataPtr dataPtr(new xce::distcache::Data(dataStr.begin(), dataStr.end())); 
			result.insert(make_pair(userIds[i], dataPtr));
		}
		vector<long> failIds = distCacheAdapterPtr->set(result);
		if( !noIds.empty()) {
			failIds.insert(failIds.end(), noIds.begin(), noIds.end());
		}
		return failIds;
	} 
		
	bool checkConflict(const UserCacheMemDataPtr& res, long oper_time, bool& need_reset) {
		timeval start_time;
		gettimeofday(&start_time, NULL);
		int userId = res->id();
		int groupId = userId % GROUP_SIZE;
		hash_map<int, long>::iterator it = last_oper_time_[groupId].find(userId);
		if(it == last_oper_time_[groupId].end()) {
			pair<hash_map<int, long>::iterator, bool> tmp_pair = last_oper_time_[groupId].insert(make_pair(userId, oper_time));
			if(tmp_pair.second) {
				time_to_userid[groupId].insert(make_pair(oper_time, userId));
				return true;
			} else {
				return false;
			}
		} else {
			if(oper_time - it->second > OPER_INTERVAL_TIME) {
				multimap<long, int>::iterator itr;
				for(itr = time_to_userid[groupId].begin(); itr->first <= it->second && itr != time_to_userid[groupId].end(); ++itr) {
					last_oper_time_[groupId].erase(itr->second);
				}
				time_to_userid[groupId].erase(time_to_userid[groupId].begin(), itr);
				pair<hash_map<int, long>::iterator, bool> tmp_pair = last_oper_time_[groupId].insert(make_pair(userId, oper_time));
				if(tmp_pair.second) {
					time_to_userid[groupId].insert(make_pair(oper_time, userId));
					return true;
				} else {
					return false;
				}
			} else {
				it->second = oper_time;
				need_reset = true;
				return false;
			}
		}
	}	
	
	bool setData(const UserCacheMemDataPtr& res) {
		logOut("Set ", res, false);
		long oper_time = getTimeMicroSec();
		string dataStr;
		res->SerializeToString(&dataStr);
		xce::distcache::DataPtr dataPtr(new xce::distcache::Data(dataStr.begin(), dataStr.end())); 
		int userId = res->id();
		int groupId = userId % GROUP_SIZE;
		bool need_reset = false;
		bool result;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_[groupId]);
			result = checkConflict(res, oper_time, need_reset);
		}
		bool setOk = distCacheAdapterPtr->set(res->id(), dataPtr);
		if(!setOk) {
			return setOk;
		}
		if(result) {
			return result;
		} else if(need_reset) {
			return resetData(userId);
		}
	}
					
	vector<long> setData(const map<long, UserCacheMemDataPtr>& res) {
		long oper_time = getTimeMicroSec();
		vector<map<long, xce::distcache::DataPtr> > result(GROUP_SIZE);
		vector<map<long, UserCacheMemDataPtr> > group_res(GROUP_SIZE);
		for(map<long, UserCacheMemDataPtr>::const_iterator it = res.begin(); it != res.end(); ++it) {
			logOut("Set",it->second, false);
			string dataStr;
			it->second->SerializeToString(&dataStr);
			xce::distcache::DataPtr dataPtr(new xce::distcache::Data(dataStr.begin(), dataStr.end())); 
			result[it->first % GROUP_SIZE].insert(make_pair(it->first, dataPtr));
			group_res[it->first % GROUP_SIZE].insert(*it);
		}
		vector<long> wrongIds;	
		for(int i = 0; i < GROUP_SIZE; ++i) {
			if(result[i].size() > 0) {
				vector<int> resetIds;
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_[i]);
					for(map<long, UserCacheMemDataPtr>::const_iterator it = group_res[i].begin(); it != group_res[i].end(); ++it) {
						timeval start_time;
						gettimeofday(&start_time, NULL);
						bool temp_needs_reset = false;
						bool check_result = checkConflict(it->second, oper_time, temp_needs_reset);
						if(!check_result && temp_needs_reset) {
							resetIds.push_back(it->first);
						}
					}
				}
				vector<long> temp_wrongIds = distCacheAdapterPtr->set(result[i]);
				for(vector<long>::const_iterator it = temp_wrongIds.begin(); it != temp_wrongIds.end(); ++it) {
					group_res[i].erase(*it);
				}
				if(resetIds.size() > 0) {
					vector<long> temp_wrongIds1 = resetData(resetIds);
					wrongIds.insert(wrongIds.end(), temp_wrongIds1.begin(), temp_wrongIds1.end());	
				}
				wrongIds.insert(wrongIds.end(), temp_wrongIds.begin(), temp_wrongIds.end());	
			}
		}
		return wrongIds;
	}
					
	UserCacheMemDataPtr getDataWithCAS(int id) {
		string dataStr; 
		xce::distcache::DataPtr dataPtr = distCacheAdapterPtr->get(id);
                                                
		if(dataPtr == NULL ) {
			MCE_WARN("DistUserCacheAdapter::getDataWithCAS not in memcached userid = " << id);
			return NULL;
		}
		dataStr.assign(dataPtr->begin(), dataPtr->end());
		UserCacheMemDataPtr data = new UserCacheMemData;
		data->ParseFromString(dataStr);
						
		logOut("GetCAS", data);
		return data;
	}
					
	map<long, UserCacheMemDataPtr> getDataWithCAS(vector<long> ids) {
		map<long, UserCacheMemDataPtr> result;
		map<int64_t, DataPtr> propMap = distCacheAdapterPtr->get(ids);
		for(map<int64_t, DataPtr>::const_iterator it = propMap.begin(); it != propMap.end(); ++it) {
			if(it->second == NULL ) {
				MCE_WARN("DistUserCacheAdapter::getDataWithCAS not in memcached userid = " << it->first);
				continue;
			}
			string dataStr;
			dataStr.assign(it->second->begin(), it->second->end());
			UserCacheMemDataPtr data = new UserCacheMemData;
			//MCE_INFO("[DistUserCacheAdapter::getDataWithCAS] start to parse string");
			data->ParseFromString(dataStr);
			result.insert(make_pair(it->first,data));
			logOut("GetCAS", data);
		}
		return result;
	}

	UserCacheMemDataPtr Base2Cache(const UserBaseFullViewPtr& data) {
		TimeCost tc = TimeCost::create("DistUserCacheAdapter::Base2Cache", TimeCost::LOG_LEVEL_INFO);
		UserCacheMemDataPtr res = new UserCacheMemData;
		res->set_id(data->id());
		res->set_status(data->status());
		res->set_univ(data->univ());
		res->set_state(data->state());
		res->set_level(data->level());
		res->set_name(data->name());
		res->set_tinyurl(data->tinyurl());
		string gg = data->gender();
		if (gg == "男生") {
			res->set_gender(1);
		} else if(gg == "女生") {
			res->set_gender(2);
		} else {
			res->set_gender(0);
		}

		vector<int> ids;
		ids.push_back(data->id());
		try {
			UserOnlineTypeSeq temData = OnlineBitmapAdapter::instance().getOnlineUsers( ids );
			map<int, int> webRes;
			for( UserOnlineTypeSeq::const_iterator it = temData.begin(); it != temData.end(); ++it )
			{
				webRes.insert( make_pair( (*it)->userId, (*it)->onlineType ) );
			}
			
			map<int, int>::const_iterator findIt = webRes.find(data->id());
			if(findIt == webRes.end()) {
				res->set_online(0);
				MCE_WARN("DistUserCacheAdapter::Base2Cache no OnlineBitmap online state userid = " << data->id());
			} else {
				int online_cache = 0;
				int online = 0;
				online = (findIt->second & WEBPAGER_ONLINE) ? ONLINEWEBPAGGER : OFFLINE;
				if(online > 0) {
					online_cache |= (1 << (online - 1));
				}
				online = (findIt->second & TALK_ONLINE) ? ONLINEXNT: OFFLINE;
				if(online > 0) {
					online_cache |= (1 << (online - 1));
				}
				res->set_online(online_cache);
			}

			map<int, int> wap_res = WapOnlineAdapter::instance().getOnlineStatus(ids);
			findIt = wap_res.find(data->id());
			if(findIt == wap_res.end()) {
				MCE_WARN("DistUserCacheAdapter::Base2Cache no Wap online state userid = " << data->id());
			} else {
				int online_cache = res->online();
				int online = 0;
				online = (findIt->second & WAP_ONLINE) ? ONLINEWAP  : OFFLINEWAP;
				if(online > 0) {
					online_cache |= (1 << (online - 1));
				}
				res->set_online(online_cache);
			}
		} catch(const Ice::Exception& e) {
			MCE_WARN("DistUserCacheAdapter::Base2Cache Exception: " << e);
		} catch(const exception& e) {
			MCE_WARN("DistUserCacheAdapter::Base2Cache Exception: " << e.what());
		} catch(...) {
			MCE_WARN("DistUserCacheAdapter::Base2Cache Unknow Exception");
	}
	return res;
}
private:
	long getTimeMicroSec() {
		struct timeval time_tv;
		memset(&time_tv, 0, sizeof(timeval));
		gettimeofday(&time_tv, NULL);
		long time = time_tv.tv_sec * 1000 + time_tv.tv_usec / 1000;
		return time;
	}	
	xce::distcache::DistCacheAdapterPtr distCacheAdapterPtr;
	vector<hash_map<int, long> > last_oper_time_;
	vector<multimap<long, int> > time_to_userid;
	IceUtil::Monitor<IceUtil::Mutex> mutex_[GROUP_SIZE];
};

}
}
}
#endif
