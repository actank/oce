#include "PageFansReloader.h"
#include <boost/lexical_cast.hpp>
#include "ConnectionQuery.h"
#include "QueryRunner.h"
#include "Date.h"
#include "util/cpp/TimeCost.h"
#include "OceCxxAdapter/src/PageFansCacheAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::buddy;

void MyUtil::initialize(){
        ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(&PageFansReloaderI::instance(), service.createIdentity("L", ""));

        TaskManager::instance().config(TASK_LEVEL_RELOAD, ThreadPoolConfig(1, 1));
        TaskManager::instance().config(TASK_LEVEL_ADDFAN, ThreadPoolConfig(1, 5));
		TaskManager::instance().config(TASK_LEVEL_REMOVEFAN, ThreadPoolConfig(1, 5));
		TaskManager::instance().config(TASK_LEVEL_UPDATEFANWITHSTATE, ThreadPoolConfig(1, 5));
		TaskManager::instance().config(TASK_LEVEL_UPDATEFANSWITHSTATE, ThreadPoolConfig(1, 5));
		TaskManager::instance().config(TASK_LEVEL_ADDBLOCK, ThreadPoolConfig(1, 5));
		TaskManager::instance().config(TASK_LEVEL_REMOVEBLOCK, ThreadPoolConfig(1, 5));

		service.registerXceObserver(new ServiceSchemaI);

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("PageFansReloader.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PageFansReloader.Interval", 5);
        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerPageFansReloader",
                        &PageFansReloaderI::instance(), mod, interval);
}



void PageFansReloaderI::reload(::Ice::Int id, const Ice::Current& current){

	   	ObjectResultPtr results = new ObjectResult;

		PageFansObjPtr obj = new PageFansObj();
		results->data.insert(make_pair(id, obj));
		PageFansCacheAdapter::instance().reload(results);
}


bool  PageFansReloaderI::isValid(const Ice::Current& current) {
		IceUtil::RWRecMutex::RLock lock(validmutex_);
		return valid_;
}

void PageFansReloaderI::setValid(bool valid, const Ice::Current& current) {
		ostringstream oss;
		oss << "PageFansReloaderI::setValid -> " << valid;
		TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
		IceUtil::RWRecMutex::WLock lock(validmutex_);
		valid_ = valid;
		if (valid_) {
				xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
		} else {
				xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
		}
}



::Ice::Int PageFansReloaderI::addFan(const PageEntryPtr& pe, const Ice::Current& current){
	TaskManager::instance().execute(new AddFanTask(pe, TASK_LEVEL_ADDFAN , current));
	return 0;
}

void AddFanTask::handle() {
        PageFansReloaderI::instance().addFanWork(_pe,_current);
}

::Ice::Int PageFansReloaderI::addFanWork(const PageEntryPtr& pe, const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] addFan pageid=" << pe->pageId << " userid="
                        << pe->userId;
        TimeCost tc = TimeCost::create(msg.str(), LOG_LEVEL_INFO);

	PageFansDatabaseHelper::instance().insertFan(pe);
	//notify cache	
	PageFansCacheAdapter::instance().addFan(pe);	
	return 0;
}

::Ice::Int PageFansReloaderI::removeFan(::Ice::Int pageId, ::Ice::Int userId,
		const Ice::Current& current){
	TaskManager::instance().execute(new RemoveFanTask( pageId, userId, TASK_LEVEL_REMOVEFAN ,current));
	return 0;
}

void RemoveFanTask::handle() {
        PageFansReloaderI::instance().removeFanWork(_pageId,_userId,_current);
}

::Ice::Int PageFansReloaderI::removeFanWork(::Ice::Int pageId, ::Ice::Int userId,
                        const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] removeFan pageid=" << pageId << " userid="
                        << userId;
        TimeCost tc = TimeCost::create(msg.str(), LOG_LEVEL_INFO);

        PageFansDatabaseHelper::instance().deleteFan(pageId,userId);
	//notify cache
	PageFansCacheAdapter::instance().removeFan(pageId,userId);
	return 0;
}

::Ice::Int PageFansReloaderI::updateFanWithState(const PageEntryPtr& pe, const Ice::Current& current){
	TaskManager::instance().execute(new UpdateFanWithStateTask( pe, TASK_LEVEL_UPDATEFANWITHSTATE , current));
	return 1;
}

void UpdateFanWithStateTask::handle(){
	PageFansReloaderI::instance().updateFanWithStateWork(_pe,_current);
}

