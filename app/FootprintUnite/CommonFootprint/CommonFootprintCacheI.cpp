#include "app/FootprintUnite/share/FootprintShare.h"
#include "CommonFootprintCacheI.h"
#include "sstream"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"


using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::footprint;
using namespace xce::log;
using namespace xce::serverstate;



//---------------------------------------------------------------------------
void MyUtil::initialize() {
       ServiceI& service = ServiceI::instance();
  
       service.getAdapter()->add(new CommonFootprintCacheI, service.createIdentity("UFC", ""));
        TaskManager::instance().config(TASK_LEVEL_BUILD,ThreadPoolConfig(0,30));
        TaskManager::instance().config(TASK_LEVEL_ADD,ThreadPoolConfig(0,100));
        TaskManager::instance().config(TASK_LEVEL_REMOVE,ThreadPoolConfig(0,100));
        TaskManager::instance().config(TASK_LEVEL_SAVE,ThreadPoolConfig(0,100));
        //ActiveTrack::instance().initialize(); 

        service.registerObjectCache(FOOTPRINT_SET, "FS", new FootprintFactoryI);//register
        // 将UserVisitCount的cache由User移到FootprintNew  -- 090911 by zhanghan
        //service.registerObjectCache(USER_VISIT_COUNT, "UVC", new UserVisitCountFactoryI);

        // 加入向Controller注册
        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Footprint.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Interval", 5);
        string ctrEndpoints = service.getCommunicator()->getProperties()
                ->getProperty("Footprint.ControllerEndpoint");
        ServerStateSubscriber::instance().initialize(ctrEndpoints,
                       &CommonFootprintCacheI::instance(), mod, interval);

}

