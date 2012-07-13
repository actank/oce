#include "FeedEdmCacheI.h"
#include "UserBaseAdapter.h"
#include "UserNetworkAdapter.h"
#include "AdEdmMatcherAdapter.h"
#include "AdNotifyAdapter.h"
#include <sstream>
#include <ctime>
#include <vector>
#include <string>
void ::MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::edm::FeedEdmCacheI::instance(),
      service.createIdentity("FEC", ""));
  xce::edm::FeedEdmCacheI::instance().Initialize();
  MCE_DEBUG("initialize FeedEdmCache ok");
  TaskManager::instance().scheduleRepeated(&xce::edm::MiniteCache::instance());
  MCE_DEBUG("start timer ok");
}
namespace xce {
namespace edm {

using namespace xce::ad;
static const size_t kMinLimitSize = 3;
static const size_t kMaxLimitSize = 8;
static const std::string kConfigureTableName = "feed_edm_configure";
static const std::string kSendedTableName = "feed_edm_sended";
static const std::string kSendedTableNameSp = "feed_edm_sended_sp";

FeedEdmCacheI::FeedEdmCacheI() 
    : need_init_(true), init_complete_(false) {
}

void FeedEdmCacheI::DbConsumeOperator::handle() {
  com::xiaonei::xce::Statement sql;
  sql << "update " << kConfigureTableName <<" set consume = consume +1  where id = " << edm_id_;
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("update consume info of edm:" << edm_id_ << " in table:" << kConfigureTableName << " failed, err->" << e);
  } 
  sql.clear();
  sql << "insert into " << kSendedTableName << "(config_id,userid) values (" << edm_id_ << "," << user_id_ << ")";
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kSendedTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("insert send of edm:" << edm_id_ << " in table:" << kSendedTableName << " failed, err->" << e);
  }
}
void FeedEdmCacheI::DbConsumeOperatorUncheck::handle() {
  com::xiaonei::xce::Statement sql;
  sql << "update " << kConfigureTableName <<" set consume = consume +1  where id = " << edm_id_;
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("update consume info of edm:" << edm_id_ << " in table:" << kConfigureTableName << " failed, err->" << e);
  }
  sql.clear();
  sql << "insert into " << kSendedTableNameSp << "(config_id,userid) values (" << edm_id_ << "," << user_id_ << ")";
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kSendedTableNameSp).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("insert send of edm:" << edm_id_ << " in table:" << kSendedTableNameSp << " failed, err->" << e);
  }
}

void FeedEdmCacheI::DbStatusOperator::handle() {
  com::xiaonei::xce::Statement sql;
  sql << "update " << kConfigureTableName <<" set status = " << kComplete << " where id = " << edm_id_;
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("update status info of edm:" << edm_id_ << " in table:" << kConfigureTableName << " failed, err->" << e);
  }
}

FeedSeedInfoPtr FeedEdmCacheI::SelectEdmById(int edmid, const Ice::Current& current){
  FeedSeedInfoPtr feed_edm_selected = new FeedSeedInfo();
  FeedEdmModelPtr fit_edm_ptr;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    fit_edm_ptr = _SelectEdmById(edmid);
    if (fit_edm_ptr == 0) {
      MCE_DEBUG("return null");
      return 0;
    }
    MCE_INFO("Select fit edm pass: " << ",select edm id:" << fit_edm_ptr->id());
    feed_edm_selected->configId = fit_edm_ptr->configure_model_ptr()->id();
    feed_edm_selected->type = fit_edm_ptr->configure_model_ptr()->type();
    feed_edm_selected->adType = fit_edm_ptr->configure_model_ptr()->ad_type();
    feed_edm_selected->limitType = fit_edm_ptr->configure_model_ptr()->limit_type();
    feed_edm_selected->interactiveFlag = fit_edm_ptr->configure_model_ptr()->interactive_flag();
    feed_edm_selected->name = fit_edm_ptr->configure_model_ptr()->name();
    feed_edm_selected->title = fit_edm_ptr->configure_model_ptr()->title();
    feed_edm_selected->url = fit_edm_ptr->configure_model_ptr()->title_link();
    feed_edm_selected->body = fit_edm_ptr->configure_model_ptr()->body();
    feed_edm_selected->bodyUrl = fit_edm_ptr->configure_model_ptr()->body_link();
    feed_edm_selected->logo = fit_edm_ptr->configure_model_ptr()->logo();
    feed_edm_selected->mediaSrc = fit_edm_ptr->configure_model_ptr()->media(); 
    feed_edm_selected->socialTitle = fit_edm_ptr->social_title();
    feed_edm_selected->appId = fit_edm_ptr->app_id();
    feed_edm_selected->friendId = fit_edm_ptr->partner();
    feed_edm_selected->displayFriendType = fit_edm_ptr->display_friend_type(); 
    feed_edm_selected->likePageLink = fit_edm_ptr->like_page_link();
    feed_edm_selected->likeName = fit_edm_ptr->like_name();
  }
	return feed_edm_selected;
}