::Ice::Int PageFansReloaderI::updateFanWithStateWork(const PageEntryPtr& pe, const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] updateFanWithState pageid=" << pe->pageId << " userid="
                        << pe->userId;
        TimeCost tc = TimeCost::create(msg.str(), LOG_LEVEL_INFO);

	PageFansDatabaseHelper::instance().updateFanWithState(pe);	
	//notify cache
	PageFansCacheAdapter::instance().updateFanWithState(pe);	
	return 0;	
}

::Ice::Int PageFansReloaderI::updateFansWithState(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState,
		const Ice::Current& current){
        TaskManager::instance().execute(new UpdateFansWithStateTask( pageId, oriState, tarState,TASK_LEVEL_UPDATEFANSWITHSTATE , current));
	   return 1;	
}         
        
void UpdateFansWithStateTask::handle(){
	PageFansReloaderI::instance().updateFansWithStateWork(_pageId,_oriState,_tarState,_current);
}

::Ice::Int PageFansReloaderI::updateFansWithStateWork(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState,
                const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] updateFansWithState pageid=" << pageId << " oriState="
                        << oriState << " tarState=" << tarState;
        TimeCost tc = TimeCost::create(msg.str(), LOG_LEVEL_INFO);

        PageFansDatabaseHelper::instance().updateFansWithState(pageId,oriState,tarState);
        //notify cache
	PageFansCacheAdapter::instance().updateFansWithState(pageId,oriState,tarState);
        return 0;

}
 
::Ice::Int PageFansReloaderI::addBlock(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp,
		const Ice::Current& current){
	TaskManager::instance().execute(new AddBlockTask( pageId, userId, operatorId, timestamp, TASK_LEVEL_ADDBLOCK , current));
   return 1;	
}

void AddBlockTask::handle(){
	PageFansReloaderI::instance().addBlockWork(_pageId,_userId,_operatorId,_timestamp,_current);
}

::Ice::Int PageFansReloaderI::addBlockWork(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp,
                const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] addBlock pageid=" << pageId << " userId="
                        << userId ;
        TimeCost tc = TimeCost::create(msg.str(), LOG_LEVEL_INFO);

        PageFansDatabaseHelper::instance().insertBlock(pageId,userId,operatorId,timestamp);
        //notify cache
		PageFansCacheAdapter::instance().addBlock(pageId,userId,operatorId,timestamp);
        return 0;
	
}

::Ice::Int PageFansReloaderI::removeBlock(::Ice::Int pageId, ::Ice::Int userId,
		const Ice::Current& current){
	TaskManager::instance().execute(new RemoveBlockTask( pageId, userId, TASK_LEVEL_REMOVEBLOCK  ,current));
   return 1;	
}

void RemoveBlockTask::handle(){
	PageFansReloaderI::instance().removeBlockWork(_pageId,_userId,_current);
}

::Ice::Int PageFansReloaderI::removeBlockWork(::Ice::Int pageId, ::Ice::Int userId,
                const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] removeBlock pageid=" << pageId << " userId="
                        << userId ;
        TimeCost tc = TimeCost::create(msg.str(), LOG_LEVEL_INFO);

        PageFansDatabaseHelper::instance().deleteBlock(pageId,userId);
        //notify cache
		PageFansCacheAdapter::instance().removeBlock(pageId,userId);
        return 0;
}
//*************************************************************************************
string PageFansDatabaseHelper::getPageFansTable(int id){
	ostringstream table;
        table << TABLE_PAGE_FANS << abs(id) % TABLE_PAGE_FANS_COUNT;
        return table.str();
}
bool PageFansDatabaseHelper::insertFan(const PageEntryPtr& pe){
	MCE_INFO("insertFan pageid=" << pe->pageId << " userid=" << pe->userId);
	Str2StrMap props;
        props["ID"] = boost::lexical_cast<string>(pe->id);
        props["PAGE_ID"] = boost::lexical_cast<string>(pe->pageId);
        props["USER_ID"] = boost::lexical_cast<string>(pe->userId);
	props["STATE"] = boost::lexical_cast<string>(pe->state);
	props["ADD_TIME"] = MyUtil::Date(pe->addTime).str();
	props["OPERATOR"] = boost::lexical_cast<string>(pe->operatorId);
        QueryRunner(DB_PAGE_FANS, CDbWServer).execute(Statement::buildInsert(
                        getPageFansTable(pe->pageId), props));
        return true;
}

