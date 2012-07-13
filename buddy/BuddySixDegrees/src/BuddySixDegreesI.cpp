#include "BuddySixDegreesI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/String.h"
#include <boost/lexical_cast.hpp>

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(BUDDY_CACHE_DATA, "BC", new BuddyCacheFactory);
	service.registerObjectCache(JOIN_CACHE_DATA, "JC", new JoinCacheFactory);
	service.registerObjectCache(ASYN_INDEX_CACHE_DATA, "AC", new AsynIndexCacheFactory);
	service.registerObjectCache(ROUT_CACHE_DATA, "RC", new RoutCacheFactory);

        TaskManager::instance().config(TASK_LEVEL_CALCULATE_ROUT, MyUtil::ThreadPoolConfig(3, 4));

	service.getAdapter()->add(&BuddySixDegreesManagerI::instance(),	service.createIdentity("M", ""));

	//启动从TokyoTyrant加载数据
	xce::buddy::BuddySixDegreesManagerI::instance().startThread();
}

/*****************************************************************************/

void BuddySixDegreesManagerI::startThread(const Ice::Current&)
{
	MCE_INFO("BuddySixDegreesManagerI::startThread::in");
	xce::buddy::BuddySixDegreesManagerI::instance().startAddFriends();
}

bool BuddySixDegreesManagerI::computeTaskStart(int host, int guest , const Ice::Current& )
{
	MCE_INFO("BuddySixDegreesManagerI::computeTaskStart, host = " << host << " guest = " << guest);
	AsynIndexDataPtr hostData = ServiceI::instance().findObject<AsynIndexDataPtr>( ASYN_INDEX_CACHE_DATA ,(long)(host) );
	if(!hostData)
	{
		friendsIndex_->asynBuildIndex(host);
	}

	AsynIndexDataPtr guestData = ServiceI::instance().findObject<AsynIndexDataPtr>(ASYN_INDEX_CACHE_DATA,(long)(guest) );
	if(!guestData)
	{
		friendsIndex_->asynBuildIndex(guest);
	}

	TaskManager::instance().execute(new CalculateRoutTask(host, guest));
	StartStopFlag::instance().start(host,guest);
	return true; 
}


bool BuddySixDegreesManagerI::computeTaskStop(int host, int guest , const Ice::Current& )
{
	MCE_INFO("BuddySixDegreesManagerI::computeTaskStop, host = " << host << " guest = " << guest);
        return StartStopFlag::instance().stop(host,guest);
        
}


MyUtil::IntSeq BuddySixDegreesManagerI::getFriendList( int hostId, int limit, const Ice::Current& current) {
	 BuddyDataPtr obj = ServiceI::instance().locateObject<BuddyDataPtr> ( BUDDY_CACHE_DATA , hostId);
	 return obj->friends;
}

MyUtil::IntSeqSeq BuddySixDegreesManagerI::checkDegreesFriends(int id1, int id2,  const Ice::Current& )
{
	std::vector<std::vector<int> >  routList;
	return routList;
}

bool BuddySixDegreesManagerI::isValid(const Ice::Current&) {
        MCE_INFO( "[isValid] " << _isValid );
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _isValid;
}

void BuddySixDegreesManagerI::setValid(bool valid,const Ice::Current&) {
        MCE_INFO( "[setValid] " << valid );
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_isValid = valid;
	if( valid ){
        	ServerStateSubscriber::instance().setStatus(1);
	} else{
        	ServerStateSubscriber::instance().setStatus(0);
	}
}

/*****************************************************************************/

