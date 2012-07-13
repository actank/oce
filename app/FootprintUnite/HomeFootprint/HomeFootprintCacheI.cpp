#include "app/FootprintUnite/share/FootprintShare.h"
#include "HomeFootprintCacheI.h"
#include "sstream"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BatchUpdaterAdapter.h"
#include "FootprintViewAdapter.h"
#include "HomeFootprintReplicaAdapter.h"
#include "util/cpp/InvokeMonitor.h"
#include <Monitor/client/service/ServiceMonitor.h>

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::footprint;
using namespace xce::log;
using namespace xce::serverstate;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&HomeFootprintCacheI::instance(), service.createIdentity("HFC", ""));
	service.getAdapter()->add(&HomeFootprintCacheReaderI::instance(), service.createIdentity("HFR", ""));
	service.getAdapter()->add(&HomeFootprintBufCacheReaderI::instance(), service.createIdentity("HFBR", ""));
	service.getAdapter()->add(&HomeUserVisitCountCacheReaderI::instance(), service.createIdentity("HUVCR", ""));
	TaskManager::instance().config(TASK_LEVEL_BUILD,ThreadPoolConfig(0,30));
	TaskManager::instance().config(TASK_LEVEL_ADD,ThreadPoolConfig(0,100));
	TaskManager::instance().config(TASK_LEVEL_REMOVE,ThreadPoolConfig(0,100));
	TaskManager::instance().config(TASK_LEVEL_SAVE,ThreadPoolConfig(0,100));

	TaskManager::instance().config(TASK_LEVEL_LOADBUFSET,ThreadPoolConfig(0,5));
	//        ActiveTrack::instance().initialize(); 

	service.registerObjectCache(FOOTPRINT_SET, "FS", new FootprintFactoryI,false);//register
	service.registerObjectCache(FOOTPRINT_BUF_SET, "SFS", new FootprintBufFactoryI);//register
	service.registerObjectCache(USER_VISIT_COUNT, "UVC", new UserVisitCountFactoryI,false);
	// 将UserVisitCount的cache由User移到FootprintNew  -- 090911 by zhanghan
	//service.registerObjectCache(USER_VISIT_COUNT, "UVC", new UserVisitCountFactoryI);

	// 加入向Controller注册
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Mod", 10);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Interval", 5);
  map<string, Ice::ObjectPtr> mapNameAndService;
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HFR", &HomeFootprintCacheReaderI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HFBR", &HomeFootprintBufCacheReaderI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HUVCR", &HomeUserVisitCountCacheReaderI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("HFC", &HomeFootprintCacheI::instance()));

  string ctrEndpoints = service.getCommunicator()->getProperties()
      ->getPropertyWithDefault("Footprint.ControllerEndpoint", "ControllerFootprintUnite");
	ServerStateSubscriber::instance().initialize(ctrEndpoints,
			mapNameAndService, &HomeFootprintCacheI::instance(), mod, interval);
  {
    int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Status", 0);
    ServerStateSubscriber::instance().setStatus(status, "HFR");
    ServerStateSubscriber::instance().setStatus(status, "HFBR");
    ServerStateSubscriber::instance().setStatus(status, "HUVCR");
    ServerStateSubscriber::instance().setStatus(status, "HFC");
    ServerStateSubscriber::instance().setStatus(status);

    std::ostringstream os;
    os << "MyUtil::initialize set status : " << status;
    MCE_WARN(os.str());
  }

	//统计log
	ServiceMonitorManager::instance().start();
}

//------------------------FootprintFactoryI---------------------------------------
Ice::ObjectPtr FootprintFactoryI::create(long id) {
	MCE_INFO("FootprintFactoryI::create id " << id );
	// 同步加载模式
	string table = HomeFootprintCacheI::instance().getTableName(id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << COLUMN_OWNER<< "=" << id;
	HomeFootprintSetPtr obj = new HomeFootprintSet(CACHE_SIZE); 
	try {
		QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, FootprintSetHandler<HomeFootprintSetPtr>(obj));
	} catch (mysqlpp::Exception& e ) {
		MCE_WARN("[FootprintFactoryI::create] create footprint of " << id << " mysqlpp::exception : " << e.what());
		return NULL;
	} catch (...) {
		MCE_WARN("[FootprintFactoryI::create] create footprint of " << id << " unknown exception");
		return NULL;
	}
	//ServiceI::instance().addObject(obj, FOOTPRINT_SET, (long)id);
	return obj;
}