FeedEdmModelPtr FeedEdmCacheI::_SelectEdmById(int edmid) {
  std::map<int,FeedEdmModelPtr> fit_edm_seq;
  for (int i = kAssign; i != kNullType -1; --i) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(i);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ++ite) {
			if((*ite)->id() == edmid){
				return (*ite);
			}
    }
  }
  return 0;
}
FeedSeedInfoPtr FeedEdmCacheI::SelectEdmByType(int user_id, int type, const Ice::Current& current){
  FeedSeedInfoPtr feed_edm_selected = new FeedSeedInfo();
  FeedEdmModelPtr fit_edm_ptr;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    fit_edm_ptr = _SelectEdmByType(user_id, type);
    if (fit_edm_ptr == 0) {
      MCE_DEBUG("return null");
      return 0;
    }
    MCE_INFO("Select fit edm pass " << ",select edm id:" << fit_edm_ptr->id());
    feed_edm_selected->configId = fit_edm_ptr->configure_model_ptr()->id();
    feed_edm_selected->type = fit_edm_ptr->configure_model_ptr()->type();
    feed_edm_selected->adType = fit_edm_ptr->configure_model_ptr()->ad_type();
    feed_edm_selected->limitType = fit_edm_ptr->configure_model_ptr()->limit_type();
    feed_edm_selected->interactiveFlag = fit_edm_ptr->configure_model_ptr()->interactive_flag();
    feed_edm_selected->name = fit_edm_ptr->configure_model_ptr()->name();
    feed_edm_selected->title = fit_edm_ptr->configure_model_ptr()->title();
    feed_edm_selected->url = fit_edm_ptr->configure_model_ptr()->title_link();
    feed_edm_selected->body = fit_edm_ptr->configure_model_ptr()->body();
    feed_edm_selected->bodyUrl = fit_edm_ptr->configure_model_ptr()->body_link();
    feed_edm_selected->logo = fit_edm_ptr->configure_model_ptr()->logo();
    feed_edm_selected->mediaSrc = fit_edm_ptr->configure_model_ptr()->media(); 
    feed_edm_selected->socialTitle = fit_edm_ptr->social_title();
    feed_edm_selected->appId = fit_edm_ptr->app_id();
    feed_edm_selected->friendId = fit_edm_ptr->partner();
    feed_edm_selected->displayFriendType = fit_edm_ptr->display_friend_type(); 
    feed_edm_selected->likePageLink = fit_edm_ptr->like_page_link();
    feed_edm_selected->likeName = fit_edm_ptr->like_name();
  }
	return feed_edm_selected;
}

FeedEdmModelPtr FeedEdmCacheI::_SelectEdmByType(int user_id, int type) {
	FeedEdmModelPtr ans;
  std::map<int,FeedEdmModelPtr> fit_edm_seq;
  for (int i = kAssign; i != kNullType -1; --i) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(i);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ++ite) {
			if((*ite)->configure_model_ptr()->type() == type){
				ans = (*ite);
				break;
			}
    }
  }
	if(ans && 12 == ans->configure_model_ptr()->type()){
		std::vector<Ice::Int> app_ids;
		app_ids.push_back(ans->app_id());
		//std::map<Ice::Int, std::vector<Ice::Int> >  partners;
		AdPartners partners;
		if (app_ids.size() > 0) {
			try {
				partners = xce::ad::AdEdmMatcherAdapter::instance().GetAdPartners(user_id,app_ids);
			} catch (Ice::Exception& e) {
				MCE_WARN("call AdNotifierAdapter err ->" << e);
			}
		}
		if (partners.size() > 0) {
			for (AdPartners::iterator ite = partners.begin(); ite != partners.end(); ++ite) {
				if (ite->second.size() >= kMinLimitSize) {
					size_t friends_num = ite->second.size() < kMaxLimitSize ? ite->second.size():kMaxLimitSize;
					std::vector<Ice::Int> partner(std::vector<Ice::Int>(ite->second.begin(),ite->second.begin() + friends_num));
					ans->set_partner(partner);
				}
			}
		}else{
			return 0;
		}
		MCE_INFO("FeedEdmCacheI::_SelectEdmByType --> userid = " << user_id << " type = 12 app_ids.size() = " << app_ids.size() << " partners.size = " << partners.size());
	}
  return ans;
}