void CalculateRoutTask::handle()
{
	TimeCost tc = TimeCost::create("CalculateRoutTask::run()", TimeCost::LOG_LEVEL_INFO);
	long id = CreateGlodId::getGlodId(host_, guest_);
    CalculateRoutThreadPtr calculateThread = new CalculateRoutThread(host_, guest_);
	calculateThread->start();

    HostGuestRoutPtr data = ServiceI::instance().findObject<HostGuestRoutPtr>(ROUT_CACHE_DATA,(long)(id) ); 
	if(data)
	{
		MCE_INFO("this friend host: " << host_ << "guest : "  << guest_  << " is allready in chahe");
		return ;
	}

	data= new  HostGuestRout();
	ServiceI::instance().getObjectCache()->addObject(ROUT_CACHE_DATA, (long)(id) , data);

 	//是否为一度好友
	BuddyDataPtr obj1 = ServiceI::instance().findObject<BuddyDataPtr> ( BUDDY_CACHE_DATA , host_);
	if(!obj1)
	{
		MCE_INFO("this friend host: " << host_   << "one degrees is not in chahe");
		data->setEnd(true);
		return ;
	}
	vector<int>::iterator  it= lower_bound( obj1->friends.begin(), obj1->friends.end(), guest_ ) ;
	if(it!=obj1->friends.end() && guest_ == (*it))
	{ 
		std::vector<int> rout;
		rout.push_back(host_);
		rout.push_back(guest_); 
		data->add(rout);
	}  

	BuddyDataPtr obj2 = ServiceI::instance().findObject<BuddyDataPtr> ( BUDDY_CACHE_DATA , guest_);
	if(!obj2)
	{
		MCE_INFO("this guest : "  << guest_  << "one degrees is not in chahe");
		data->setEnd(true);
		return ;
	}

	::MyUtil::IntSeq hostFriends = obj1->friends;
	::MyUtil::IntSeq guestFriends = obj2->friends;

	//是否为二度好友
	FriendsCompare twoDgrees;
	std::vector<std::vector<int> >  routList2 = twoDgrees.hasCommonFriend(host_, guest_,hostFriends,guestFriends);
	data->add(routList2);
      

	int indexa = 0;
	int indexb = 0;
	while(true)
	{
		if(StartStopFlag::instance().isHostAndGuestStop(host_,guest_)== true)
		{
			MCE_INFO("CalculateRoutTask::run,end! stop by pepole !");
			break;
		}
		ostringstream os;
		os<<"FriendsCompareSixDegreesNew():hasCommonFriend " ;
		os<< "id1:" << host_ << "id2:" << guest_ ;

		TimeCost tc = TimeCost::create(os.str() , TimeCost::LOG_LEVEL_INFO);

		//取host索引
		AsynIndexDataPtr hostData = ServiceI::instance().findObject<AsynIndexDataPtr>( ASYN_INDEX_CACHE_DATA ,(long)(host_) );
		if(!hostData)
		{
			MCE_INFO("this friend host: " << host_ << "guest : "  << guest_  << " index cache error is not in chahe");
			usleep(100000);
			continue ;
		}

		//取guest索引
		AsynIndexDataPtr guestData = ServiceI::instance().findObject<AsynIndexDataPtr>(ASYN_INDEX_CACHE_DATA,(long)(guest_) );
		if(!guestData)
		{
			MCE_INFO("this friend guest: " << host_ << "guest : "  << guest_  << " index cache error is not in chahe");
			usleep(100000);
			continue ;
		}    
		tc.step("getIndexObject");

		//取对象中最新索引
		IndexDataPtr  newFriendsA = hostData->getFreshersData(indexa);
		IndexDataPtr  newFriendsB = guestData->getFreshersData(indexb ) ;
		tc.step("getFreshers");

		if (hostData->getFinish() && guestData->getFinish()
				&& newFriendsA->indexs.empty() && newFriendsB->indexs.empty()) {
			MCE_INFO("CalculateRoutTask::run, end!!");
			break;
		}

		//放进计算线程计算
		calculateThread -> pushIndexSeq(newFriendsA, newFriendsB);
		tc.step("pushIndexSeq");

		if(!newFriendsA->indexs.empty())
		{
			indexa++; 
		}
		if(!newFriendsB->indexs.empty())
		{
			indexb++; 
		}
	}
}

/*****************************************************************************/
void CalculateRoutThread::pushIndexSeq(IndexDataPtr dataA, IndexDataPtr dataB) {

	ostringstream os;
	os << "CalculateRoutThread::pushIndexSeq, dataA.size() = " << dataA->indexs.size() << " dataB.size() = " << dataB->indexs.size() << " host =" << _host << " host = " << _host << " guest = " << _guest;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);
	
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	if (!dataA->indexs.empty()) {
		_newFriendsA.push_back(dataA);
	}
	
	if (!dataB->indexs.empty()) {
		_newFriendsB.push_back(dataB);
	}

	_mutex.notify();
}