Ice::ObjectPtr FootprintBufFactoryI::create(long id) {
	MCE_INFO("FootprintBufFactoryI::create id " << id );
	// 同步加载模式
	string table = HomeFootprintCacheI::instance().getTableBufName(id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << COLUMN_OWNER<< "=" << id;
	HomeFootprintSetPtr obj = new HomeFootprintSet(CACHE_BUF_SIZE); 
	try {
		QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, FootprintSetHandler<HomeFootprintSetPtr>(obj));
	} catch (mysqlpp::Exception& e ) {
		MCE_WARN("[FootprintBufFactoryI::create] create footprint of " << id << " mysqlpp::exception : " << e.what());
		return NULL;
	} catch (...) {
		MCE_WARN("[FootprintBufFactoryI::create] create footprint of " << id << " unknown exception");
		return NULL;
	}
	//ServiceI::instance().addObject(obj, FOOTPRINT_BUF_SET, id);
	return obj;
}

Ice::ObjectPtr OldFootprintFactoryI::create(long id) {

	string table;
	{
		ostringstream oss;
		oss << "footprintn_"<<id%100; 
		table = oss.str();
	}
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE host =" << id;
	HomeFootprintSetPtr obj = new HomeFootprintSet(CACHE_SIZE);
	try {
		QueryRunner("footprintn", CDbRServer, table).query(sql, OldFootprintSetHandler(obj));
	} catch (mysqlpp::Exception& e ) {
		MCE_WARN("[FootprintFactoryI::create] create footprint of " << id << " mysqlpp::exception : " << e.what());
		return NULL;
	} catch (...) {
		MCE_WARN("[FootprintFactoryI::create] create footprint of " << id << " unknown exception");
		return NULL;
	}
	//	ServiceI::instance().addObject(obj, FOOTPRINT_SET, (long)id);
	MCE_INFO("Loaded Old footprints :" << obj->getSize() << " for " << id);
	return obj;

}
bool OldFootprintSetHandler::handle(mysqlpp::Row& row) const {

	fsp_->setOld((int)row["host"],row["guests"]);
	return true;
}

//------------------------------------------------------------
HomeFootprintCacheI::HomeFootprintCacheI() {

	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();


	//        CACHE_SIZE_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".FootprintSet.CacheSize", 9);
	//        CACHE_BUF_SIZE_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".FootprintSet.BufCacheSize", 87);

	//        dataSize_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".FootprintSet.DataSize",6);

	tableCount_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);

	tableName_ = props->getPropertyWithDefault("Service." + service.getName() + ".TableName", "home_footprint_head");
	tableBufName_ = props->getPropertyWithDefault("Service." + service.getName() + ".TableBufName", "home_footprint_body");

	saveBatchSize_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBatchSize", 100);
	saveBufBatchSize_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBufBatchSize", 200);

	saveBatchTime_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBatchTime", 15);
	saveBufBatchTime_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBufBatchTime", 15);

	saveBucket_ = new SaveBucket<HomeStepPtr,HomeFootprintCacheI>(tableName_,tableCount_,saveBatchSize_, saveBatchTime_, 0, CACHE_SIZE,this);
	saveBufBucket_ = new SaveBucket<HomeStepPtr,HomeFootprintCacheI>(tableBufName_,tableCount_,saveBufBatchSize_, saveBufBatchTime_, CACHE_SIZE,  CACHE_BUF_SIZE,this);

	evictor_ = new Evictor(tableCount_,saveBufBatchSize_);
	buf_factory_ = new FootprintBufFactoryI();
	setVisitCountThread_ = new SetVisitCountThread();
	setVisitCountThread_ -> start();
	MCE_INFO("[HomeFootprintCacheI::HomeFootprintCacheI] serverName:" << service.getName() << " tableName:" << tableName_ 
			<< " tableCount:" << tableCount_ << " saveBatchSize:" << saveBatchSize_ );
}
HomeFootprintCacheI::~HomeFootprintCacheI(){};

	void
