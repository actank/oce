#include "FriendRankCacheI.h"
#include "FriendRankLoaderAdapter.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::socialgraph;

// MyUtil::initialize {{{
void MyUtil::initialize()
{
	MCE_INFO("FriendRankCacheI MyUtil::initialize().");
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FriendRankCacheI::instance(),
			service.createIdentity("FriendRankCache", ""));
	service.registerObjectCache(FRIEND_RANK_DATA, "FRIENDRANK", new FriendRankFactoryI, false);

	// 从服务配置文件获取参数
	Ice::CommunicatorPtr& ic = service.getCommunicator();
	int mod         = ic->getProperties()->getPropertyAsInt("FriendRankCache.Mod");
	string endpoint = ic->getProperties()->getProperty("FriendRankCache.Endpoint");
	int interval    = ic->getProperties()->getPropertyAsIntWithDefault(
			"FriendRankCache.Interval", 5);
	FriendRankCacheI::instance().Bootstrap(endpoint, mod);

	// 向Controller注册
	// _isValid=false，Controller会触发Loader的create函数进行预加载
	ServerStateSubscriber::instance().initialize("ControllerFriendRankCache",
			&FriendRankCacheI::instance(), mod, interval);

	// 设置线程类消费线程的最大值和最小值，可根据任务执行时间长度来调整
	TaskManager::instance().config(TASK_LEVEL_FRIEND_RANK_CACHE_FILL, ThreadPoolConfig(1,1));
	TaskManager::instance().config(TASK_LEVEL_FRIEND_RANK_CACHE_LOAD, ThreadPoolConfig(1,1));
	service.registerXceObserver(new ServiceSchemaI);
}// }}}

// FriendRankCacheI::Bootstrap {{{
// 进行FriendRankCacheI的初始化工作
void FriendRankCacheI::Bootstrap(const string& endpoint, int mod)
{
	MCE_INFO( "[FriendRankCacheI::Bootstrap] endpoint=" << endpoint << " mod=" << mod );
	_endpoint = endpoint;
	_mod = mod;
}// }}}

// FriendRankFactoryI::create(Ice::Int id) {{{
Ice::ObjectPtr FriendRankFactoryI::create(Ice::Int userId)
{
	MCE_INFO("FriendRankFactoryI::create single -> id: "<<userId);
	// 直接从数据库取数据
	FriendRankDataPtr result = new FriendRankData;
	//result->userId = 0;
	com::xiaonei::xce::Statement sql;
	sql	<< "SELECT user_id,rank FROM friend_rank WHERE user_id=" << userId;
	com::xiaonei::xce::QueryRunner("friend_rank", com::xiaonei::xce::CDbRServer).query(
			sql, FriendRankSingleResultHandler(result));
	
	// 策略：这里即使userId没有数据也要向Cache加载一个空对象，这样下次就会更快
	// 调用Loader加载该Id (因为服务可能有多份要更新Cache)
	TaskManager::instance().execute(new LoadTask(userId));
	return result;
}// }}}

// FriendRankSingleResultHandler::handle {{{
bool FriendRankSingleResultHandler::handle(mysqlpp::Row& row) const 
{
	//_result->userId = (int)row["user_id"];
	// if using binary
	const int num = row["rank"].length() / sizeof(int);
	_result->data.reserve(num/2);
	const char* p = row["rank"].c_str();
	//MCE_INFO("#######################" << num);
	RankData elem;
	for (int i=0; i<num; i+=2)
	{
		elem.userid = *(int*)p;  p += sizeof(int);
		elem.weight = *(int*)p;  p += sizeof(int);
		//MCE_INFO(userid<<weight);
		_result->data.push_back(elem);
	}
	//MCE_INFO("#######################" << num);
	return true;
}// }}}

