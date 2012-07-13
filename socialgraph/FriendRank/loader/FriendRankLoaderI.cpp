#include "FriendRankCacheAdapter.h"
#include "FriendRankLoaderI.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::cacheloader;
using namespace xce::socialgraph;
using namespace std;

// MyUtil::initialize {{{
void MyUtil::initialize()
{
	MCE_INFO("FriendRankLoaderI MyUtil::initialize().");
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FriendRankLoaderI::instance(), 
			service.createIdentity("FriendRankLoader", ""));

	// 从服务配置文件获取参数
	Ice::CommunicatorPtr& ic = service.getCommunicator();
	string ctlEndpoint   = ic->getProperties()->getPropertyWithDefault(
			"FriendRankCache.Controller", "ControllerFriendRankCache");
	int cInterval        = ic->getProperties()->getPropertyAsIntWithDefault(
			"Subscriber.Controller.Interval", 60);
	int timeout          = ic->getProperties()->getPropertyAsIntWithDefault(
			"FriendRankCache.Proxy.Timeout", 300);
	int produceBatchSize = ic->getProperties()->getPropertyAsIntWithDefault(
			"FriendRankLoader.ProduceBatchSize", 10000);
	int consumeBatchSize = ic->getProperties()->getPropertyAsIntWithDefault(
			"FriendRankLoader.ConsumeBatchSize", 3000);
	int writeBatchSize   = ic->getProperties()->getPropertyAsIntWithDefault(
			"FriendRankLoader.WriteBatchSize", 1000);
	int threadSize       = ic->getProperties()->getPropertyAsIntWithDefault(
			"FriendRankLoader.ThreadSize", 10);

	// 预加载服务，由BatchUsersIdProducer产生ID，采用<min,max>方式加载
	CacheLoader<FriendRankCachePrx>::instance().initialize( 
			new BatchUsersIdProducer,
			&FriendRankLoaderFactory::instance(),
			ctlEndpoint,
			cInterval,
			timeout,
			produceBatchSize,
			consumeBatchSize,
			writeBatchSize,
			threadSize );
}// }}}


// FriendRankLoaderFactory::create(Ice::Int userId) {{{
Ice::ObjectPtr FriendRankLoaderFactory::create(Ice::Int userId)
{
	MCE_INFO("FriendRankLoaderI::create single -> id: "<<userId);

	MyUtil::IntSeq ids;
	ids.push_back(userId);
	MyUtil::ObjectResultPtr results = create(ids);

	return results->data[userId];
}// }}}

// FriendRankLoaderFactory::create(const MyUtil::IntSeq& ids) {{{
ObjectResultPtr FriendRankLoaderFactory::create(const MyUtil::IntSeq& ids)
{
	MCE_DEBUG("FriendRankLoaderI::create group -> id size: "<<ids.size());
	MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult;
	if (ids.size()<=0)
		return results;

	Statement sql;
	sql << "SELECT user_id,rank FROM friend_rank"
		<< " WHERE user_id in (" << ids[0];
	for (size_t i=1; i < ids.size(); ++i)
	{
		sql << "," << ids[i];
	}
	sql << ")";

	try {
		QueryRunner("friend_rank", CDbRServer).query(
				sql, FriendRankBatchResultHandler(results));
	} catch (Ice::Exception& e) {
		MCE_WARN("[create] Ice::Exception " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[create] std::excetpion " << e.what());
	} catch (...) {
		MCE_WARN("[create] exception");
	}

	return results;
}// }}}

// FriendRankLoaderFactory::create(Ice::Int minId, Ice::Int maxId) {{{
ObjectResultPtr FriendRankLoaderFactory::create(Ice::Int minId, Ice::Int maxId)
{
	MCE_DEBUG("FriendRankLoaderI::create group -> ids : ["<<minId<<","<<maxId<<"]");
	MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult;

	// consume [minId,maxId], see CacheLoader.h:consume()
	Statement sql;
	sql << "SELECT user_id,rank FROM friend_rank "
		<< "WHERE user_id>=" << minId << " AND user_id<=" << maxId;
	try {
		QueryRunner("friend_rank", CDbRServer).query(
				sql, FriendRankBatchResultHandler(results));
	} catch (Ice::Exception& e) {
		MCE_WARN("[create] Ice::Exception " <<e.what());
	} catch (std::exception& e) {
		MCE_WARN("[create] std::excetpion " <<e.what());
	} catch (...) {
		MCE_WARN("[create] exception");
	}

	//cout << "2 ###########################################" << endl;
	//for (map<long,ObjectPtr>::const_iterator iter = results->data.begin();
	//		iter != results->data.end(); ++iter)
	//{
	//	const FriendRankDataPtr& one = FriendRankDataPtr::dynamicCast(iter->second);
	//	cout << iter->first << " : " << one->userId << " : ";
	//	for (map<int,int>::const_iterator oneIter = one->data.begin();
	//			oneIter != one->data.end(); ++oneIter)
	//	{
	//		cout << "<" << oneIter->first << "," << oneIter->second << ">";
	//	}
	//	cout << endl;
	//}
	//cout << "2 ###########################################" << endl;
	return results;
}// }}}

// FriendRankBatchResultHandler::handle {{{
bool FriendRankBatchResultHandler::handle(mysqlpp::Row& row) const
{
	// rank field : [id][weight][id2][weight]...
	// 数据库中已经是排序后的数据
	RankData elem;
	// rank's type is mysql blob field
	int hostId = (int)row["user_id"];
	const int num = row["rank"].length() / sizeof(int);
	FriendRankDataPtr result = new FriendRankData;
	result->data.reserve(num/2);

	const char* p = row["rank"].c_str();
	for (int i=0; i<num; i+=2)
	{
		elem.userid = *(int*)p;  p += sizeof(int);
		elem.weight = *(int*)p;  p += sizeof(int);
		result->data.push_back(elem);
	}

	_results->data.insert(make_pair(hostId,result));

	//cout << "1 ###########################################" << endl;
	//for (map<int,int>::const_iterator oneIter = result->data.begin();
	//		oneIter != result->data.end(); ++oneIter)
	//{
	//	cout << "<" << oneIter->first << "," << oneIter->second << ">" << endl;
	//}
	//cout << "1 ###########################################" << endl;

	return true;
}// }}}

// FriendRankLoaderI::reload {{{
void FriendRankLoaderI::reload(::Ice::Int id, const Ice::Current& current)
{
	MCE_INFO("[FriendRankLoaderI::reload] id=" << id);
	TaskManager::instance().execute( new LoadTask( id ) );
}// }}}

// LoadTask::handle {{{
void LoadTask::handle()
{
	Ice::ObjectPtr obj = FriendRankLoaderFactory::instance().create(_id);
	FriendRankDataPtr result = FriendRankDataPtr::dynamicCast(obj);
	if (result->data.size()>0)
	{
		FriendRankCacheAdapter::instance().SetRank(_id, result);
	}
}// }}}