HomeFootprintCacheI::visit(const ::FootprintInfoPtr& fpt,
		const Ice::Current& current)
{
	ostringstream oss;
	oss << " owner: " << fpt->owner << " visitor: " << fpt->visitor;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HomeFootprintSetPtr p = getFootprintSet(fpt->owner);
	if(!p){
		return;
	}
	try {
		pair<int,HomeStepPtr> state= p->add(fpt);
		//		MCE_INFO("HomeFootprintCache::visit owner:"<<fpt->owner<<" visitor:"<<fpt->visitor<<" from:"<<fpt->gid<<" state:"<<state.first);

		//如果这次添加导致某个脚印被T, 则在第二级cache中添加这个脚印
		if(state.second){
			evictor_->add(fpt->owner,state.second);	
		}
		//第一级没有满，第二级也有可能存在，所以依然要在第二级删除这个脚印
		evictor_->remove(fpt->owner,fpt);
		saveBucket_->put(fpt->owner);

		if ( state.first==VALID ) {
			try {
				Ice::Context ctx = current.ctx;
				if(ctx.size()==0 || (ctx.size() && ctx["nowriteviewcount"]!="true")){
					MCE_INFO("BatchUpdataeAdapter::incUserViewCount"<<fpt->owner<<" ctx:"<<ctx["nowriteviewcount"]);
					//mop::hi::svc::adapter::BatchUpdaterAdapter::instance().incUserViewCount(fpt->owner);
					setVisitCountThread_ -> push(fpt->owner);
					incUserVisitCount(fpt->owner);
				}
			} catch (Ice::Exception& e) {
				MCE_WARN("[FootprintManagerI::add]BatchUpdaterAdapter Ice::Exception :" << e.what());
			} catch (...) {
				MCE_WARN("[FootprintManagerI::add]BatchUpdaterAdapter UNKNOWN Exception");
			}
		} else if ( state.first == REACHLIMIT ) {
			MCE_INFO("HomeFootprintCacheI::visit, id: " << fpt->owner << " visitcount reach limit!");
			//通知历史记录服务
			FootprintViewAdapter::instance().setVisitCountLimit(fpt->owner);
		}
		/*
		// 加入远程统计log 
		ostringstream object;
		object << footprint->tinyinfo->guest;
		ActiveTrack::instance().action(footprint->host, ref, "ADD_FOOTPRINT", object.str());
		 */
	} catch (Ice::Exception& e) {
		MCE_WARN("FootprintManagerI::add exception." << e);
	} catch (...) {
		MCE_WARN("Some exception. In FootprintManagerI::add");
	}


}

	void
HomeFootprintCacheI::visitBatch(const ::FootprintInfoSeq& fpts,
		const Ice::Current& current)
{
	ostringstream oss;
	oss << " ";
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	for(FootprintInfoSeq::const_iterator it=fpts.begin(); it!=fpts.end(); ++it){
		visit(*it);
	}
}

	void
HomeFootprintCacheI::remove(const FootprintInfoPtr& fpt,
		const Ice::Current& current)
{
	ostringstream oss;
	oss << " owner: " << fpt->owner << " visitor: " << fpt->visitor;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	HomeFootprintSetPtr p = getFootprintSet(fpt->owner);
	if(!p)
		return;
	try{
		if(p->remove(fpt)){
			evictor_->fill(fpt->owner,NULL);
			saveBucket_->put(fpt->owner);
		}else{
			evictor_->remove(fpt->owner,fpt);
		}
	} catch(Ice::Exception& e) {
		MCE_WARN("HomeFootprintCacheI::remove exception." << e);
	} catch(...) {
		MCE_WARN("Some exception. In HomeFootprintCacheI::remove");
	} 
}

	::Ice::Int