void CalculateRoutThread::calculateRout(vector<IndexInfoIPtr> newFriendsA, vector<IndexInfoIPtr> newFriendsB) {
	ostringstream os;
	os << "CalculateRoutThread::calculateRout, seqA.size() = " << newFriendsA.size() << " seqB.size() = " << newFriendsB.size();
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);

	//启动线程合并新来索引
	AddNewFriendsThreadPtr newThreadA = new AddNewFriendsThread(_host, newFriendsA, _allFriendsA);
	AddNewFriendsThreadPtr newThreadB = new AddNewFriendsThread(_guest, newFriendsB, _allFriendsB);

	newThreadA->start();
	newThreadB->start();

	newThreadA->getThreadControl().join();
	newThreadB->getThreadControl().join();
	tc.step("AddNewFriendsThread");

	//取一度好友用来去重
	BuddyDataPtr obj1 = ServiceI::instance().findObject<BuddyDataPtr> ( BUDDY_CACHE_DATA , _host);
	if(!obj1)
	{
		MCE_INFO("CalculateRoutThread::calculateRout this friend host: " << _host  << "one degrees is not in chahe");
		return  ;
	}

	BuddyDataPtr obj2 = ServiceI::instance().findObject<BuddyDataPtr> ( BUDDY_CACHE_DATA , _guest);
	if(!obj2)
	{
		MCE_INFO("CalculateRoutThread::calculateRout this guest : "  << _guest << "one degrees is not in chahe");
		return ;
	}

	::MyUtil::IntSeq hostFriends = obj1->friends;
	::MyUtil::IntSeq guestFriends = obj2->friends;
	
	//计算六度空间好友
	FriendsCompareSixDegreesNew sixDegrees;
	std::vector<std::vector<int> >  routList6 = sixDegrees.hasCommonFriendNew(_host,_guest,hostFriends , guestFriends ,newFriendsA,newFriendsB,_allFriendsA,_allFriendsB);
	tc.step("sixDegrees");

	//把计算结果放进Cache
	HostGuestRoutPtr data = getHostGuestRout();
	if(!data)
	{
		MCE_INFO("BuddySixDegreesManagerI::getRouteList not eixist the data");
		return;
	}    
    data->add(routList6);
}

HostGuestRoutPtr CalculateRoutThread::getHostGuestRout() {
	long id = CreateGlodId::getGlodId(_host, _guest);
	return ServiceI::instance().findObject<HostGuestRoutPtr>(ROUT_CACHE_DATA,(long)(id) ); 

}

bool CalculateRoutThread::isEnd() {
	AsynIndexDataPtr hostData = ServiceI::instance().findObject<AsynIndexDataPtr>( ASYN_INDEX_CACHE_DATA ,(long)(_host) );
	AsynIndexDataPtr guestData = ServiceI::instance().findObject<AsynIndexDataPtr>(ASYN_INDEX_CACHE_DATA,(long)(_guest) );
	if(!guestData || !hostData)
	{
		MCE_INFO("CalculateRoutThread::isEnd, Data is NULL");
		return false;
	} 

	HostGuestRoutPtr data = getHostGuestRout();
	if(!data)
	{
		MCE_INFO("CalculateRoutThread::isEnd, Routdata is NULL");
		return false;
	}    

	if(hostData->getFinish() == true && guestData->getFinish() == true){
		MCE_INFO("CalculateRoutThread::isEnd, setEnd true");
		data->setEnd(true);
		StartStopFlag::instance().stop(_host, _guest);
		return true;
	}

	return false;
}

