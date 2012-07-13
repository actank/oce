#include "MenuCacheManagerI.h"
#include "ServiceI.h"
#include <Ice/Ice.h>
#include "Channel.h"
#include "OceCxxAdapter/src/ControllerAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "OceCxxAdapter/src/MenuCacheReplicaAdapter.h"
#include "util/cpp/IntSeqSerialize.h"

using namespace xce::menu;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

void MyUtil::initialize() {
	MCE_DEBUG("MenuCache::initialize");
	ServiceI& service = ServiceI::instance();

	// 设置ObjectCache
	service.registerObjectCache(RECMENU_CACHE_DATA, "RCMC",
			new RecentMenuCacheDataFactory,false);
	service.registerObjectCache(FAVMENU_CACHE_DATA, "FVMC",
			new FavoriteMenuCacheDataFactory,false);
	
	// 向icegrid加入服务
	Ice::ObjectPrx base = service.getAdapter()->add(&MenuCacheManagerI::instance(),
                        service.createIdentity("M", ""));

	// 注册HA
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MenuCache.Mod", 10);
	string controllerEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("MenuCache.Endpoint", "ControllerMenuCacheN");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MenuCache.Interval", 5);
	ServerStateSubscriber::instance().initialize(controllerEndpoint, 
			&MenuCacheManagerI::instance(), mod, interval);
	int syncCacheNum = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MenuCache.SyncCache",1);

	Ice::ObjectPrx locatePrx = ServerStateSubscriber::instance().getProxySelf();
	// 获取cluster
	int clusterDefault = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MenuCache.DefaultCluster", 10);
	int cluster = clusterDefault;
	xce::clustercontroller::ControllerAdapter::instance().initialize(controllerEndpoint, 300);
	try {
		int clusterCtl = xce::clustercontroller::ControllerAdapter::instance().getCluster();
		if (clusterCtl > 0 ) {
			cluster = clusterCtl;
			MCE_INFO("[MyUtil::initialize] get cluster from controller : " << cluster);
		} else {
			MCE_WARN("[MyUtil::initialize] get cluster from controller error, use default : " << cluster);
		}
	} catch (...) {
		MCE_WARN("[MyUtil::initialize] get cluster from controller error, use default : " << cluster);
	}
		
	// 将自身的prx传出, 用于预加载
	// 改为给予直接代理 -- by zhanghan 091118
	// MenuCacheManagerPrx prx = MenuCacheManagerPrx::uncheckedCast(base);
	MenuCacheManagerPrx prx = MenuCacheManagerPrx::uncheckedCast(ServerStateSubscriber::instance().getProxySelf());
	xce::menu::MenuCacheManagerI::instance().initialize(cluster, mod, syncCacheNum, locatePrx);
}

/*****************************************************************************/
MenuCacheData::MenuCacheData(const MenuCacheRecordPtr& record) {
	infoArray = record->infoArray;
}

AppIdSeq MenuCacheData::getMenucontents(){
	IceUtil::RWRecMutex::RLock lock(_menucontentsMutex);
	return infoArray;
}

void MenuCacheData::setMenucontents(AppIdSeq menucontents){
	IceUtil::RWRecMutex::WLock lock(_menucontentsMutex);
	infoArray=menucontents;
}

/*****************************************************************************/
MenuCacheManagerI::MenuCacheManagerI() {

}

void MenuCacheManagerI::initialize(int cluster, int mod, int syncCacheNum, Ice::ObjectPrx locatePrx) {
	//_menuCacheManager = prx;
	MCE_INFO("MenuCacheManagerI::initialize");
	try{	
	cluster_ = 10;
	mod_ = mod;
	setValid(false);
	syncCache_ = (syncCacheNum > 0);
	if(locatePrx){
		locatePrx_ = MenuCacheManagerPrx::uncheckedCast(locatePrx);
	}else{
		locatePrx_ = NULL;
	}
	
	syncFavThread_.initialize(mod_,locatePrx_);
	syncRecThread_.initialize(mod_,locatePrx_);
	if(syncCache_){
		syncFavThread_.start(128*1024).detach();
		syncRecThread_.start(128*1024).detach();
	}}catch(exception & e){
		MCE_WARN("initialize exception "<<e.what());
	}
	MCE_INFO("MenuCache Preload mod: "<<mod<<" Clustor:"<<cluster);
//	try{
//		RecentMenuLogicManagerAdapter::instance().preload(prx,mod_,cluster_);
//		FavoriteMenuLogicManagerAdapter::instance().preload(prx,mod_,cluster_);
//	}catch(Ice::Exception& e){
//		MCE_WARN("MenuCacheManagerI::initialize " << e);
//		return;
//	}
}
bool MenuCacheManagerI::syncCache(){
	return syncCache_;
}