//------------------------FootprintFactoryI---------------------------------------
Ice::ObjectPtr FootprintFactoryI::create(long id) {
	MCE_INFO("FootprintFactoryI::create id " << id );

	// 同步加载模式
	string table = CommonFootprintCacheI::instance().getTableName(id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << COLUMN_OWNER<< "=" << id;
	CommonFootprintSetPtr obj = new CommonFootprintSet(SAVESIZE); 
	try {
		QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, FootprintSetHandler<CommonFootprintSetPtr>(obj));
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

//------------------------------------------------------------
CommonFootprintCacheI::CommonFootprintCacheI() {

        ServiceI& service = ServiceI::instance();
        Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
		
        cacheSize_ = props->getPropertyAsIntWithDefault(
                        "Service." + service.getName() + ".FootprintSet.CacheSize", 24);
//        dataSize_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".FootprintSet.DataSize",6);

        tableCount_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);
        //tableName_ = props->getPropertyWithDefault("Service." + service.getName() + ".TableName", "visit_footprint");
        tableName_ = props->getPropertyWithDefault("Service." + service.getName() + ".TableName", "");
        saveBatchSize_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBatchSize", 100);
        saveBatchTime_ = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBatchTime", 15);

	saveBucket_ = new SaveBucket<BaseStepPtr,CommonFootprintCacheI>(tableName_,tableCount_,saveBatchSize_, saveBatchTime_,0,cacheSize_,this);

        MCE_INFO("[FootprintManagerI::FootprintManagerI] serverName:" << service.getName() << " tableName:" << tableName_ 
                        << " tableCount:" << tableCount_ << " saveBatchSize:" << saveBatchSize_);
}
CommonFootprintCacheI::~CommonFootprintCacheI(){};

void
xce::footprint::CommonFootprintCacheI::visit(const ::xce::footprint::FootprintInfoPtr& fpt,
                                          const Ice::Current& current)
{
	MCE_INFO("[CommonFootprintCache::visit] owner:" << fpt->owner<<" visit:"<<fpt->visitor);
	CommonFootprintSetPtr p = getFootprintSet(fpt->owner);
	if(!p)
		return;
	try {
                p->add(fpt);
		saveBucket_->put(fpt->owner);

        //        TaskManager::instance().execute(new AddTask<BaseStepPtr, CommonFootprintCacheI>(fpt, *saveBucket_, *this));
	//	ostringstream object;
        //      object << footprint->tinyinfo->guest;
        //      ActiveTrack::instance().action(footprint->host, ref, "ADD_FOOTPRINT", object.str());
        } catch (Ice::Exception& e) {
                MCE_WARN("FootprintManagerI::add exception." << e);
        } catch (...) {
                MCE_WARN("Some exception. In FootprintManagerI::add");
        }
}

void
xce::footprint::CommonFootprintCacheI::visitBatch(const ::xce::footprint::FootprintInfoSeq& fpts,
                                               const Ice::Current& current)
{
	for(FootprintInfoSeq::const_iterator it=fpts.begin(); it!=fpts.end(); ++it){
		visit(*it);
	}
}

void
xce::footprint::CommonFootprintCacheI::remove(const ::xce::footprint::FootprintInfoPtr& fpt,
                                           const Ice::Current& current)
{
	MCE_INFO("[CommonFootprintCacheI::remove] owner:"<<fpt->owner<<" visitor:"<<fpt->visitor);
	CommonFootprintSetPtr p = getFootprintSet(fpt->owner);
	if(!p)
		return;
	try{
		p->remove(fpt);
		saveBucket_->put(fpt->owner);
//		TaskManager::instance().execute(new AddTask<BaseStepPtr, CommonFootprintCacheI>(fpt, *saveBucket_, *this));
	} catch(Ice::Exception& e) {
                MCE_WARN("FootprintManagerI::add exception." << e);
	} catch(...) {
                MCE_WARN("Some exception. In FootprintManagerI::add");
	} 
}

::Ice::Int
xce::footprint::CommonFootprintCacheI::getSize(::Ice::Long id,
                                            const Ice::Current& current)
{
    CommonFootprintSetPtr p = getFootprintSet(id);
    if(!p)
		return 0;
    try{
	 int size = p->getSize();
	 return size;
    } catch(Ice::Exception& e) {
	MCE_WARN("FootprintManagerI::getSize exception. "<<e);
    } catch(...) {
	MCE_WARN("Some exception. In FootprintManagerI::getSize");
    }
    return 0;
}

::xce::footprint::CommonInfoPtr
xce::footprint::CommonFootprintCacheI::getAll(::Ice::Long id,
                                           ::Ice::Int begin,
                                           ::Ice::Int nLimit,
                                           ::Ice::Int tLimit=0,
                                           const Ice::Current& current)
{
    CommonFootprintSetPtr p = getFootprintSet(id);
	
    CommonInfoPtr res = new CommonInfo;
    res->steps = p->get(begin,nLimit,tLimit);
    return res;
}

bool
xce::footprint::CommonFootprintCacheI::isValid(const Ice::Current& current)
{
	MCE_INFO("MenuCacheManagerI::isValid");
	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return isValid_;
}

void
xce::footprint::CommonFootprintCacheI::setValid(bool newState,
                                             const Ice::Current& current)
{
        IceUtil::RWRecMutex::WLock lock(validMutex_);
        isValid_ = newState;
        if (isValid_) {
                ServerStateSubscriber::instance().setStatus(1);
        } else {
                ServerStateSubscriber::instance().setStatus(0);
        }

}

void
xce::footprint::CommonFootprintCacheI::contral(const ::MyUtil::Str2StrMap& cmd,
                                            const Ice::Current& current)
{
}

string xce::footprint::CommonFootprintCacheI::getTableName(long owner) {
	if(tableCount_ == 1)
		return tableName_;
	ostringstream oss;
	oss<< tableName_ <<"_"<< owner%tableCount_;	
	return oss.str();
}

CommonFootprintSetPtr CommonFootprintCacheI::getFootprintSet(long id) {
        CommonFootprintSetPtr fptn = ServiceI::instance().locateObject<CommonFootprintSetPtr>(FOOTPRINT_SET, id);
	if ( fptn ) 
		return fptn;
	else
        	return NULL;
}