HomeFootprintCacheI::getSize(::Ice::Long id,
		const Ice::Current& current)
{
	ostringstream oss;
	oss << " owner: "<<id;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	HomeFootprintSetPtr p = getFootprintSet(id);
	if(!p)
		return 0;
	HomeFootprintSetPtr p2 = getFootprintBufSet(id);
	if(!p2)
		return p->getSize();
	return p->getSize() + p2->getSize();
}

	::HomeInfoPtr
HomeFootprintCacheI::getAll(::Ice::Long id,
		::Ice::Int begin,
		::Ice::Int nLimit,
		::Ice::Int tLimit=0,
		const Ice::Current& current)
{
	ostringstream oss;
	oss<< " owner:" << id << " limit: " << nLimit ;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HomeInfoPtr res = new HomeInfo;

	HomeFootprintSetPtr p = getFootprintSet(id);
	if(begin < CACHE_SIZE){
		if(p){
			res->steps = p->get(begin,nLimit,tLimit);
		}
	}
	if((begin+nLimit) > CACHE_SIZE) { 
		HomeFootprintSetPtr p2 = findFootprintBufSet(id);
		if(p2){
      if (begin >= CACHE_SIZE) {
			  vector<HomeStepPtr> vec = p2->get(begin - CACHE_SIZE, nLimit, tLimit);
			  res->steps.insert(res->steps.end(),vec.begin(),vec.end());
      } else {
			  vector<HomeStepPtr>  vec = p2->get(0,nLimit+begin-CACHE_SIZE,tLimit);
			  res->steps.insert(res->steps.end(),vec.begin(),vec.end());
      }
		} else {
			//if not find, load it asynchronizedly.
			TaskManager::instance().execute(new LoadBufSetTask(id, buf_factory_));
		}
	}
	res->visitcount = getUserVisitCount(id);
	/*
	   ostringstream oss;
	   for(size_t i=0;i<res->steps.size();++i){
	   oss<<res->steps[i]->visitor<<" ";       
	   }
	   MCE_INFO("get visitor list:"<<oss.str());
	 */
	MCE_INFO("HomeFootprintCacheI::getAll id: " << id << ", begin: " << begin << ", limit: " << nLimit << " getSize: " 
      << res->steps.size() << " visitcount: " << res->visitcount );
	return res;
}

	bool
HomeFootprintCacheI::isValid(const Ice::Current& current)
{
	MCE_INFO("[HomeFootprintCacheI::isValid]");
	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return isValid_;
}

void HomeFootprintCacheI::setValid(bool newState, const Ice::Current& current)
{
	IceUtil::RWRecMutex::WLock lock(validMutex_);
  int state = 0;
  if (HomeFootprintCacheReaderI::instance().isValid() && HomeFootprintBufCacheReaderI::instance().isValid()
      && HomeUserVisitCountCacheReaderI::instance().isValid()) {
    state = 1;
  }
  MCE_INFO("[HomeFootprintCacheI::setValid] valid = " << state);
  ServerStateSubscriber::instance().setStatus(state, "HFC");
  ServerStateSubscriber::instance().setStatus(state);
}

	void
HomeFootprintCacheI::contral(const ::MyUtil::Str2StrMap& cmd,
		const Ice::Current& current)
{
}

string
	HomeFootprintCacheI::getTableName(long owner) {
		if(tableCount_==1)
			return tableName_;
		ostringstream oss;
		oss<< tableName_ <<"_"<< owner%tableCount_;	
		return oss.str();
	}

string
	HomeFootprintCacheI::getTableBufName(long owner) {
		if(tableCount_==1)
			return tableBufName_;
		ostringstream oss;
		oss<< tableBufName_ <<"_"<< owner%tableCount_;	
		return oss.str();
	}

HomeVisitHistoryPtr HomeFootprintCacheI::getHomeFootprintObj(long id, int category, const Ice::Current& current) {
  HomeFootprintSetPtr ptr = ServiceI::instance().locateObject<HomeFootprintSetPtr>(FOOTPRINT_SET, id);
  if (ptr != 0) {
    return ptr->parseTo();
  }
  return 0;
}

