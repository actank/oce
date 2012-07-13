#include "PageCommonFansCacheManagerI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyByIdReplicaAdapter.h"
#include "FriendRankCacheAdapter.h"
#include <algorithm>
#include "util/cpp/TimeCost.h"
//
void MyUtil::initialize() {
	using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
	using namespace xce::buddy;
  ServiceI& service = ServiceI::instance();
  const static std::string alias("PCFCD");
  service.registerObjectCache(kPageCommonFansCacheData, alias, new PageCommonFansDataFactory);
	service.getAdapter()->add(&PageCommonFansCacheManagerI::instance(), service.createIdentity("PCFCDID", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PageCommonFansCache.Mod",0);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PageCommonFansCache.Interval", 5);

	ConnectionPoolManager::instance().initialize();

	service.registerXceObserver(new ServiceSchemaI);
  // register to Controller Service
  ServerStateSubscriber::instance().initialize("ControllerPageCommonFansCache",  &PageCommonFansCacheManagerI::instance(), mod, interval);
}

//=========================================================

namespace xce {
namespace buddy {

int CompareFriendRank(const xce::socialgraph::RankData& rd1, const xce::socialgraph::RankData& rd2) {
	return rd1.weight > rd2.weight;
}

void ComputeMessage::Run() {
  using namespace MyUtil;
  using namespace xce::buddy::adapter;
  /*
  xce::socialgraph::RankSeq rankSeq;
  try {
		rankSeq = xce::socialgraph::FriendRankCacheAdapter::instance().GetRank(userId_);
	} catch (...) {
		MCE_WARN("FriendRankCacheAdapter::GetRank()... excpetion  userId_:" << userId_);
	}
  std::sort(rankSeq.begin(), rankSeq.end(), CompareFriendRank);
  MyUtil::IntSeq userIdSeq;
  for (size_t i = 0; i < rankSeq.size(); ++i) {
		userIdSeq.push_back(rankSeq[i].userid);
  }
 	*/
  MyUtil::IntSeq userIdSeq;
  try {
    userIdSeq = BuddyByIdCacheAdapter::instance().getFriendListAsc(userId_, 2000);
  } catch (...) {
    MCE_WARN("BuddyByIdCacheAdapter::instance().getFriendListAsc(" << userId_ << ", 2000) exception");
  }
  
  MyUtil::IntSeq fansIdSeq;
  PageFansDAO dao;
  if (!dao.FetchPageFans(pageId_, userIdSeq, fansIdSeq)) {
    return;
  } 
  if (fansIdSeq.empty())
    return;    

  if (!data_.empty()) {
    xce::buddy::Items items;
    items.ParseFromString(data_);
    xce::buddy::Item* item = items.add_items();
    item->set_pid(pageId_);
    for (size_t i = 0; i < fansIdSeq.size(); ++i) {
			item->add_common(fansIdSeq[i]);
		}
    PageCommonFansDataPtr pageCommonFansData = new PageCommonFansData();
    items.SerializeToString(&(pageCommonFansData->data_));
		ServiceI::instance().addObject(pageCommonFansData, kPageCommonFansCacheData, userId_);
    data_ = pageCommonFansData->data_;
	}
  else {
    xce::buddy::Items items;
    xce::buddy::Item* item = items.add_items();
    item->set_pid(pageId_);
    for (size_t i = 0; i < fansIdSeq.size(); ++i) {
			item->add_common(fansIdSeq[i]);
		}
    PageCommonFansDataPtr pageCommonFansData = new PageCommonFansData();
    items.SerializeToString(&(pageCommonFansData->data_));
		ServiceI::instance().addObject(pageCommonFansData, kPageCommonFansCacheData, userId_);
    data_ = pageCommonFansData->data_;
	}
	
  MCE_INFO("SetData ObjectCache size  " << ServiceI::instance().getObjectCache()->locateCategory(kPageCommonFansCacheData)->size());
}

bool PageFansDAO::FetchPageFans(int pageId, MyUtil::IntSeq& targetIdSeq, MyUtil::IntSeq& resultIdSeq) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce; 
  std::ostringstream oss;
  oss << "FetchPageFans() pageId:" << pageId << "  targetIdSeq.size():" << targetIdSeq.size();
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);

  if (targetIdSeq.empty())
		return false;

	std::ostringstream pattern;
  pattern << "page_fans_" << pageId % 100;

  Statement sql;

  sql << "SELECT user_id FROM page_fans_" << pageId % 100
      << " WHERE page_id = " << pageId << " and user_id in (";
  for (size_t i = 0; i < targetIdSeq.size(); ++i) {
		if (i != targetIdSeq.size() - 1) {
      sql << targetIdSeq[i] << ",";
		}
		else {
      sql << targetIdSeq[i] << ")";
		}
	}

  try {
		QueryRunner("page_fans_search", CDbRServer, pattern.str()).query(sql, FetchFansResultHandler(resultIdSeq));
    return true;
  } catch (...) {
		MCE_WARN("FetchPageFans ... exception  pageId:" << pageId);
	}  

	return false;
}


bool FetchFansResultHandler::handle(mysqlpp::Row& row) const {
  if (!row.empty()) {
		int fan_id = (int) row["user_id"];
		resultIntSeq_.push_back(fan_id);
	}
	return true;
}


MyUtil::IntSeq PageCommonFansCacheManagerI::GetCommon(Ice::Int uid, 
																											Ice::Int pid, 
																											Ice::Int begin, 
																											Ice::Int limit, 
																											const Ice::Current& current) {
	using namespace MyUtil;
	MCE_INFO("GetCommon uid:" << uid << " pid:" << pid << " begin:" << begin << " limit:" << limit);
	MyUtil::IntSeq seq;
	if (!CheckParam(begin, limit))
		return seq;

  try {
    rwLock_.ReadLock();
		if (pageSet_.find(pid) == pageSet_.end()) {
      MCE_INFO("pageId:" << pid << " is not the target page.  uid:" << uid);
      rwLock_.ReadUnlock();
			return seq;
    }
    rwLock_.ReadUnlock();
  } catch (boost::thread_resource_error& error) {
    rwLock_.ReadUnlock();
    MCE_WARN("GetCommon " << error.what());
	} 
 
	PageCommonFansDataPtr object = ServiceI::instance().findObject<PageCommonFansDataPtr>(kPageCommonFansCacheData, uid);
  std::string byte_data;
  if (object) { //找到了
		IceUtil::Mutex::Lock lock(object->mutex_);
    xce::buddy::Items items;
    byte_data = object->data_;
    items.ParseFromString(byte_data);
    bool miss = true;
    for (int i = 0; i < items.items_size(); ++i) {
			const xce::buddy::Item& item = items.items(i);
      if (pid == item.pid()) {
        miss = false;
				break;  
			}
		}
    if (miss) {
      ComputeMessage message(uid, pid, object->data_);
      message.Run();
      byte_data = message.data();
		}
		MCE_INFO("object hit and miss:" << miss << " uid:" << uid << " pid:" << pid << " items.items_size():" << items.items_size());

  }
  else {
    ComputeMessage message(uid, pid);
    message.Run();
    byte_data = message.data();
  } 

  xce::buddy::Items items;
  items.ParseFromString(byte_data);
  for (int i = 0; i < items.items_size(); ++i) {
		const xce::buddy::Item& item = items.items(i);
    if (pid == item.pid()) {
			int size = item.common_size();
			if (limit == -1) {
				for (int i = begin; i < size; ++i) {
					seq.push_back(item.common(i));
				}
				break;
			}
			int end = begin + limit;
			for (int i = begin; i < size && i < end; ++i) {
				seq.push_back(item.common(i));
			}
			break;  
		}
  }
  return seq;
}

MyUtil::IntSeq PageCommonFansCacheManagerI::GetUnCommon(Ice::Int uid, 
																												Ice::Int pid, 
																												Ice::Int begin, 
																												Ice::Int limit, 
																												const Ice::Current& current) {
	using namespace MyUtil;
	MCE_INFO("GetUnCommon uid:" << uid << " pid:" << pid << " begin:" << begin << " limit:" << limit);
	MyUtil::IntSeq seq;
  return seq;


}

CommonFans PageCommonFansCacheManagerI::GetCommonAndUnCommon(const CommonFansParam& param, const Ice::Current& current) {
	using namespace MyUtil;
	MCE_INFO("GetCommonAndUnCommon uid:" << param.uid << " pid:" << param.pid << " cbegin:" << param.cbegin << " climit:" << param.climit << " ubegin:" << param.ubegin << " ulimit:" << param.ulimit);
  CommonFans cf;
	return cf;
}

bool PageCommonFansCacheManagerI::SetData(const PageCommonFans& pcf, const Ice::Current& current) {
  return false;
}

MyUtil::Int2IntSeqMap PageCommonFansCacheManagerI::GetFull(int uid, const Ice::Current& current) {
  using namespace MyUtil;
  MCE_INFO("GetFull() uid:" << uid);
	MyUtil::Int2IntSeqMap pid2fansSeqMap;
	PageCommonFansDataPtr object = ServiceI::instance().findObject<PageCommonFansDataPtr>(kPageCommonFansCacheData, uid);
  if (object) {  //取到了
		IceUtil::Mutex::Lock lock(object->mutex_);
    xce::buddy::Items items;
    items.ParseFromString(object->data_);
    for (int i = 0; i < items.items_size(); ++i) {
			const xce::buddy::Item& item = items.items(i);
      int pid = item.pid();
 			MyUtil::IntSeq fansSeq;
      for (int i = 0; i < item.common_size(); ++i) {
        fansSeq.push_back(item.common(i));
			}     
      pid2fansSeqMap.insert(std::make_pair(pid, fansSeq));
		}
	}
  return pid2fansSeqMap;
}

void PageCommonFansCacheManagerI::AddPages(const MyUtil::IntSeq& pageIdSeq, const Ice::Current& current) {
  try {
		rwLock_.WriteLock();
		pageSet_.insert(pageIdSeq.begin(), pageIdSeq.end());
    rwLock_.WriteUnlock();
    std::ostringstream oss;
    for (MyUtil::IntSeq::const_iterator iter = pageIdSeq.begin(); iter != pageIdSeq.end(); ++iter) {
			oss << *iter << " ";
		}
    MCE_INFO("add new pages, pageIdSeq:" << oss.str() << "\t pageIdSeq.size():" << pageIdSeq.size());
  } catch (...) {
    rwLock_.WriteUnlock();
	}
}


void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	//...
}



}
} // end xce::buddy