FeedSeedInfoPtr FeedEdmCacheI::SelectEdmForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user, const Ice::Current&) {
  timeval start_cache,end_cache;
  FeedSeedInfoPtr feed_edm_selected = new FeedSeedInfo();
  FeedEdmModelPtr fit_edm_ptr;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    gettimeofday(&start_cache,NULL);
    fit_edm_ptr = _SelectEdmForPhone(user_id, params, user);
    gettimeofday(&end_cache,NULL);
    MCE_INFO("FeedEdmCacheI::SelectEdmForPhone --> select edm spend time:" << (end_cache.tv_sec - start_cache.tv_sec) * 1000000 +  end_cache.tv_usec - start_cache.tv_usec);
    if (fit_edm_ptr == 0) {
      MCE_DEBUG("return null");
      return 0;
    }
    MCE_INFO("FeedEdmCacheI::SelectEdmForPhone --> Select fit edm pass, user id:" << user->userId << ",select edm id:" << fit_edm_ptr->id());
    feed_edm_selected->configId = fit_edm_ptr->configure_model_ptr()->id();
    feed_edm_selected->type = fit_edm_ptr->configure_model_ptr()->type();
    feed_edm_selected->adType = fit_edm_ptr->configure_model_ptr()->ad_type();
    feed_edm_selected->limitType = fit_edm_ptr->configure_model_ptr()->limit_type();
    feed_edm_selected->interactiveFlag = fit_edm_ptr->configure_model_ptr()->interactive_flag();
    feed_edm_selected->name = fit_edm_ptr->configure_model_ptr()->name();
    feed_edm_selected->title = fit_edm_ptr->configure_model_ptr()->title();
    feed_edm_selected->url = fit_edm_ptr->configure_model_ptr()->title_link();
    feed_edm_selected->body = fit_edm_ptr->configure_model_ptr()->body();
    feed_edm_selected->bodyUrl = fit_edm_ptr->configure_model_ptr()->body_link();
    feed_edm_selected->logo = fit_edm_ptr->configure_model_ptr()->logo();
    feed_edm_selected->mediaSrc = fit_edm_ptr->configure_model_ptr()->media(); 
    feed_edm_selected->socialTitle = fit_edm_ptr->social_title();
    feed_edm_selected->appId = fit_edm_ptr->app_id();
    feed_edm_selected->friendId = fit_edm_ptr->partner();
    feed_edm_selected->displayFriendType = fit_edm_ptr->display_friend_type(); 
    feed_edm_selected->likePageLink = fit_edm_ptr->like_page_link();
    feed_edm_selected->likeName = fit_edm_ptr->like_name();
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    if (fit_edm_ptr->configure_model_ptr()->limit_type() == 1) {
      DbConsumeOperatorPtr consume_op_ptr = new DbConsumeOperator(feed_edm_selected->configId, user_id);
      MyUtil::TaskManager::instance().execute(consume_op_ptr);
    } else {
      DbConsumeOperatorUncheckPtr consume_op_ptr = new DbConsumeOperatorUncheck(feed_edm_selected->configId, user_id);
      MyUtil::TaskManager::instance().execute(consume_op_ptr);
    }
    fit_edm_ptr->AfterSender(user_id);
    
    _CheckConsume(fit_edm_ptr);
  }
  return feed_edm_selected; 
}