UserVisitCountPtr HomeFootprintCacheI::getUserVisitCountObj(int id, const Ice::Current&) {
  UserVisitCountInfoPtr ptr = ServiceI::instance().locateObject<UserVisitCountInfoPtr>(USER_VISIT_COUNT, id);
  if (ptr != 0) {
    return ptr->parseToUserVisitCountPtr();
  }
  return 0;
}

HomeFootprintSetPtr HomeFootprintCacheI::getFootprintSet(long id) {
	return ServiceI::instance().locateObject<HomeFootprintSetPtr>(FOOTPRINT_SET, id);
}

HomeFootprintSetPtr HomeFootprintCacheI::getFootprintBufSet(long id) {
	return ServiceI::instance().locateObject<HomeFootprintSetPtr>(FOOTPRINT_BUF_SET, id);
}

HomeFootprintSetPtr HomeFootprintCacheI::findFootprintSet(long id) {
	return ServiceI::instance().findObject<HomeFootprintSetPtr>(FOOTPRINT_SET, id);
}

HomeFootprintSetPtr HomeFootprintCacheI::findFootprintBufSet(long id) {
	return ServiceI::instance().findObject<HomeFootprintSetPtr>(FOOTPRINT_BUF_SET, id);
}

	HomeInfoPtr
HomeFootprintCacheI::visitAndGet(const ::FootprintInfoPtr& fpt,
		::Ice::Int begin,
		::Ice::Int nLimit,
		::Ice::Int tLimit,
		const Ice::Current& current)
{
	ostringstream oss;
	oss<< " owner:" << fpt -> owner << " limit: " << nLimit ;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	visit(fpt,current);

	return getAll(fpt->owner,begin,nLimit,tLimit);
}

UserVisitCountInfoPtr HomeFootprintCacheI::locateUserVisitCount(long userId) {
	UserVisitCountInfoPtr countPtr = ServiceI::instance().locateObject<UserVisitCountInfoPtr> (USER_VISIT_COUNT, userId);
	return countPtr;
}

UserVisitCountInfoPtr HomeFootprintCacheI::findUserVisitCount(long userId) {
	return ServiceI::instance().findObject<UserVisitCountInfoPtr> (
			USER_VISIT_COUNT, userId);
}


Ice::Int HomeFootprintCacheI::getUserVisitCount(long userId, const Ice::Current& current) {


	ostringstream oss;
	oss<< " owner:" << userId ;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserVisitCountInfoPtr p = locateUserVisitCount(userId);
	try {
		if ( p ) {
			int count = p->getCount();
			if ( count <= 0 ) {
				//小于等于0，说明内存脏或者有异常，校验内存数据
				MCE_WARN("HomeFootprintCacheI::getUserVisitCount, id: " << userId << " visitcount <= 0");
				setVisitCountThread_ -> push(userId);
				return 0;
			} else {
				MCE_DEBUG("[HomeFootprintCacheI::getUserVisitCount] get visitcount of " << userId << " as " << count);
				return count;
			}
		} else {
      //MCE_INFO("[HomeFootprintCacheI::getUserVisitCount] no object exist for userId = " << userId);
			return 0;
		}
	} catch (...) {
		MCE_WARN("[HomeFootprintCacheI::getUserVisitCount] getCount Exception ");
	}
	return 0;
}
void HomeFootprintCacheI::incUserVisitCount(long userId, const Ice::Current& current) {

	ostringstream oss;
	oss<< " owner:" << userId ;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserVisitCountInfoPtr p = locateUserVisitCount(userId);
	if ( p ) {
		p->incCount(1);
	} else {
		// 如果内存中没有则数据库加，保证db正确
		mop::hi::svc::adapter::BatchUpdaterAdapter::instance().incUserViewCount(userId);
	 }
}
void HomeFootprintCacheI::incUserVisitCountBatch(const MyUtil::LongSeq& userIds,
		Ice::Int count, const Ice::Current& current) {

	ostringstream oss;
	oss<< " ";
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	for (size_t i = 0; i < userIds.size(); ++i) {
		UserVisitCountInfoPtr p = locateUserVisitCount(userIds[i]);
		if ( p ) {
			p->incCount(count);
		} else {
			// 如果内存中没有则不做，这个为调用接口
		}
	}
}