void CalculateRoutThread::run() {
	while (true) {
		{
			if(StartStopFlag::instance().isHostAndGuestStop(_host,_guest)== true)
			{
				MCE_INFO("CalculateRoutThread::run() end! stop by pepole !");
				break;
			}

			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
			if (_newFriendsA.empty() && _newFriendsB.empty()) {
	                    _mutex.timedWait(IceUtil::Time::seconds(2));			    
		    }
			
			if (!_newFriendsA.empty() || !_newFriendsB.empty()) {

				ostringstream os;
				os << "CalculateRoutThread::run(), seqA.size() = " << _newFriendsA.size() << " seqB.size() = " << _newFriendsB.size();
				TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);
				
				vector<IndexInfoIPtr> newFriendsA;
				vector<IndexInfoIPtr> newFriendsB;
				if (!_newFriendsA.empty()) {
					newFriendsA = _newFriendsA.back()->indexs;
				}

				if (!_newFriendsB.empty()) {
					newFriendsB = _newFriendsB.back()->indexs;
				}

				//计算方法
				calculateRout(newFriendsA, newFriendsB);

				if(!_newFriendsA.empty()) {
					_newFriendsA.erase(_newFriendsA.end());
				}

				if(!_newFriendsB.empty()) {
					_newFriendsB.erase(_newFriendsB.end());
				}

				MCE_INFO("CalculateRoutThread::run(), erase end! size: " << _newFriendsA.size() << "/" << _newFriendsB.size());
			} else {
				MCE_INFO("CalculateRoutThread::run(), isEnd!");
			
				if(isEnd()) {
					break;
				} else {
					continue;
				}
			}
		}
	}
	MCE_INFO("CalculateRoutThread::run(), end!");
}

/*****************************************************************************/
void CalculateRoutThread::AddNewFriendsThread::run()
{
	ostringstream os;
	os << "CalculateRoutThread::AddNewFriendsThread::run, id= " << _id;
	TimeCost tc = TimeCost::create(os.str(), TimeCost::LOG_LEVEL_INFO);

	if (_newFriends.empty()) {
		MCE_WARN("CalculateRoutThread::AddNewFriendsThread::run, is empty, id = " << _id);
		return;
	}

	for(IndexInfoSeq::iterator it = _newFriends.begin();it != _newFriends.end(); ++it) 
	{
		IndexInfoIPtr indexInfo = *it;
		IndexInfoSeq::iterator in = lower_bound(_allFriends.begin(), _allFriends.end(), indexInfo, less_entry());
		if (in != _allFriends.end() && !FriendsIndexHelper::isOptimalPath(indexInfo, *in)) {
			continue;
		}

		_allFriends.insert(in, indexInfo);
		MCE_INFO("CalculateRoutThread::AddNewFriendsThread::run, id = " << _id << " (*it) =" << (*it)->id);
	}

}

/*****************************************************************************/
RouteListPtr BuddySixDegreesManagerI::getRouteList(int host, int guest, int begin, int limit,  const Ice::Current& )
{

	long id = CreateGlodId::getGlodId(host, guest);
	TimeCost tc = TimeCost::create("getRoutList", TimeCost::LOG_LEVEL_INFO);
    RouteListPtr list = new RouteList();
	HostGuestRoutPtr data = ServiceI::instance().findObject<HostGuestRoutPtr>(ROUT_CACHE_DATA,(long)(id) ); 
	if(!data)
	{
		MCE_INFO("BuddySixDegreesManagerI::getRouteList not eixist the data");
        list->isEnd = false;
        return list;
	}

    IntSeqSeq& routList = data->getRoutList(); 
	if( limit == -1 ){
		return list;
	}

	if( limit < 0 ){
		list->isEnd = true;
		return list;
	}

	if( ((unsigned int)begin > routList.size()) || (begin < 0)){
		list->isEnd = true;
		MCE_INFO("BuddySixDegreesManagerI::getRouteList the begin > the size");
		return list;
	}

	if( (unsigned int)(begin+limit) >= routList.size()){
		list->friendsRoutList.insert( list->friendsRoutList.end(), routList.begin()+begin, routList.end());
		MCE_INFO("list->friendsRoutList.size :"<< list->friendsRoutList.size());
		list->isEnd = data->getEnd();
		return list;
	}else{
		list->friendsRoutList.insert(list->friendsRoutList.end(), routList.begin()+begin, routList.begin()+begin+limit);
		MCE_INFO("list->friendsRoutList.size :"<< list->friendsRoutList.size());
		list->isEnd = data->getEnd();
		return list;
	}
}