// FriendRankCacheI::GetRank {{{
RankSeq FriendRankCacheI::GetRank(int userId, const Ice::Current & cur)
{
	MCE_INFO("FriendRankCacheI::GetRank( " << userId << " )");
	// 方案1：若Cache中不存在，会自动调用FriendRankFactoryI::create
	//FriendRankDataPtr rank = 
	//	ServiceI::instance().locateObject<FriendRankDataPtr>(FRIEND_RANK_DATA, userId);

	// 方案2：若Cache中不存在，就直接返回空对象。前提DB与Cache保持一致
	FriendRankDataPtr rank =
		ServiceI::instance().findObject<FriendRankDataPtr>(FRIEND_RANK_DATA, userId);
	if (!rank)
	{
		return RankSeq();
		//FriendRankDataPtr result = new FriendRankData;
		//com::xiaonei::xce::Statement sql;
		//sql << "SELECT user_id,rank FROM friend_rank WHERE user_id=" << userId;
		//com::xiaonei::xce::QueryRunner("friend_rank", com::xiaonei::xce::CDbRServer).query(
		//		sql, FriendRankSingleResultHandler(result));
		//return result->data;
	}
	else
		return rank->data;
}// }}}
// FriendRankCacheI::GetRevRank {{{
map<int,int> FriendRankCacheI::GetRevRank(int hostId, const MyUtil::IntSeq& guestIds,
		const Ice::Current& cur)
{
	//struct timeval start;
	//gettimeofday(&start, NULL);

	MCE_INFO("FriendRankCacheI::GetRevRank("<<hostId<<",guestIds.size="<<guestIds.size()<<")");
	map<int,int> revrank;
	RankData target;
	int guestId = 0;

	for (vector<int>::const_iterator iter = guestIds.begin();
			 iter != guestIds.end(); ++iter)
	{
		guestId = *iter;
		// 方案1：若Cache中找不到数据，会自动调用FriendRankFactoryI::create。
		//FriendRankDataPtr rank = ServiceI::instance().locateObject<FriendRankDataPtr>(
		//		FRIEND_RANK_DATA, guestId);
		//MCE_INFO("get rank of " << guestId << "\tsize=" << rank->data.size());
		
		// 方案2：若Cache中不存在，就直接返回空对象。前提DB与Cache保持一致
		FriendRankDataPtr rank =
			ServiceI::instance().findObject<FriendRankDataPtr>(FRIEND_RANK_DATA, guestId);
		if (!rank)
			continue;

		target.userid = hostId;
		vector<RankData>::iterator kIter = 
			lower_bound(rank->data.begin(), rank->data.end(), target, RankDataComp());
		if(kIter!=rank->data.end() && kIter->userid==target.userid)
		{
			revrank.insert(make_pair<int,int>(guestId, kIter->weight));
		}
	}

	//MCE_INFO("--------------------");
	//for (map<int,int>::const_iterator iter = revrank.begin();
	//		iter != revrank.end(); ++iter)
	//{
	//	MCE_INFO(iter->first << " : " << iter->second);
	//}

	//struct timeval end;
	//gettimeofday(&end, NULL);
	//double timeuse = 1000000*(end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
	//MCE_INFO("FriendRankCacheI::GetRevRank() Time used "<<timeuse/1000000<<"second.");
	return revrank;
}// }}}

// FriendRankCacheI::SetRank {{{
void FriendRankCacheI::SetRank(int userId, const FriendRankDataPtr& obj, 
		const Ice::Current & cur)
{
	MCE_INFO("FriendRankCacheI::SetRank( " <<userId << " )");
	ServiceI::instance().addObject(obj, FRIEND_RANK_DATA, userId);
}// }}}
// FriendRankCacheI::SetRanks {{{
void FriendRankCacheI::SetRanks(const ::MyUtil::ObjectResultPtr& rankData, 
		const Ice::Current & cur)
{
	MCE_INFO( "[FriendRankCacheI::setRanks] size=" << rankData->data.size());
	setData(rankData, cur);
	//ServiceI::instance().getObjectCache()->addObjects(FRIEND_RANK_DATA, rankData->data);
}// }}}