void HomeFootprintCacheI::setUserVisitCountToReplica(int userId, int count) {
  MyUtil::ObjectResultPtr ptr = new MyUtil::ObjectResult;
  ptr->data.insert(make_pair<long, Ice::ObjectPtr>(userId, new UserVisitCount(userId, count)));
  HomeFootprintReplicaAdapter::instance().setUserVisitCount(ptr);
}

void HomeFootprintCacheI::setUserVisitCountToReplica(const map<int, int>& userToCount) {
  MyUtil::ObjectResultPtr ptr = new MyUtil::ObjectResult;
  for (map<int, int>::const_iterator it = userToCount.begin(); it != userToCount.end(); ++it) {
    ptr->data.insert(make_pair<long, Ice::ObjectPtr>(it->first, new UserVisitCount(it->first, it->second)));
  }
  HomeFootprintReplicaAdapter::instance().setUserVisitCount(ptr);
}

void HomeFootprintCacheI::setUserVisitCount(long userId, Ice::Int count,
		const Ice::Current& current) {
	ostringstream oss;
	oss<< " owner: " << userId;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	if (count < 0) {
		return;
	}

	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
	Str2StrMap properties;
	properties[COLUMN_VIEWCOUNT] = boost::lexical_cast<string>(count);

	// 因为在调用这个方法前并未单独写数据库，所以在这里要写入db
	QueryRunner(DESC_VIEW_COUNT, CDbWServer).execute(Statement::buildUpdate(
				TABLE_VIEW_COUNT, filter, properties));
  setUserVisitCountToReplica(userId, count);
	UserVisitCountInfoPtr p = findUserVisitCount(userId);
	if ( p ) {
		p->setCount(count);
	}
}

// ------------ UserVisitCountFactoryI -------------------
Ice::ObjectPtr UserVisitCountFactoryI::create(long id) {
	vector<UserVisitCountInfoPtr> results;
	Statement sql;
	sql << "SELECT " << COLUMN_ID << ", " << COLUMN_VIEWCOUNT << " FROM " << TABLE_VIEW_COUNT << " WHERE id = '" << id << "'";
	try {
		UserVisitCountInfoResultHandlerI handler(results);
		QueryRunner(DESC_VIEW_COUNT, CDbRServer).query(sql, handler);

		MCE_INFO("Loading VisitCount " << id << " done." << results.size());
		if (results.size() == 1) {
			MCE_INFO("Loading VisitCount " << id << " done.");
		} else if (results.size() <= 0 ) {
			Str2StrMap filter;
			filter[COLUMN_ID] = boost::lexical_cast<string>(id);
			QueryRunner(DESC_VIEW_COUNT, CDbWServer).schedule(
					Statement::buildInsert(TABLE_VIEW_COUNT, filter));
      HomeFootprintCacheI::setUserVisitCountToReplica(id, 0);
		} else if (results.size() > 1) {
			// 如果出现大于1条的错误情况,则将所有一并删除,重新插入作为修正
			Str2StrMap filter;
			filter[COLUMN_ID] = boost::lexical_cast<string>(id);
			QueryRunner(DESC_VIEW_COUNT, CDbWServer).execute(
					Statement::buildDelete(TABLE_VIEW_COUNT, filter));
			Str2StrMap filterInsert;
			filterInsert[COLUMN_ID] = boost::lexical_cast<string>(id);
			filterInsert[COLUMN_VIEWCOUNT] = boost::lexical_cast<string>((*(results.end() - 1))->getCount());
			QueryRunner(DESC_VIEW_COUNT, CDbWServer).schedule(
					Statement::buildInsert(TABLE_VIEW_COUNT, filter));
      HomeFootprintCacheI::setUserVisitCountToReplica(id, (*(results.end() - 1))->getCount());
		}
	} catch (mysqlpp::Exception& e) {
		// 出现错误时, 设置－1，因为都改为locateObject方法，所以会被重新加载
		MCE_WARN("[UserVisitCountFactoryI::create] load VisitCount of " << id << " mysqlpp::Exception " << e.what());
	} catch (...) {
		MCE_WARN("[UserVisitCountFactoryI::create] load VisitCount of " << id << " exception");
	}

  if (results.empty()) {
    return 0;
  } else {
	  MCE_INFO("Loading with initialize VisitCount " << id << " as " << (*results.begin())->getCount());
	  return *(results.begin());
  }
}