FeedSeedInfoPtr FeedEdmCacheI::SelectEdm(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user, const Ice::Current&) {
  //MCE_INFO("enter feed edm cache server");
  timeval start_cache,end_cache;
  FeedSeedInfoPtr feed_edm_selected = new FeedSeedInfo();
  FeedEdmModelPtr fit_edm_ptr;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    gettimeofday(&start_cache,NULL);
    fit_edm_ptr = _SelectEdm(user_id, params, user);
    gettimeofday(&end_cache,NULL);
    MCE_INFO("select edm spend time:" << (end_cache.tv_sec - start_cache.tv_sec) * 1000000 +  end_cache.tv_usec - start_cache.tv_usec);
    if (fit_edm_ptr == 0) {
      MCE_DEBUG("return null");
      return 0;
    }
    MCE_INFO("Select fit edm pass, user id:" << user->userId << ",select edm id:" << fit_edm_ptr->id());
    feed_edm_selected->configId = fit_edm_ptr->configure_model_ptr()->id();
    feed_edm_selected->type = fit_edm_ptr->configure_model_ptr()->type();
    feed_edm_selected->adType = fit_edm_ptr->configure_model_ptr()->ad_type();
    feed_edm_selected->limitType = fit_edm_ptr->configure_model_ptr()->limit_type();
    feed_edm_selected->interactiveFlag = fit_edm_ptr->configure_model_ptr()->interactive_flag();
    feed_edm_selected->name = fit_edm_ptr->configure_model_ptr()->name();
    feed_edm_selected->title = fit_edm_ptr->configure_model_ptr()->title();
    feed_edm_selected->url = fit_edm_ptr->configure_model_ptr()->title_link();
    feed_edm_selected->body = fit_edm_ptr->configure_model_ptr()->body();
    feed_edm_selected->bodyUrl = fit_edm_ptr->configure_model_ptr()->body_link();
    feed_edm_selected->logo = fit_edm_ptr->configure_model_ptr()->logo();
    feed_edm_selected->mediaSrc = fit_edm_ptr->configure_model_ptr()->media(); 
    feed_edm_selected->socialTitle = fit_edm_ptr->social_title();
    feed_edm_selected->appId = fit_edm_ptr->app_id();
    feed_edm_selected->friendId = fit_edm_ptr->partner();
    feed_edm_selected->displayFriendType = fit_edm_ptr->display_friend_type(); 
    feed_edm_selected->likePageLink = fit_edm_ptr->like_page_link();
    feed_edm_selected->likeName = fit_edm_ptr->like_name();
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    if (fit_edm_ptr->configure_model_ptr()->limit_type() == 1) {
      DbConsumeOperatorPtr consume_op_ptr = new DbConsumeOperator(feed_edm_selected->configId, user_id);
      MyUtil::TaskManager::instance().execute(consume_op_ptr);
    } else {
      DbConsumeOperatorUncheckPtr consume_op_ptr = new DbConsumeOperatorUncheck(feed_edm_selected->configId, user_id);
      MyUtil::TaskManager::instance().execute(consume_op_ptr);
    }
    fit_edm_ptr->AfterSender(user_id);
    
    _CheckConsume(fit_edm_ptr);
  }
  return feed_edm_selected; 
}
void FeedEdmCacheI::_CheckConsume(FeedEdmModelPtr fit_edm_ptr) {
  if (fit_edm_ptr->consume() >= fit_edm_ptr->demand()) {
    DbStatusOperatorPtr status_op_ptr = new DbStatusOperator(fit_edm_ptr->id());
    MyUtil::TaskManager::instance().execute(status_op_ptr);
    fit_edm_ptr->set_status(kComplete);
    
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(fit_edm_ptr->pub_type());
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ) {
      if ((*ite)->id() == fit_edm_ptr->id()) {
        type_seq.erase(ite++);
      } else {
        ++ite;
      }
    }
  } 
}
void FeedEdmCacheI::SetSendingFlag(Ice::Int edm_id, const Ice::Current&) {
  {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (int type = kNullType; type <= kAssign; ++type) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(type);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ) {
      if ((*ite)->id() == edm_id) {
        type_seq.erase(ite++);
      } else {
        ++ite;
      }
    }
  }
  }
  com::xiaonei::xce::Statement sql;
  //sql << "select * from " << kConfigureTableName << " where id=" << edm_id << " and status != " << kSending ;
  sql << "select id,type,ad_type,interactive_flag,displayzzs,demand,max_per_hour,name,status,begin_time,create_time,update_time,title,title_link,body,body_link,logo,customer_id,consume,click,expose,ilike,share,media,pub_type,time_interval,feed_clazz,reply_count,limit_type,social_title,social_page_id,display_friend_type,like_page_link,like_name from " << kConfigureTableName << " where id=" << edm_id ;
  mysqlpp::StoreQueryResult res;
  try {
    res =  com::xiaonei::xce::QueryRunner("feed_edm",com::xiaonei::xce::CDbRServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("selete from configure db error,edm_id:" << edm_id);
  }
  if (res.num_rows() == 0) {
    MCE_WARN("couldn't find unsending edm:"<< edm_id <<" in add edm to configure cache!");
  } else {
    mysqlpp::Row row = res.at(0);
    ConfigureModelPtr configure_ptr = _FormatEdmConfigure(row);
    configure_ptr->set_status(kSending);
    FeedEdmModelPtr feed_edm_model_ptr;
    switch (configure_ptr->pub_type()) {
      case kNullType:
        feed_edm_model_ptr = new FeedNullTypeEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        break;
      case kOrient:
        feed_edm_model_ptr = new FeedOrientEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        break;
      case kAssign:
        feed_edm_model_ptr = new FeedAssignEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        break;
      default:
        MCE_WARN("unexpect pub_type of edm:" << configure_ptr->id());
    }
    sql.clear();
    sql << "update " << kConfigureTableName <<" set status = " << kSending << " where id = " << edm_id;
    try {
      com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kConfigureTableName).store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("update status info of edm:" << edm_id << " in table:" << kConfigureTableName << " failed, err->" << e);
    }
    IceUtil::RWRecMutex::WLock lock(mutex_);
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(feed_edm_model_ptr->pub_type());
    type_seq.push_back(feed_edm_model_ptr);
  }
}
void FeedEdmCacheI::SetSuspendFlag(Ice::Int edm_id, const Ice::Current&) {
  //MCE_INFO("set suspend flag");
  {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (int type = kNullType; type <= kAssign; ++type) { 
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(type);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ) {
      if ((*ite)->id() == edm_id) {
        type_seq.erase(ite++);
      } else {
        ++ite;
      }
    }
  }
  }
  com::xiaonei::xce::Statement sql;
  sql << "update " << kConfigureTableName <<" set status = " << kComplete << " where id = " << edm_id;
  try {
    MCE_DEBUG("to suspend");
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("update status info of edm:" << edm_id << " in table:" << kConfigureTableName << " failed, err->" << e);
  }
}
void FeedEdmCacheI::UpdateEdmInfo(Ice::Int edm_id, const Ice::Current&) {
  {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (int type = kNullType; type <= kAssign; ++type) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(type);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ) {
      if ((*ite)->id() == edm_id) {
        type_seq.erase(ite++);
      } else {
        ++ite;
      }
    }
  }
  } 
  com::xiaonei::xce::Statement sql;
  sql << "select id,type,ad_type,interactive_flag,displayzzs,demand,max_per_hour,name,status,begin_time,create_time,update_time,title,title_link,body,body_link,logo,customer_id,consume,click,expose,ilike,share,media,pub_type,time_interval,feed_clazz,reply_count,limit_type,social_title,social_page_id,display_friend_type,like_page_link,like_name from " << kConfigureTableName << " where id=" << edm_id ;
  mysqlpp::StoreQueryResult res;
  try {
    res =  com::xiaonei::xce::QueryRunner("feed_edm",com::xiaonei::xce::CDbRServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("selete edm info from configure db error,edm_id:" << edm_id);
  }
  if (res.num_rows() == 0) {
    MCE_WARN("couldn't find unsending edm:"<< edm_id <<" in add edm to configure cache!");
  } else {
    mysqlpp::Row row = res.at(0);
    ConfigureModelPtr configure_ptr = _FormatEdmConfigure(row);
    configure_ptr->set_status(kSending);
    FeedEdmModelPtr feed_edm_model_ptr;
    switch (configure_ptr->pub_type()) {
      case kNullType:
        feed_edm_model_ptr = new FeedNullTypeEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        break;
      case kOrient:
        feed_edm_model_ptr = new FeedOrientEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        break;
      case kAssign:
        feed_edm_model_ptr = new FeedAssignEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        break;
      default:
        MCE_WARN("unexpect pub_type of edm:" << configure_ptr->id());
    }
    sql.clear();
    sql << "update " << kConfigureTableName <<" set status = " << kSending << " where id = " << edm_id;
    try {
      com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kConfigureTableName).store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("update status info of edm:" << edm_id << " in table:" << kConfigureTableName << " failed, err->" << e);
    }
    IceUtil::RWRecMutex::WLock lock(mutex_);
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(feed_edm_model_ptr->pub_type());
    type_seq.push_back(feed_edm_model_ptr);
  }
}
void FeedEdmCacheI::SetForComplete(const Ice::Current&) {
  //MCE_INFO("update edm for complete");
  for (int type = kNullType; type <= kAssign; ++type) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(type);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ) {
      if ((*ite)->consume() +1 >= (*ite)->demand()) {
          MCE_INFO("update edm:" << (*ite)->id());
          DbStatusOperatorPtr status_op_ptr = new DbStatusOperator((*ite)->id());
          MyUtil::TaskManager::instance().execute(status_op_ptr);
          (*ite)->set_status(kComplete);
          type_seq.erase(ite++);
      } else {
        ++ite;
      }
    }
  }
}
std::string FeedEdmCacheI::LookUpEdm(int edm_id, const Ice::Current&) {
  std::string model_string = "";
  IceUtil::RWRecMutex::RLock lock(mutex_);
  for (int type = kNullType; type <= kAssign; ++type) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(type);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ++ite) {
      if ((*ite)->id() == edm_id) {
        model_string = (*ite)->ToString();
        return model_string;
      }
    }
  }
  return model_string;
}
std::list<FeedEdmModelPtr>& FeedEdmCacheI::_MappingSeq(int type) {
  switch (type) {
    case kNullType :
      return null_type_seq_;
    case kOrient :
      return orient_seq_;
    case kAssign :
      return assign_seq_;
  }
}