CacheType2AppIdSeqMap xce::menu::MenuCacheManagerI::getAll(::Ice::Int userId, const Ice::Current& current) {
	CacheType2AppIdSeqMap result;
	MCE_INFO("MenuCacheManagerI::getAll, userId:" << userId);
	try{	
		MenuCacheDataPtr recentMenuDataPtr = findRecentMenuCacheData(userId);
		if(recentMenuDataPtr)
			result[RECENTLY]=recentMenuDataPtr->getMenucontents();

		MenuCacheDataPtr favoriteMenuDataPtr = findFavoriteMenuCacheData(userId);
		if(favoriteMenuDataPtr)
			result[FAVORITE]=favoriteMenuDataPtr->getMenucontents();
	}catch(Ice::Exception& e){
		MCE_WARN("MenuCacheManagerI::getAll " << e << " userId " << userId);
	}
	//result[INSTALLED]=getAllMenuCacheData(userId)->getMenucontents();
	
	return result;
}

CacheType2AppIdSeqMap xce::menu::MenuCacheManagerI::get(
		::Ice::Int userId, const CacheTypeSeq& typeArray, const Ice::Current& current) {
	MCE_DEBUG("MenuCacheManagerI::get, userId, typeArray.size():" << userId << typeArray.size());
	CacheType2AppIdSeqMap result;
//cout << "MenuCacheManagerI::get " << "userId" << userId <<endl;
	try{	
		for(size_t i=0;i<typeArray.size();i++){
			if(typeArray[i]==RECENTLY){
				MenuCacheDataPtr recentMenuDataPtr = findRecentMenuCacheData(userId);
				if(recentMenuDataPtr)
					result[RECENTLY]=recentMenuDataPtr->getMenucontents();
			}else if(typeArray[i]==FAVORITE){
				MenuCacheDataPtr favoriteMenuDataPtr = findFavoriteMenuCacheData(userId);
				if(favoriteMenuDataPtr)
					result[FAVORITE]=favoriteMenuDataPtr->getMenucontents();
			}
		}
	}catch(Ice::Exception& e){
		MCE_WARN("MenuCacheManagerI::get " << e << " userId " << userId);
	}	
	return result;
}

void xce::menu::MenuCacheManagerI::setBatch(
		const MenuCacheRecordSeq& records, const Ice::Current& current) {
	MCE_INFO("[MenuCacheManagerI::setBatch] setBatch, size = " << records.size() << " record(s)");
	for (MenuCacheRecordSeq::const_iterator it = records.begin(); it != records.end(); ++it ) {
		set((*it)->userId, (*it)->type, (*it)->infoArray, current);
	}
}
void xce::menu::MenuCacheManagerI::setData(
		const MyUtil::ObjectResultPtr& objects,CacheType type, const Ice::Current& current){
	MCE_INFO("[MenuCacheManagerI::setData ] setData size:" << objects->data.size() <<" type: "<<type );
	
	MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult;
	for (map<long, Ice::ObjectPtr>::const_iterator i = objects->data.begin(); i != objects->data.end(); ++i) {
		results->data.insert(make_pair(i->first, new MenuCacheData(MenuCacheRecordPtr::dynamicCast(i->second))));
	}

	if(type==RECENTLY)
		ServiceI::instance().getObjectCache()->addObjects(RECMENU_CACHE_DATA, results->data);
	else if(type==FAVORITE)
		ServiceI::instance().getObjectCache()->addObjects(FAVMENU_CACHE_DATA, results->data);
		
}