//-----------------LoadBufSetTask-------------------------
void LoadBufSetTask::handle() {
	try {
		MCE_DEBUG("LoadBufSetTask::handle, id= " << userId_);
		Ice::ObjectPtr obj = factory_->create(userId_);
		if(obj) {
			ServiceI::instance().addObject(obj, FOOTPRINT_BUF_SET, userId_);
		}
	} catch (...) {
		MCE_WARN("HomeFootprint::LoadBufSetTask::handle, id= " << userId_);
	}
}

//-------------------SetVisitCountThread--------------------
void SetVisitCountThread::run() {
	while(true) {
		std::set<long> owners;
		try {
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			if (owners_.size() < VISITCOUNT_BATCH_SIZE) {
				mutex_.timedWait(IceUtil::Time::seconds(VISITCOUNT_BATCH_TIME));
			}
			owners_.swap(owners);
			MCE_DEBUG("SetVisitCountThread::run, owners.size() " << owners.size());
			if (!owners.empty()) {

				//先拿当前cache里数据到FootprintView的历史记录里仲裁，仲裁通过再写入DB，防止DB写脏
				//默认仲裁3次，如果不通过则记录日志，外面用crontab扫描每天的WARN
				int count = 0;
				while (true) {
					com::xiaonei::xce::Statement sql;
					sql << "INSERT INTO " << TABLE_VIEW_COUNT << " (ID, viewcount) values ";
					Int2IntMap counts;
					for (std::set<long>::iterator it = owners.begin(); it != owners.end(); ++it) {
						if ( it != owners.begin() ) {
							sql << ", ";
						}

						Ice::Int visit_count = HomeFootprintCacheI::instance().getUserVisitCount(*it);
						
						//0 有可能为getUserVisitCount异常
						//都需要重新加载数据
						if (0 < visit_count) {
							sql << "(" << (*it) << "," << visit_count << ")";
							counts[*it] = visit_count;
						} else {
							//如果为-1/0，说明内存数据可能有误，这时不能写脏数据库
							//重新reload数据,到历史记录里校验
							ServiceI::instance().reloadObject(USER_VISIT_COUNT, (*it));
							counts[*it] = HomeFootprintCacheI::instance().getUserVisitCount(*it);
							sql << "(" << (*it) << "," << counts[*it] << ")";
							MCE_INFO("SetVisitCountThread::run " << *it << " visitcount is zero! new count: " << counts[*it]);
						}
					}
					
					bool verity_value = FootprintViewAdapter::instance().verifyVisitCount(counts);
					if (verity_value) {
						sql << " ON DUPLICATE KEY UPDATE " << "viewcount = values(viewcount)";
						com::xiaonei::xce::QueryRunner(DESC_VIEW_COUNT, CDbWServer, TABLE_VIEW_COUNT).schedule(sql);
            HomeFootprintCacheI::setUserVisitCountToReplica(counts); 
						break;
					} else {
						count ++;
					}

					if (count > 3) {
						MCE_WARN("FootprintViewAdapter::verityVisitCount is error, counts.size(): " << counts.size());
						break;
					}
				}//end while
			}//end empty
		} catch(...) {
			std::ostringstream oss;
			for (std::set<long>::iterator it = owners.begin(); it != owners.end(); ++it) {
				oss << *it << " ";
			}
			MCE_WARN("SetVisitCountThread::run Exception, ids: " << oss.str());
		}
	}
}
