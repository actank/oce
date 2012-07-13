#include "PageFansI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "PageFansReloaderAdapter.h"

using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::buddy;

void MyUtil::initialize() {
        ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(&PageFansManagerI::instance(), service.createIdentity(
                        "M", ""));
        service.registerObjectCache( PAGEFANS_DATA, "ES", &PageFansServantFactory::instance());


        //----------------------------HA----------------------------------------------

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("PageFansCache.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PageFansCache.Interval", 5);

		ServerStateSubscriber::instance().initialize("ControllerPageFansCache",
                        &PageFansManagerI::instance(), mod, interval);

        TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1, 10));
        TaskManager::instance().config(TASK_LEVEL_UPDATE, ThreadPoolConfig(1, 1));

        service.registerXceObserver(new ServiceSchemaI);

}

void PageFansManagerI::setValid(bool valid, const Ice::Current& current) 
{
		ostringstream oss;
		oss << "PageFansManagerI::setValid -> "<<valid<< " " << current.con->toString().substr(current.con->toString().find("remote",25));
		TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
		IceUtil::RWRecMutex::WLock lock(_validMutex);
		_valid = valid;
		ServerStateSubscriber::instance().setStatus((int)valid);
}

bool PageFansManagerI::isValid( const Ice::Current& current) 
{
		IceUtil::RWRecMutex::RLock lock(_validMutex);
		return _valid;
}


PageFansObjIPtr PageFansManagerI::locatePageFans(::Ice::Int id) {
			return ServiceI::instance().locateObject<PageFansObjIPtr> ( PAGEFANS_DATA , id);
}

PageEntryPtr PageFansManagerI::getFan(::Ice::Int pageId, ::Ice::Int userId,
                        const Ice::Current& current){
		MCE_INFO("PageFansManagerI::getFan: pageId" << pageId);
		PageFansObjIPtr obj = locatePageFans(pageId);
		return obj->getFan(userId);
}

PageEntryList PageFansManagerI::getFans( ::Ice::Int pageId,::Ice::Int begin , ::Ice::Int limit,
                        const Ice::Current& current){
		
		MCE_INFO("PageFansManagerI::getFans: pageId" << pageId);
  		PageFansObjIPtr obj = locatePageFans(pageId);
		PageEntryList list =  obj->realfans;
		PageEntryList result; 
		if( limit == -1 ){
				return list;
		}
		if( ((unsigned int)begin > list.size()) || (begin < 0)){
				MCE_INFO("PageFansManagerI::getFans the begin > the size or begin < 0");
				return result;
		}
		if( (unsigned int)(begin+limit) >= list.size()){
				result.insert(result.end(),list.begin()+begin,list.end());
				MCE_INFO("PageFansManagerI::getFans :"<< result.size());
				return result;
		}else{
				result.insert(result.end(),list.begin()+begin,list.end()+limit);
				MCE_INFO("PageFansManagerI::getFans :"<< result.size());
				return result;

		}

}
PageEntryList PageFansManagerI::getFansByUserIds(::Ice::Int pageId,const MyUtil::IntSeq&  userIds,
                        const Ice::Current& current){
		MCE_INFO("PageFansManagerI::getFansByUserIds: pageId" << pageId << " iDs.size " << userIds.size());
  		PageFansObjIPtr obj = locatePageFans(pageId);
		PageEntryList list =  obj->realfans;
		PageEntryList result ;
	    for(MyUtil::IntSeq::const_iterator it=userIds.begin();it!=userIds.end();it++)
		{
			PageEntryPtr pe = obj->getFan(*it);
			if(pe)
				result.push_back(pe);
		}	
        MCE_INFO("PageFansManagerI::getFansByUserIds result.size " << result.size() );
		return result;
}