void xce::menu::MenuCacheManagerI::set(		
		::Ice::Int userId, CacheType type, const AppIdSeq& infoArray, const Ice::Current& current) {		
	MCE_INFO("[MenuCacheManagerI::set] id = " << userId << " type:" << infoArray.size()); 
	// 先判断是否属于自己
	if ( ! belongsMe(userId) ) {
		return;
	}

	try{	
		if(type==RECENTLY){
			MenuCacheDataPtr recentMenuDataPtr=findRecentMenuCacheData(userId);
			if(0 == recentMenuDataPtr){
				recentMenuDataPtr = new MenuCacheData;
				recentMenuDataPtr->setMenucontents(infoArray);
				ServiceI::instance().addObject(
		                       recentMenuDataPtr,RECMENU_CACHE_DATA, userId);
			}else{	
				//RecentMenuCacheDataPtr recentMenuDataPtr=getRecentMenuCacheData(userId);
			
				recentMenuDataPtr->setMenucontents(infoArray);
			}
		}else if(type==FAVORITE){
			MenuCacheDataPtr favoriteMenuDataPtr=findFavoriteMenuCacheData(userId);
			if(0 == favoriteMenuDataPtr){
				favoriteMenuDataPtr = new MenuCacheData;
                                favoriteMenuDataPtr->setMenucontents(infoArray);
                                ServiceI::instance().addObject(
                                        favoriteMenuDataPtr,FAVMENU_CACHE_DATA, userId);
                        }else{
				favoriteMenuDataPtr->setMenucontents(infoArray);
			}
		}else{
			
		}		
	}catch(Ice::Exception& e){
		MCE_WARN("MenuCacheManagerI::set " << e << " userId " << userId);
	}
}

void xce::menu::MenuCacheManagerI::load(
		const UserIdSeq& userIds, const Ice::Current& current) {
	MCE_DEBUG("MenuCacheManagerI::load, usrIds.size:" << userIds.size());
	try{
		
		
		for(size_t i=0;i< userIds.size();i++){
		
			ServiceI::instance().reloadObject(
	                       		RECMENU_CACHE_DATA, i);
			//getRecentMenuCacheData(userIds[i]);
			//getFavoriteMenuCacheData(userIds[i]);
			
                        ServiceI::instance().reloadObject(
                               		FAVMENU_CACHE_DATA, i);
		}
	}catch(Ice::Exception& e){
		MCE_WARN("MenuCacheManagerI::load " << e);
	}
}
MenuCacheDataPtr xce::menu::MenuCacheManagerI::findRecentMenuCacheData(Ice::Int userId) {
	MCE_DEBUG("MenuCacheManagerI::findRecentMenuCacheData, userId" << userId);
        return ServiceI::instance().findObject<MenuCacheDataPtr> (
                        RECMENU_CACHE_DATA, userId);
}

MenuCacheDataPtr xce::menu::MenuCacheManagerI::getRecentMenuCacheData(Ice::Int userId) {
//	MCE_DEBUG("MenuCacheManagerI::getRecentMenuCacheData, userId" << userId);
	return ServiceI::instance().locateObject<MenuCacheDataPtr> (
			RECMENU_CACHE_DATA, userId);
}
MenuCacheDataPtr xce::menu::MenuCacheManagerI::findFavoriteMenuCacheData(Ice::Int userId) {
	MCE_DEBUG("MenuCacheManagerI::findFavoriteMenuCacheData, userId" << userId);
        return ServiceI::instance().findObject<MenuCacheDataPtr> (
                        FAVMENU_CACHE_DATA, userId);
}

MenuCacheDataPtr xce::menu::MenuCacheManagerI::getFavoriteMenuCacheData(Ice::Int userId) {
//	MCE_DEBUG("MenuCacheManagerI::getFavoriteMenuCacheData, userId" << userId);
	return ServiceI::instance().locateObject<MenuCacheDataPtr> (
			FAVMENU_CACHE_DATA, userId);
}

bool MenuCacheManagerI::isValid(const Ice::Current&) {
	MCE_INFO("MenuCacheManagerI::isValid");
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _isValid;
}

void MenuCacheManagerI::setValid(bool newState, const Ice::Current&) {
	MCE_INFO("MenuCacheManagerI::setValid");
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_isValid = newState;
	if (_isValid) {
                ServerStateSubscriber::instance().setStatus(1);
        } else {
                ServerStateSubscriber::instance().setStatus(0);
        }

}