// FriendRankCacheI::setData {{{
// Cache-loader内部机制调用
void FriendRankCacheI::setData(const ::MyUtil::ObjectResultPtr& rankData,
		const Ice::Current& cur)
{
	MCE_INFO( "[FriendRankCacheI::setData] size=" << rankData->data.size());
	TaskManager::instance().execute(new FillTask(rankData));
}// }}}
// Load {{{
void FriendRankCacheI::Load(const RankSeqMap& datas, 
		const Ice::Current& cur)
{
	// RankSeqMap <<==>> map<int,vector<int> >
	MCE_INFO( "[FriendRankCacheI::Load] size=" << datas.size());
	MyUtil::ObjectResultPtr allrank = new MyUtil::ObjectResult;
	RankData elem;
	int hostId = 0;

	for (RankSeqMap::const_iterator iter = datas.begin();
			iter != datas.end(); ++iter)
	{
		hostId = iter->first;
		const vector<int>& rank = iter->second;
		int num = rank.size();
		assert(num % 2 == 0);

		FriendRankDataPtr myrank = new FriendRankData;
		myrank->data.reserve(num/2);
		for (int i=0; i<num; /*++i*/)
		{
			elem.userid = rank.at(i++);
			elem.weight = rank.at(i++);
			myrank->data.push_back(elem);
		}
		allrank->data.insert(make_pair(hostId,myrank));
	}

	setData(allrank, cur);
}// }}}

// FriendRankCacheI::isValid {{{
bool FriendRankCacheI::isValid(const Ice::Current&)
{
	MCE_INFO("[FriendRankCacheI::isValid] " << _isValid );
	IceUtil::RWRecMutex::RLock lock(_validMutex);
	return _isValid;
}// }}}

// FriendRankCacheI::setValid {{{
void FriendRankCacheI::setValid(bool valid,const Ice::Current&)
{
	MCE_INFO( "[FriendRankCacheI::setValid] " << valid );
	IceUtil::RWRecMutex::WLock lock(_validMutex);
	_isValid = valid;
	if( valid ){
		ServerStateSubscriber::instance().setStatus(1);
	} else{
		ServerStateSubscriber::instance().setStatus(0);
	}
}// }}}


// FillTask::handle {{{
void FillTask::handle()
{
	//cout << "3 ###########################################" << endl;
	//for (MyUtil::ObjectMap::iterator iter = _rankData->data.begin();
	//		iter != _rankData->data.end(); ++iter)
	//{
	//	cout << iter->first << " : ";
	//	const FriendRankDataPtr& one = FriendRankDataPtr::dynamicCast(iter->second);
	//	for (map<int,int>::const_iterator oneIter = one->data.begin();
	//			oneIter != one->data.end(); ++oneIter)
	//	{
	//		cout << "<" << oneIter->first << "," << oneIter->second << ">";
	//	}
	//	cout << endl;
	//}
	//cout << "3 ###########################################" << endl;

	ServiceI::instance().getObjectCache()->addObjects(FRIEND_RANK_DATA, _rankData->data);
	MCE_INFO("FillTask done. Task size / ObjectCache size : "
			<< _rankData->data.size() << " / "
			<< ServiceI::instance().getObjectCache()->locateCategory(FRIEND_RANK_DATA)->size());
}// }}}

// LoadTask::handle {{{
void LoadTask::handle()
{
	FriendRankLoaderAdapter::instance().reload(_userId);
}// }}}


// ServiceSchemaI::configure {{{
// 小白那边JSP监控使用
void ServiceSchemaI::configure(const Ice::PropertiesPtr& props)
{
	TaskManager::instance().config(props, 
			"Fill", TASK_LEVEL_FRIEND_RANK_CACHE_FILL, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, 
			"Load", TASK_LEVEL_FRIEND_RANK_CACHE_LOAD, ThreadPoolConfig(1,1));
}// }}}