/*****************************************************************************/
Ice::ObjectPtr BuddyCacheFactory::create(Ice::Int id) {
        BuddyDataPtr obj = new BuddyData();
        MCE_INFO("BuddyCacheFactory::create id:" << id);
        return obj;
}

Ice::ObjectPtr JoinCacheFactory::create(Ice::Int id) {
        MCE_INFO("JoinCacheFactory::create id:" << id);
        BuddyDataIPtr obj = new BuddyDataI();
        return obj;
}

Ice::ObjectPtr AsynIndexCacheFactory::create(Ice::Int id) {
        MCE_INFO("AsynIndexCacheFactory::create id:" << id);
        AsynIndexDataPtr obj = new AsynIndexData(id);
        return obj;
}

Ice::ObjectPtr RoutCacheFactory::create(Ice::Int id) {
        MCE_INFO("RoutCacheFactory::create id:" << id);
        HostGuestRoutPtr obj = new HostGuestRout();
        return obj;
}

/*****************************************************************************/

void AddFriendsThread::run()
{

	TCRDB *rdb;
    int ecode;

	/* create the object */
	rdb = tcrdbnew();
	char *key=0;
	char *value=0;

	if(!tcrdbopen(rdb, host_.c_str(), port_))
	{
		ecode = tcrdbecode(rdb);
		MCE_INFO( "open error:" << tcrdberrmsg(ecode));
	}

	tcrdbiterinit(rdb);
	int i = 0;

	while((key = tcrdbiternext2(rdb)) != NULL)
	{
		try{
			value = tcrdbget2(rdb, key);
		}catch(...)
		{
			MCE_WARN("[Exception] AddFriendsThread::run::tcrdbget2 get NULL");
		}
		if(value) 
		{
			i++; 
			string friendstr(value);
			vector<string> friends=MyUtil::StrUtil::split(friendstr, ":");

			BuddyDataPtr data = new BuddyData();
			for(vector<string>::iterator it = friends.begin();it!=friends.end(); ++it ) 
			{
				data->friends.push_back(atoi(it->c_str()));
			}
			if(i%100 == 0 ) 
			{
				MCE_INFO("the key : "<< key << "the friends.size :"<< friends.size());
				MCE_INFO("the host : "<< host_ <<"port:" << port_ << "  load id sum :"<<  i);
			}   

			ServiceI::instance().getObjectCache()->addObject(BUDDY_CACHE_DATA, (long)atoi(key) , data);

			//this is very important to build the back index from user to Node
			//the fiend is key and the user(who's friend.size>1000) is the value;
			if(friends.size()> 1000 )
			{
				{
					IceUtil::RWRecMutex::WLock  lock(mutex_);
					ids_.insert(atoi(key));

				}
				int id = atoi(key);
				for(std::vector<string>::iterator it = friends.begin();it!=friends.end();++it)
				{
					int friendid = atoi(it->c_str());
					BuddyDataIPtr data = ServiceI::instance().findObject<BuddyDataIPtr>(JOIN_CACHE_DATA,(long)(friendid) ); 
					if(data == 0)
					{
						data = new BuddyDataI();
						data->addFriend(id);
						ServiceI::instance().getObjectCache()->addObject(JOIN_CACHE_DATA, (long)(friendid) , data);
					}else
					{
						BuddyDataIPtr data = ServiceI::instance().locateObject<BuddyDataIPtr> (JOIN_CACHE_DATA ,(long)friendid);
						data->addFriend(id);
						ServiceI::instance().getObjectCache()->addObject(JOIN_CACHE_DATA, (long)(friendid) , data);
					} 
				}
			}
			free(value);
		}
		free(key);
	}

	MCE_INFO("the host : "<< host_ <<"port:" << port_ << " load id size:"<<  i);
	MCE_INFO( "the host : "<< host_ <<"port:" << port_ <<" ids > 1000 size"<<  ids_.size());

	if(!tcrdbclose(rdb)){
		ecode = tcrdbecode(rdb);
		fprintf(stderr, "close error: %s\n", tcrdberrmsg(ecode));
	}

	/* delete the object */
	tcrdbdel(rdb);
	MCE_INFO(" AddFriendsThread::end ");
}