bool PageFansDatabaseHelper::deleteFan(::Ice::Int pageid, ::Ice::Int userid){
	MCE_INFO("deleteFan pageid=" << pageid << " userid=" << userid);
        Str2StrMap props;
        props["PAGE_ID"] = boost::lexical_cast<string>(pageid);
        props["USER_ID"] = boost::lexical_cast<string>(userid);
        QueryRunner(DB_PAGE_FANS, CDbWServer).execute(Statement::buildDelete(
                        getPageFansTable(pageid), props));
        return true;
}

bool PageFansDatabaseHelper::updateFanWithState(const PageEntryPtr& pe){
	MCE_INFO("updateFanWithState pageid=" << pe->pageId << " userid=" << pe->userId);
        Str2StrMap filter,props;
        filter["PAGE_ID"] = boost::lexical_cast<string>(pe->pageId);
        filter["USER_ID"] = boost::lexical_cast<string>(pe->userId);
        props["STATE"] = boost::lexical_cast<string>(pe->state);
        QueryRunner(DB_PAGE_FANS, CDbWServer).execute(Statement::buildUpdate(
                        getPageFansTable(pe->pageId), filter, props));
        return true;
}

bool PageFansDatabaseHelper::updateFansWithState(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState){
	MCE_INFO("updateFansWithState pageid=" << pageId );	
	Str2StrMap filter,props;
        filter["PAGE_ID"] = boost::lexical_cast<string>(pageId);
	filter["STATE"] = boost::lexical_cast<string>(oriState);
	props["STATE"] = boost::lexical_cast<string>(tarState);
	QueryRunner(DB_PAGE_FANS, CDbWServer).execute(Statement::buildUpdate(
                        getPageFansTable(pageId), filter, props));
	return true;
}

bool PageFansDatabaseHelper::insertBlock(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp){
	MCE_INFO("insertBlock pageid=" << pageId << " userid=" << userId 
			<< " operatorId=" << operatorId << " timestamp=" << timestamp);
	
	Str2StrMap props;
        props["PAGEID"] = boost::lexical_cast<string>(pageId);
        props["USERID"] = boost::lexical_cast<string>(userId);
        props["EXECUTOR"] = boost::lexical_cast<string>(operatorId);
	props["TIMESTAMP"] = MyUtil::Date(timestamp).str();
        QueryRunner(DB_PAGE_FANS, CDbWServer).execute(Statement::buildInsert(
                        TABLE_PAGE_FANS_BLOCK, props));
	return 1;	
}

bool PageFansDatabaseHelper::deleteBlock(::Ice::Int pageId, ::Ice::Int userId){
	MCE_INFO("deleteFan pageid=" << pageId << " userid=" << userId);
        Str2StrMap props;
        props["PAGEID"] = boost::lexical_cast<string>(pageId);
        props["USERID"] = boost::lexical_cast<string>(userId);
        QueryRunner(DB_PAGE_FANS_BLOCK, CDbWServer).execute(Statement::buildDelete(
                        TABLE_PAGE_FANS_BLOCK, props));
        return true;
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	MyUtil::TaskManager::instance().config(props, "Reload", TASK_LEVEL_RELOAD, MyUtil::ThreadPoolConfig(1,1));
        MyUtil::TaskManager::instance().config(props, "AddFan", TASK_LEVEL_ADDFAN, MyUtil::ThreadPoolConfig(1,5));
        MyUtil::TaskManager::instance().config(props, "RemoveFan", TASK_LEVEL_REMOVEFAN, MyUtil::ThreadPoolConfig(1,5));
        MyUtil::TaskManager::instance().config(props, "UpdateFanWithState", TASK_LEVEL_UPDATEFANWITHSTATE, MyUtil::ThreadPoolConfig(1,5));
        MyUtil::TaskManager::instance().config(props, "UpdateFansWithState", TASK_LEVEL_UPDATEFANSWITHSTATE, MyUtil::ThreadPoolConfig(1,5));
        MyUtil::TaskManager::instance().config(props, "AddBlock", TASK_LEVEL_ADDBLOCK, MyUtil::ThreadPoolConfig(1, 5));
	MyUtil::TaskManager::instance().config(props, "RemoveBlock", TASK_LEVEL_REMOVEBLOCK, MyUtil::ThreadPoolConfig(1, 5));
}