PageEntryList PageFansManagerI::getFansByState(::Ice::Int pageId, ::Ice::Int state, ::Ice::Int begin,  ::Ice::Int limit,
                        const Ice::Current& current){
		MCE_INFO("PageFansManagerI::getFansByState: pageId" << pageId << " state " << state);
  		PageFansObjIPtr obj = locatePageFans(pageId);
		PageEntryList temp ;
		PageEntryList list ;
		if(state == NORMAL)
		{
				list =  obj->getRealFans();
		}
		else{
				temp =  obj->getOtherFans();
				for(PageEntryList::iterator it = temp.begin(); it!=temp.end();++it)
				{
						if((*it)->state == state)
						{
							list.push_back(*it);		
						}
				}
		}

		
		PageEntryList result;
		if( limit == -1 ){
				return list;
		}
		if( ((unsigned int)begin > list.size()) || (begin < 0)){
				MCE_INFO("PageFansManagerI::getFanByStates the begin > the size or begin < 0");
				return result;
		}
		if( (unsigned int)(begin+limit) >= list.size()){
				for(PageEntryList::iterator it = list.begin()+begin; it!=list.end();++it)
				{
						result.push_back(*it);
				}
				MCE_INFO("PageFansManagerI::getFansState size:"<< result.size());
				return result;
		}else{
				for(PageEntryList::iterator it = list.begin()+begin; it!=list.end()+limit;++it)
				{
						result.push_back(*it);
				}
				MCE_INFO("PageFansManagerI::getFansState size :"<< result.size());
				return result;

		}


}

::Ice::Int PageFansManagerI::getFansCountByState(::Ice::Int pageId, ::Ice::Int state,
                        const Ice::Current& current){

		MCE_INFO("PageFansManagerI::getFansCountByState: pageId : " << pageId << " : " << state);
		PageFansObjIPtr obj = locatePageFans(pageId);
	
		PageEntryList temp ;
		PageEntryList list ;
		if(state == NORMAL)
		{
				list =  obj->getRealFans();
		}
		else{
				temp =  obj->getOtherFans();
				for(PageEntryList::iterator it = temp.begin(); it!=temp.end();++it)
				{
						if((*it)->state == state)
						{
							list.push_back(*it);		
						}
				}
		}

		return list.size();
}

bool PageFansManagerI::isFan(::Ice::Int pageId, ::Ice::Int userId,
                        const Ice::Current& current){
	MCE_INFO("PageFansManagerI::isFan: pageId:userId" << pageId << ":" << userId);
	PageFansObjIPtr obj = locatePageFans(pageId);
	return obj->isFan(userId);

}

::Ice::Int PageFansManagerI::addFan(const PageEntryPtr& pe, const Ice::Current& current){
	MCE_INFO("PageFansManagerI::addFan: pageId:userId" << pe->pageId << ":" << pe->userId);
	PageFansObjIPtr obj = locatePageFans(pe->pageId);
	return obj->addFan(pe);

}

::Ice::Int PageFansManagerI::removeFan(::Ice::Int pageId, ::Ice::Int userId,
                        const Ice::Current& current){

	MCE_INFO("PageFansManagerI::removeFan: pageId:userId" << pageId << ":" << userId);
	PageFansObjIPtr obj = locatePageFans(pageId);
	return obj->removeFan(userId);
}

::Ice::Int PageFansManagerI::updateFanWithState(const PageEntryPtr& pe, const Ice::Current& current){
	MCE_INFO("PageFansManagerI::updateFan: pageId:userId" << pe->pageId << ":" << pe->userId);
	PageFansObjIPtr obj = locatePageFans(pe->pageId);
	return obj->updateFan(pe);
}

::Ice::Int PageFansManagerI::updateFansWithState(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState,
                        const Ice::Current& current){
	MCE_INFO("PageFansManagerI::updateFansWithState: oriState" << oriState << " tarState " << tarState);
	TaskManager::instance().execute(new UpdateTask(pageId,oriState,tarState));
	return 1;

}

::Ice::Int PageFansManagerI::addBlock(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp,
                        const Ice::Current& current){

	MCE_INFO("PageFansManagerI::addBlock: userId " << userId << " operator " << operatorId );
	PageFansObjIPtr obj = locatePageFans(pageId);
	PageEntryPtr pe = new PageEntry();
	pe->userId = userId;
	pe->state = BLOCK;
	pe->pageId = pageId;
	pe->operatorId = operatorId;
	pe->addTime = timestamp ;
    return obj->addBlock(pe);
}

::Ice::Int PageFansManagerI::removeBlock(::Ice::Int pageId, ::Ice::Int userId,
                        const Ice::Current& current){
	
		MCE_INFO("PageFansManagerI::removeBlock: pageId:userId" << pageId << ":" << userId);
		PageFansObjIPtr obj = locatePageFans(pageId);
		return obj->removeBlock(userId);
}