bool MenuCacheManagerI::belongsMe(int userid) {
	if ( cluster_ > 0 ) {
		if ( userid % cluster_ == mod_ ) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}


/*****************************************************************************/

Ice::ObjectPtr RecentMenuCacheDataFactory::create(Ice::Int userId) {	
	MCE_DEBUG("RecentMenuCacheDataFactory::create, userId " << userId);
	TimeCost tc = TimeCost::create("FactoryCreate");
	TaskManager::instance().execute(new RecentMenuCacheDataCreateTask(userId));
	return NULL;
}

void RecentMenuCacheDataCreateTask::handle(){
	MenuCacheDataPtr result = new MenuCacheData;
	map<int, vector<int> > dataMap;
	try{
		Statement sql;
		ostringstream tmp;
		tmp << "SELECT " << FIELD_UID  << "," << FIELD_RECLIST << " FROM " << REC_TABLE_NAME << userId_%TABLE_NUM
			<< " WHERE " << FIELD_UID << " = " << userId_ <<" LIMIT 1";
		sql << tmp.str();
		
		ostringstream oss;
		oss<<DB_DESC_RECPATTERN<<userId_%TABLE_NUM;
		try {
			QueryRunner(DB_DESC_RECMENUCACHE, CDbRServer, oss.str()).query(sql,  BatchRecentMenuHandler(dataMap));
		} catch (Ice::Exception& e) {
			MCE_WARN("[RecentMenuCacheDataFactoryI::create(single)] create from sql Ice::Exception : " << e.what());
		} catch (...) {
			MCE_WARN("[RecentMenuCacheDataFactoryI::create(single)] create from sql Ice::Exception ");
		}
		map<int, vector<int> >::iterator it;
		if((it = dataMap.find(userId_))!=dataMap.end()){
			result->setMenucontents(it->second);// TODO check the result
			if(MenuCacheManagerI::instance().syncCache()){
				MenuCacheManagerI::instance().invokeRecSync(userId_,result);
			}
			ServiceI::instance().addObject(result, RECMENU_CACHE_DATA, userId_);
		}
	}catch(Ice::Exception& e){
		MCE_WARN("RecentMenuCacheDataFactory::create " << e << " usrId " << userId_);
	}
}
MyUtil::ObjectResultPtr RecentMenuCacheDataFactory::create(MyUtil::IntSeq& uids) {	
	MCE_DEBUG("RecentMenuCacheDataFactory::create, userIds size:" << uids.size());
	try{	
		TaskManager::instance().execute(new RecentMenuCacheDataFactoryCreateTask(uids));
	}catch(Ice::Exception& e){
		MCE_WARN("RecentMenuCacheDataFactory::create batch " << e );
	}
	return NULL;
}

void
RecentMenuCacheDataFactoryCreateTask::handle(){
	MCE_DEBUG("RecentMenuCacheDataFactoryCreateTask::handle()");
	vector<vector<int> > uidGroup(TABLE_NUM);
	MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();	
	map<int ,vector<int> > dataMap;
	//按照uid%tablenum 进行分组
	for(size_t i=0;i < uids_.size(); ++i) {
		uidGroup[uids_[i]%TABLE_NUM].push_back(uids_[i]);
	}
	for(size_t group=0; group < TABLE_NUM; ++group) {
		size_t beginPos = 0;
		size_t endPos = 0;
		while ( beginPos < uidGroup[group].size() )
		{   
			ostringstream osIdblock;
			endPos = ((beginPos+LOAD_BATCH_SIZE) > uidGroup[group].size())?
				(uidGroup[group].size()):(beginPos+LOAD_BATCH_SIZE);
			for ( size_t i = beginPos; i < endPos; ++i ) { 
				if ( i > beginPos )  {
					osIdblock << ",";
				}   
				osIdblock << uidGroup[group][i];
			}   
			beginPos = endPos;

			Statement sql;
			ostringstream tmp;
			tmp << "SELECT " << FIELD_UID  << "," << FIELD_RECLIST << " FROM " << REC_TABLE_NAME << group 
				<< " WHERE " << FIELD_UID << " IN (" << osIdblock.str() << ")";    
			sql << tmp.str();
			ostringstream oss;
			oss<<DB_DESC_RECPATTERN<<group;
			try {
				QueryRunner(DB_DESC_RECMENUCACHE, CDbRServer, oss.str()).query(sql,  BatchRecentMenuHandler(dataMap));
			} catch (Ice::Exception& e) {
				MCE_WARN("[RecentMenuCacheDataFactoryI::create(batch)] create from sql Ice::Exception : " << e.what());
			} catch (...) {
				MCE_WARN("[RecentMenuCacheDataFactoryI::create(batch)] create from sql Ice::Exception ");
			}    
		}   

	}
	for(map<int, vector<int> >::iterator it=dataMap.begin();it!=dataMap.end(); ++it){
		MenuCacheDataPtr result = new MenuCacheData;
		result->setMenucontents(it->second);
		if(MenuCacheManagerI::instance().syncCache())
			MenuCacheManagerI::instance().invokeRecSync(it->first,result);
		objs->data.insert(make_pair<long, Ice::ObjectPtr>(it->first, result));
	}
	ServiceI::instance().getObjectCache()->addObjects(RECMENU_CACHE_DATA, objs->data);
	MCE_DEBUG("[RecentMenuCacheDataFactoryI::create(batch)] add Object size: "<<objs->data.size());
}

/*****************************************************************************/


Ice::ObjectPtr FavoriteMenuCacheDataFactory::create(Ice::Int userId) {	
	MCE_DEBUG("FavoriteMenuCacheDataFactory::create, userId " << userId);
	TaskManager::instance().execute(new FavoriteMenuCacheDataCreateTask(userId));	
	return NULL;
}
void FavoriteMenuCacheDataCreateTask::handle(){
        TimeCost tc = TimeCost::create("FavoriteMenuCacheDataCreate");
        MenuCacheDataPtr result = new MenuCacheData;
        map<int, vector<int> > dataMap;
        try{
                Statement sql;
                ostringstream tmp;
                tmp << "SELECT " << FIELD_UID  << "," << FIELD_FAVLIST << " FROM " << FAV_TABLE_NAME << userId_%TABLE_NUM
                        << " WHERE " << FIELD_UID << " = " << userId_ <<" LIMIT 1";
                sql << tmp.str();
		ostringstream oss;
		oss<<DB_DESC_FAVPATTERN<<userId_%TABLE_NUM;
                try {
                        QueryRunner(DB_DESC_FAVMENUCACHE, CDbRServer, oss.str()).query(sql,  BatchFavoriteMenuHandler(dataMap));
                } catch (Ice::Exception& e) {
                        MCE_WARN("[FavoriteMenuCacheDataFactoryI::create(single)] create from sql Ice::Exception : " << e.what());
                } catch (...) {
                        MCE_WARN("[FavoriteMenuCacheDataFactoryI::create(single)] create from sql Ice::Exception ");
                }
                map<int, vector<int> >::iterator it;
                if((it = dataMap.find(userId_))!=dataMap.end()){
                        result->setMenucontents(it->second);// TODO check the result
                        if(MenuCacheManagerI::instance().syncCache()){
                                MenuCacheManagerI::instance().invokeFavSync(userId_,result);
                        }
                       	ServiceI::instance().addObject(result, FAVMENU_CACHE_DATA, userId_); 
                }
        }catch(Ice::Exception& e){
                MCE_WARN("FavoriteMenuCacheDataFactory::create " << e << " usrId " << userId_);
        }
} 

MyUtil::ObjectResultPtr FavoriteMenuCacheDataFactory::create(MyUtil::IntSeq& uids) {	
	MCE_DEBUG("FavoriteMenuCacheDataFactory::create, userIds size:" << uids.size());
	TimeCost tc = TimeCost::create("FavoriteMenuCacheDataFactoryCreateSeq");
	try{	
		TaskManager::instance().execute(new FavoriteMenuCacheDataFactoryCreateTask(uids));
	}catch(Ice::Exception& e){
		MCE_WARN("FavoriteMenuCacheDataFactory::create batch " << e );
	}
	return new ObjectResult;
}

void
FavoriteMenuCacheDataFactoryCreateTask::handle(){
	MCE_DEBUG("FavoriteMenuCacheDataFactoryCreateTask::handle()");
	vector<vector<int> > uidGroup(TABLE_NUM);
	MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();	
	map<int ,vector<int> > dataMap;
	//按照uid%tablenum 进行分组
	for(size_t i=0;i < uids_.size(); ++i) {
		uidGroup[uids_[i]%TABLE_NUM].push_back(uids_[i]);
	}
	for(size_t group=0; group < TABLE_NUM; ++group) {
		size_t beginPos = 0;
		size_t endPos = 0;
		while ( beginPos < uidGroup[group].size() )
		{   
			ostringstream osIdblock;
			endPos = ((beginPos+LOAD_BATCH_SIZE) > uidGroup[group].size())?
				(uidGroup[group].size()):(beginPos+LOAD_BATCH_SIZE);
			for ( size_t i = beginPos; i < endPos; ++i ) { 
				if ( i > beginPos )  {
					osIdblock << ",";
				}   
				osIdblock << uidGroup[group][i];
			}   
			beginPos = endPos;

			Statement sql;
			ostringstream tmp;
			tmp << "SELECT " << FIELD_UID  << "," << FIELD_FAVLIST << " FROM " << FAV_TABLE_NAME << group 
				<< " WHERE " << FIELD_UID << " IN (" << osIdblock.str() << ")";    
			sql << tmp.str();
			ostringstream oss;
			oss<<DB_DESC_FAVPATTERN<<group;
			try {
				QueryRunner(DB_DESC_FAVMENUCACHE, CDbRServer, oss.str()).query(sql,  BatchFavoriteMenuHandler(dataMap));
			} catch (Ice::Exception& e) {
				MCE_WARN("[FavoriteMenuCacheDataFactoryI::create(batch)] create from sql Ice::Exception : " << e.what());
			} catch (...) {
				MCE_WARN("[FavoriteMenuCacheDataFactoryI::create(batch)] create from sql Ice::Exception ");
			}    
		}   

	}
	for(map<int, vector<int> >::iterator it=dataMap.begin();it!=dataMap.end(); ++it){
		MenuCacheDataPtr result = new MenuCacheData;
		result->setMenucontents(it->second);
		if(MenuCacheManagerI::instance().syncCache())
			MenuCacheManagerI::instance().invokeFavSync(it->first,result);
		objs->data.insert(make_pair<long, Ice::ObjectPtr>(it->first, result));
	}
	ServiceI::instance().getObjectCache()->addObjects(FAVMENU_CACHE_DATA, objs->data);
	MCE_DEBUG("[FavoriteMenuCacheDataFactoryI::create(batch)] add Object size: "<<objs->data.size());
}

/*****************************************************************************/
bool BatchRecentMenuHandler::handle(mysqlpp::Row& row) const {
	int uid = (int) row[FIELD_UID.c_str()];
	vector<int> list = IntSeqSerialize::unserialize(row[FIELD_RECLIST.c_str()]);
	result_.insert(make_pair<int, vector<int> >(uid, list));	
	return true;	
}

/*****************************************************************************/
bool BatchFavoriteMenuHandler::handle(mysqlpp::Row& row) const {
	int uid = (int) row[FIELD_UID.c_str()];
	vector<int> list = IntSeqSerialize::unserialize(row[FIELD_FAVLIST.c_str()]);
	result_.insert(make_pair<int, vector<int> >(uid, list));	
	return true;	
}
/*****************************************************************************/
void SyncFavCacheThread::invoke(int id,const MenuCacheDataPtr& data){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	cache_.insert(make_pair<int, Ice::ObjectPtr>(id,data));
	if(cache_.size()>0){
		mutex_.notify();
	}
}
void SyncFavCacheThread::run(){
	while(true){
		try{
			ObjectResultPtr batch = new ObjectResult;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				if(cache_.size()==0){
					mutex_.wait();
					continue;
				}
				batch->data.swap(cache_);
			}
			MenuCacheReplicaAdapter::instance().setData(batch,mod_,locatePrx_,FAVORITE);
			sleep(1);
		}catch(...){
			MCE_WARN("[SyncFavCacheThread::run] invokeSyc exception ");
		}
	}
}
/*****************************************************************************/
void SyncRecCacheThread::invoke(int id,const MenuCacheDataPtr& data){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	MCE_INFO("SyncRecCacheInvoke add a data size:"<<data->infoArray.size());
	cache_.insert(make_pair<int, Ice::ObjectPtr>(id,data));
	if(cache_.size()>0){
		mutex_.notify();
	}
}
void SyncRecCacheThread::run(){
	while(true){
		try{
			ObjectResultPtr batch = new ObjectResult;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
				if(cache_.size()==0){
					mutex_.wait();
					continue;
				}
				batch->data.swap(cache_);
			}
			MenuCacheReplicaAdapter::instance().setData(batch,mod_,locatePrx_,RECENTLY);
			sleep(1);
		}catch(...){
			MCE_WARN("[SyncRecCacheThread::run] invokeSyc exception ");
		}
	}
}
/*****************************************************************************/