FeedEdmModelPtr FeedEdmCacheI::_SelectEdmForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user) {
	std::vector<FeedEdmModelPtr> passed_select;
	for (int i = kAssign; i != kNullType -1; --i) {
		std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(i);
		for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ++ite) {
			if(8014 == (*ite)->configure_model_ptr()->type()){
				FeedEdmModelPtr fit_edm_ptr = (*ite)->ValidateFunc(user);
				if (fit_edm_ptr != 0) {
					passed_select.push_back((*ite));
				}
			}
		}
		if (passed_select.size() > 0) {
			std::ostringstream out;
			out << "FeedEdmCacheI::_SelectEdmForPhone --> all edms that validate pass:";
			for (std::vector<FeedEdmModelPtr>::iterator ite = passed_select.begin(); ite != passed_select.end(); ++ite) {
				out << " @edm_id:" << (*ite)->id() << "@";
			}
			MCE_DEBUG(out.str());
			size_t size = passed_select.size();
			timeval now_timeval;
			gettimeofday(&now_timeval,NULL);
			srand(now_timeval.tv_usec);
			int random = rand() % size;
			MCE_DEBUG("fit edm size:" << size << ",random num:" << random);
			return passed_select.at(random);
		}
	}
	return 0;
}
FeedEdmModelPtr FeedEdmCacheI::_SelectEdm(Ice::Int user_id, const MyUtil::Str2StrMap& params, const FeedUserInfoPtr& user) {
  std::map<int,FeedEdmModelPtr> fit_edm_seq;
  for (int i = kAssign; i != kNullType -1; --i) {
    std::list<FeedEdmModelPtr>& type_seq = _MappingSeq(i);
    for (std::list<FeedEdmModelPtr>::iterator ite = type_seq.begin(); ite != type_seq.end(); ++ite) {
      FeedEdmModelPtr fit_edm_ptr = (*ite)->ValidateFunc(user);
      if (fit_edm_ptr != 0) {
        fit_edm_seq.insert(std::make_pair<int,FeedEdmModelPtr>(fit_edm_ptr->id(),fit_edm_ptr));
      }
    }
    if (fit_edm_seq.size() > 0) {
      std::vector<Ice::Int> app_ids;
      std::vector<FeedEdmModelPtr> passed_select;
      for (std::map<int,FeedEdmModelPtr>::iterator ite = fit_edm_seq.begin(); ite != fit_edm_seq.end(); ++ite) {
        if (ite->second->app_id() > 0 ) {
          app_ids.push_back(ite->second->app_id());
        } else {
          passed_select.push_back(ite->second);
        }
      } 
      std::map<Ice::Int, std::vector<Ice::Int> >  partners;
      if (app_ids.size() > 0) {
        try {
          partners = xce::ad::AdEdmMatcherAdapter::instance().GetAdPartners(user_id,app_ids);
        } catch (Ice::Exception& e) {
          MCE_WARN("call AdNotifierAdapter err ->" << e);
        }
				MCE_INFO("FeedEdmCacheI::_SelectEdm --> userid = " << user_id << " type = 12 app_ids.size() = " << app_ids.size() << " partners.size = " << partners.size());
      }
      if (partners.size() > 0) {
				for(map<int, FeedEdmModelPtr>::iterator eit = fit_edm_seq.begin(); eit != fit_edm_seq.end(); ++eit){
					FeedEdmModelPtr model_ptr = eit->second;
					int app_id = model_ptr->app_id();
					if(app_id > 0){
						map<int, std::vector<int> >::iterator pit = partners.find(app_id);
						if(pit != partners.end() && pit->second.size() >= kMinLimitSize){
							size_t friends_num = pit->second.size() < kMaxLimitSize ? pit->second.size():kMaxLimitSize;
							std::vector<Ice::Int> partner(std::vector<Ice::Int>(pit->second.begin(),pit->second.begin() + friends_num));
							model_ptr->set_partner(partner);
							passed_select.push_back(model_ptr);
						}
					}
				}
				/*
        for (map<int, std::vector<Ice::Int> >::iterator ite = partners.begin(); ite != partners.end(); ++ite) {
          if (ite->second.size() >= kMinLimitSize) {
            FeedEdmModelPtr passed_model_ptr = fit_edm_seq.find(ite->first)->second;
            size_t friends_num = ite->second.size() < kMaxLimitSize ? ite->second.size():kMaxLimitSize;
            std::vector<Ice::Int> partner(std::vector<Ice::Int>(ite->second.begin(),ite->second.begin() + friends_num));
            passed_model_ptr->set_partner(partner);
            passed_select.push_back(passed_model_ptr);
          }
        }
				*/
      }
      std::ostringstream out;
      
      out << "all edms that validate pass:";
      for (std::vector<FeedEdmModelPtr>::iterator ite = passed_select.begin(); ite != passed_select.end(); ++ite) {
        out << " @edm_id:" << (*ite)->id() << "@";
      }
      MCE_DEBUG(out.str());
      size_t size = passed_select.size();
			if(size <= 0 ){
				return 0;
			}
      timeval now_timeval;
      gettimeofday(&now_timeval,NULL);
      srand(now_timeval.tv_usec);
      int random = rand() % size;
      MCE_DEBUG("fit edm size:" << size << ",random num:" << random);
      return passed_select.at(random);
    }  
  }
  return 0;
}
void FeedEdmCacheI::Initialize() {
  _Load();
}
void FeedEdmCacheI::_Load() {
  MCE_DEBUG("load edm info from db,need_init:" << need_init_ << ",init_complete:" << init_complete_);
  null_type_seq_temp_.clear();
  orient_seq_temp_.clear();
  assign_seq_temp_.clear();
  if(!init_complete_) {
    need_init_ = true;
  }
  init_complete_ = false;
  _LoadEdmModelFromDb();
  IceUtil::RWRecMutex::WLock lock(mutex_);
  null_type_seq_.swap(null_type_seq_temp_);
  orient_seq_.swap(orient_seq_temp_);
  assign_seq_.swap(assign_seq_temp_);
  null_type_seq_temp_.clear();
  orient_seq_temp_.clear();
  assign_seq_temp_.clear();
  init_complete_ = true;
  need_init_ = false;
}
void FeedEdmCacheI::_LoadEdmModelFromDb() {
  com::xiaonei::xce::Statement sql;
  sql << "select id,type,ad_type,interactive_flag,displayzzs,demand,max_per_hour,name,status,begin_time,create_time,update_time,title,title_link,body,body_link,logo,customer_id,consume,click,expose,ilike,share,media,pub_type,time_interval,feed_clazz,reply_count,limit_type,social_title,social_page_id,display_friend_type,like_page_link,like_name from " << kConfigureTableName << " where status=" << kSending;
  mysqlpp::StoreQueryResult res;  
  try {
    res =  com::xiaonei::xce::QueryRunner("feed_edm",com::xiaonei::xce::CDbRServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("loading sending status edm from configure table error!");
  }
  for (unsigned i = 0;i < res.num_rows();++i) {
    mysqlpp::Row row = res.at(i);
    ConfigureModelPtr configure_ptr = _FormatEdmConfigure(row);
    FeedEdmModelPtr feed_edm_model_ptr;
    switch (configure_ptr->pub_type()) {
      case kNullType:
        feed_edm_model_ptr = new FeedNullTypeEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        null_type_seq_temp_.push_back(feed_edm_model_ptr);
        break;
      case kOrient:
        feed_edm_model_ptr = new FeedOrientEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        orient_seq_temp_.push_back(feed_edm_model_ptr);
        break;
      case kAssign:
        feed_edm_model_ptr = new FeedAssignEdmModel(configure_ptr,0);
        feed_edm_model_ptr->LoadMarryInfo();
        assign_seq_temp_.push_back(feed_edm_model_ptr);
        break;
      default:
        MCE_WARN("unexpect pub_type of edm:" << configure_ptr->id());
    }
  }
}
ConfigureModelPtr FeedEdmCacheI::_FormatEdmConfigure(const mysqlpp::Row& row) {
  ConfigureModelPtr configure_ptr = new ConfigureModel();
  if (configure_ptr == 0) {
    MCE_WARN("configure point is null!");
    return false;
  }
  configure_ptr->set_id(row["id"]);
  configure_ptr->set_type(row["type"]);
  configure_ptr->set_ad_type(row["ad_type"]);
  configure_ptr->set_limit_type(row["limit_type"]);
  configure_ptr->set_interactive_flag(row["interactive_flag"]);
  configure_ptr->set_displayzzs(row["displayzzs"]);
  configure_ptr->set_demand(row["demand"]);
  configure_ptr->set_max_per_hour(row["max_per_hour"]);
  configure_ptr->set_name((std::string)row["name"]);
  /* 0:不完整 1:待发 2：发送 3：发送完成 4:关闭 */
  configure_ptr->set_status(row["status"]);
  configure_ptr->set_begin_time((time_t)mysqlpp::DateTime(row["begin_time"]));
  configure_ptr->set_create_ime((time_t)mysqlpp::DateTime(row["create_time"]));
  configure_ptr->set_update_ime((time_t)mysqlpp::DateTime(row["update_time"]));
  configure_ptr->set_title((std::string)row["title"]);
  configure_ptr->set_title_link((std::string)row["title_link"]);
  configure_ptr->set_body((std::string)row["body"]);
  configure_ptr->set_body_link((std::string)row["body_link"]);
  configure_ptr->set_logo((std::string)row["logo"]);
  configure_ptr->set_media((std::string)row["media"]);
  configure_ptr->set_customer_id(row["customer_id"]);
  configure_ptr->set_consume(row["consume"]);
  configure_ptr->set_click(row["click"]);
  configure_ptr->set_expose(row["expose"]);
  configure_ptr->set_ilike(row["ilike"]);
  configure_ptr->set_share(row["share"]);
  configure_ptr->set_pub_type(row["pub_type"]);
  configure_ptr->set_time_interval((std::string)row["time_interval"]);
  configure_ptr->set_feed_clazz((std::string)row["feed_clazz"]);
  configure_ptr->set_reply_count(row["reply_count"]);
  configure_ptr->set_social_title((std::string)row["social_title"]);
  configure_ptr->set_app_id(row["social_page_id"]);
  configure_ptr->set_display_friend_type(row["display_friend_type"]);
  string like_page_link = (std::string)row["like_page_link"];  
  if (like_page_link == "NULL") {
    like_page_link = "";
  }
  configure_ptr->set_like_page_link(like_page_link);
  string like_name = (std::string)row["like_name"];
  if (like_name == ""||like_name == "NULL") {
    like_name = "喜欢";
  }
  configure_ptr->set_like_name(like_name);
  return configure_ptr;
}

}
}