::Ice::Int PageFansManagerI::findBlock(::Ice::Int pageId, ::Ice::Int userId,
                        const Ice::Current& current){
		MCE_INFO("PageFansManagerI::removeBlock: pageId:userId" << pageId << ":" << userId);
		PageFansObjIPtr obj = locatePageFans(pageId);
		return obj->findBlock(userId);

}

PageEntryList PageFansManagerI::getBlockList(::Ice::Int pageId, const Ice::Current& current){
		MCE_INFO("PageFansManagerI::getBlockList: pageId:userId" << pageId );
		PageFansObjIPtr obj = locatePageFans(pageId);
		return obj->getBlockFans();
}

/*****************************************************************************/
Ice::ObjectPtr PageFansServantFactory::create(Ice::Int id) {
        MCE_INFO("PageFansServantFactory::create, id " << id);
        PageFansReloaderAdapter::instance().reload(id);
        return NULL;
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
}

void PageFansManagerI::setData(const ObjectResultPtr& data, const Ice::Current& current) 
{
		ostringstream oss;
		oss << "PageFansManagerI::setData -> size: "<< data->data.size()<< " " << current.con->toString().substr(current.con->toString().find("remote",25));
		TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_DEBUG);
		TaskManager::instance().execute(new FillTask(data));

}

void FillTask::handle() 
{
		ObjectResultPtr results = new ObjectResult;
 	
			MCE_INFO("FILL TASK id : "  );
		for(MyUtil::ObjectMap::iterator it = _data->data.begin();it != _data->data.end();++it)
		{
				int id= it->first;
				PageFansObjIPtr obj = new PageFansObjI();
				int PRODUCE_BATCH_SIZE = 3000;
				int minId = 0;
				results->data.insert(make_pair(id, obj));


				while(true)
				{
						{
								PageFansResultHandlerForCache handler(results );
								Statement sql;

								sql << "SELECT id, user_id, state , UNIX_TIMESTAMP(add_time) As  add_time , operator , page_id  FROM " <<
										PageFansHelper::getTableName("page_fans", id) << " WHERE page_id = '" << id
										<<"'and state = 0 and user_id > " << minId  <<" ORDER BY user_id LIMIT " << PRODUCE_BATCH_SIZE ;
								QueryRunner( DB_PAGE_FANS , CDbRServer ).query(sql, handler);
						}
						
						PageEntryList list =  obj->realfans;
						if ( list.size() == 0)
						{
								MCE_INFO("no  state =0  end");
								break ;
						}
						PageEntryPtr pe = list.back();

						int lastId = pe->userId;

						if( minId == lastId )
						{
								MCE_INFO("add state =0 end");
								break ;
						
						}else
						{
								minId = lastId;
						}
						MCE_INFO("add state =0   << pageId : " <<  id  << "  minId : " << minId);

				}

				{
						PageFansResultHandlerForCache handler(results );
						Statement sql;

						sql << "SELECT id, user_id, state , UNIX_TIMESTAMP(add_time) As  add_time , operator , page_id  FROM " <<
								PageFansHelper::getTableName("page_fans", id) << " WHERE page_id = '" << id
								<<"'and state <> 0 ORDER BY user_id ";
						QueryRunner( DB_PAGE_FANS , CDbRServer ).query(sql, handler);
				}

				{
						PageEntryBlockResultHandlerForCache handler(results );
						Statement sql;

						sql << "SELECT id, userid, pageid , executor ,UNIX_TIMESTAMP(timestamp) As  timestamp  FROM " <<  TABLE_PAGE_FANS_BLOCK << " WHERE pageid = '" << id
								<<"' ORDER BY userid ";

						QueryRunner( DB_PAGE_FANS_BLOCK , CDbRServer ).query(sql, handler);

				}
			MCE_INFO("FILL TASK id : " <<  it->first << " size : " <<obj->realfans.size() );


		}
	/*
		for(MyUtil::ObjectMap::iterator it = _data->data.begin();it != _data->data.end();++it) 
		{  
				PageFansObjIPtr obj = new PageFansObjI(PageFansObjPtr::dynamicCast(it->second));
				results->data.insert(make_pair(it->first, obj));
		}
*/
		MyUtil::ServiceI::instance().getObjectCache()->addObjects(PAGEFANS_DATA , results->data); 
}

void UpdateTask::handle() 
{
		PageFansObjIPtr obj = ServiceI::instance().locateObject<PageFansObjIPtr> ( PAGEFANS_DATA , _pageId);
		obj->updateAllState(_oriState , _tarState);
}
