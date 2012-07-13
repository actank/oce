#include "ILikeItManagerI.h"
#include "UtilCxx/src/Allocator.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "OceCxxAdapter/src/ILikeItCacheOldAdapter.h"
#include "OceCxxAdapter/src/ILikeItCacheAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::platform::storage;
using namespace xce::usercache;
using namespace xce::clusterstate;
using namespace xce::ilikeit;
using namespace xce::log;

// =============== MyUtil::initialize ====================================
void MyUtil::initialize() {

	ILikeItManagerI::instance().initialize();
//	ILikeItCacheOldAdapter::instance();		// 兼容通知旧Cache
	ILikeItCacheAdapter::instance();
	HeadUrlUtil::instance();

	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&ILikeItManagerI::instance(), service.createIdentity("M", ""));

	TaskManager::instance().config(TASK_LEVEL_ADD, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_REMOVE, MyUtil::ThreadPoolConfig(1, 10));
	
	ActiveTrack::instance().initialize();
	
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ILikeItLogic.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ILikeItLogic.Interval", 5);
	// 加入向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerILikeItLogic", 
			&ILikeItManagerI::instance(), mod, interval);
}


// =============== ILikeItManagerI主类 ===================================
void ILikeItManagerI::initialize() {
	_invokeAdd.start(128*1024).detach();
	_invokeRemove.start(128*1024).detach();
}

void ILikeItManagerI::addLike(const LikeshipPtr& ship, const Ice::Current& crt) {

	// 同步ILikeItCache	
//	ILikeItCacheOldAdapter::instance().addLikeship(ship);	// 同时同步旧Cache
	ILikeItCacheAdapter::instance().addLikeship(ship);
	
	ostringstream tmp;
	tmp << ship->gid->type << "_" << ship->gid->id;
	MCE_INFO("[ILikeItManagerI::addLike] add " << ship->uid << " to " << tmp.str());

	// 加入sql序列
	_invokeAdd.invoke(ship);

	// 加入远程统计log
	ActiveTrack::instance().action(ship->uid, "ilikeit", "add_like", tmp.str());
}

LikeInfoPtr ILikeItManagerI::addLikeAndGet(const LikeshipPtr& ship, const Ice::Current& crt) {

	LikeInfoPtr info; 
	// 获取该对象已有数据
	try{
		info = ILikeItCacheAdapter::instance().getLikeDetail(ship->gid, ship->uid, 8, true);
	}catch(std::exception& ex){
		MCE_INFO(ex.what());
	}catch(...){
		MCE_INFO("Unknown exception ");
	}
//	LikeInfoPtr info = ILikeItCacheOldAdapter::instance().getLikeDetail(ship->gid, ship->uid, 8, true);	// 从旧服务中获取
	
	// 同时向前端返回一个拼成的‘假‘数据
	// 处理已有这个uid的情况
	int pos = -1;
	for ( size_t i = 0; i < info->detailList.size(); ++i ) {
		if ( info->detailList[i]->uid == ship->uid ) {
			pos = i;
			break;
		}
	}
	// 当已有该用户时,不对结果顺序进行移动 -- 090813 by zhanghan
	if ( pos >= 0 ) {
		return info;
	}
	// 添加
	addLike(ship);

	/* if ( pos == 0 ) {
		return info;
	} else if ( pos > 0 ) {
		UserDetailPtr tmp = info->detailList[pos];
		for ( size_t i = pos; i >=1; --i ) {
			info->detailList[i] = info->detailList[i-1];
			info->detailList[0] = tmp;
		}
		return info;
	} */

	// 处理没有该用户的情况
	UserCachePtr userCache;
	try {
		vector<int> uids;
		uids.push_back(ship->uid);
		userCache = UserCacheAdapter::instance().GetUserCache(uids)[ship->uid];
		//userCache = UserCacheAdapter::instance().getUserCache(ship->uid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItManagerI::addLikeAndGet] getUserCacheList Ice::Excetpion : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItManagerI::addLikeAndGet] getUserCacheList std::excetpion : " << e.what());
	} catch (...) {
		MCE_WARN("[ILikeItManagerI::addLikeAndGet] getUserCacheList Excetpion");
	}
	if(userCache!=NULL){
		UserDetailPtr tmp = new UserDetail;
		tmp->uid = ship->uid;
		tmp->name = userCache->name();	
		string imgOri = userCache->tinyurl();
		tmp->img = HeadUrlUtil::instance().getHeadFullUrl(imgOri);

		if ( info->selfCount <= 0 )  {
			info->selfCount = 1;
			info->totalCount ++;
		}
		// 都改为放在最后  090825 by zhanghan
		info->detailList.push_back(tmp); 
	}
	/* if ( info->detailList.size() <= 0 ) {
		info->detailList.push_back(tmp);
	} else {
		UserDetailPtr last = info->detailList[info->detailList.size()-1];
		info->detailList.push_back(last);
		for ( size_t i = info->detailList.size()-1; i >=1; --i) {
			info->detailList[i] = info->detailList[i-1];
		}
		info->detailList[0] = tmp;
	} */
/*
	if(crt.ctx.size()!=0){
		Ice::Context ctx = crt.ctx;
		if(ctx.find("ownerId")!=ctx.end()&&ctx.find("typeId")!=ctx.end()&&ctx.find("name")!=ctx.end()&&ctx.find("gid")!=ctx.end()){
			MCE_INFO("send webpager for : "<<ctx["ownerId"]);
			try{
				int ownerId=boost::lexical_cast<int>(ctx["ownerId"]);
				int typeId=boost::lexical_cast<int>(ctx["typeId"]);
				if(ownerId!=ship->uid)
					ILikeItUtil::sendWebPager(ownerId,ship->uid,info->totalCount,typeId, ctx["gid"], ctx["name"]);	
			}catch(...){
				MCE_WARN("can't sendWebPager ");
			}
		}
	}
*/
	return info;
}

LikeInfo2Ptr ILikeItManagerI::addLikeAndGet2(const LikeshipPtr& ship, const Ice::Current& crt) {

	MCE_INFO("addLikeAndGet2 ");
	// 获取该对象已有数据
	LikeInfo2Ptr info = ILikeItCacheAdapter::instance().getLikeDetail2(ship->gid, ship->uid, 8, true);
	//	LikeInfoPtr info = ILikeItCacheOldAdapter::instance().getLikeDetail(ship->gid, ship->uid, 8, true);	// 从旧服务中获取
	
	// 同时向前端返回一个拼成的‘假‘数据
	// 处理已有这个uid的情况
	int pos = -1;
	for ( size_t i = 0; i < info->detailList.size(); ++i ) {
		if ( info->detailList[i]->uid == ship->uid ) {
			pos = i;
			break;
		}
	}
	// 当已有该用户时,不对结果顺序进行移动 -- 090813 by zhanghan
	if ( pos >= 0 ) {
		return info;
	}

	// 添加
	try{
		addLike(ship);
	}catch(std::exception&  ex){
		MCE_WARN(ex.what());
	}catch(...){
		MCE_WARN("unknow exception ");
	}

	/* if ( pos == 0 ) {
		return info;
	} else if ( pos > 0 ) {
		UserDetailPtr tmp = info->detailList[pos];
		for ( size_t i = pos; i >=1; --i ) {
			info->detailList[i] = info->detailList[i-1];
			info->detailList[0] = tmp;
		}
		return info;
	} */

	// 处理没有该用户的情况
	UserCachePtr userCache;
	try {
		vector<int> uids;
		uids.push_back(ship->uid);
		userCache = UserCacheAdapter::instance().GetUserCache(uids)[ship->uid];
		//userCache = UserCacheAdapter::instance().getUserCache(ship->uid);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItManagerI::addLikeAndGet] getUserCacheList Ice::Excetpion : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItManagerI::addLikeAndGet] getUserCacheList std::excetpion : " << e.what());
	} catch (...) {
		MCE_WARN("[ILikeItManagerI::addLikeAndGet] getUserCacheList Excetpion");
	}
	if(userCache!=NULL){
		UserDetail2Ptr tmp = new UserDetail2;
		tmp->uid = ship->uid;
		tmp->name = userCache->name();	
		string imgOri = userCache->tinyurl();
		tmp->img = HeadUrlUtil::instance().getHeadFullUrl(imgOri);
		tmp->isKeepUse = userCache->IsKEEPUSE();
		if ( info->selfCount <= 0 )  {
			info->selfCount = 1;
			info->totalCount ++;
		}
		// 都改为放在最后  090825 by zhanghan
		info->detailList.push_back(tmp); 
	}
	/* if ( info->detailList.size() <= 0 ) {
		info->detailList.push_back(tmp);
	} else {
		UserDetailPtr last = info->detailList[info->detailList.size()-1];
		info->detailList.push_back(last);
		for ( size_t i = info->detailList.size()-1; i >=1; --i) {
			info->detailList[i] = info->detailList[i-1];
		}
		info->detailList[0] = tmp;
	} */
/*
	MCE_INFO("send size "<<crt.ctx.size());
	if(crt.ctx.size()!=0){
		Ice::Context ctx = crt.ctx;
		if(ctx.find("ownerId")!=ctx.end()&&ctx.find("typeId")!=ctx.end()&&ctx.find("name")!=ctx.end()&&ctx.find("gid")!=ctx.end()){
			MCE_INFO("send webpage for : "<<ctx["ownerId"]);
			try{
				int ownerId=boost::lexical_cast<int>(ctx["ownerId"]);
				int typeId=boost::lexical_cast<int>(ctx["typeId"]);
				if(ownerId!=ship->uid)
					ILikeItUtil::sendWebPager(ownerId, ship->uid, info->totalCount, typeId, ctx["gid"], ctx["name"]);
			}catch(...){
				MCE_WARN("can't sendWebPager ");
			}
		}
	}
*/

	return info;
}
void ILikeItManagerI::addLikeBatch(const LikeshipSeq& shipSeq, const Ice::Current& crt) {
	
	if ( shipSeq.size() <= 0 ) {
		MCE_WARN("[ILikeItManagerI::addLikeBatch] 0 ship(s), don't add");
		return;
	}

	// 按表分组
	map<int, LikeshipSeq> mapGroup;	
	for (LikeshipSeq::const_iterator it = shipSeq.begin(); it != shipSeq.end(); ++it) {
		int mod = GlobalIdTrans::GlobalId2Long((*it)->gid) % TABLE_NUM;
		if ( mapGroup.find(mod) != mapGroup.end() ) {
			mapGroup[mod].push_back((*it));
		}
		else {
			LikeshipSeq seq;
			seq.push_back((*it));
			mapGroup.insert( make_pair<int, LikeshipSeq>(mod, seq) );
		}
	}

	// 执行sql
	// vector<GlobalIdPtr> gids;
	for (map<int, LikeshipSeq>::iterator itMap = mapGroup.begin(); itMap != mapGroup.end(); ++itMap) {
		Statement sql;
		ostringstream tmp;
		tmp << "INSERT IGNORE INTO " << TABLE_LIKESHIP << (*itMap).first << " (" << FIELD_GlobalId << "," << FIELD_UID << ") values";
		for (LikeshipSeq::const_iterator it = (*itMap).second.begin();
				it != (*itMap).second.end(); ++it) {
			if ( it != (*itMap).second.begin() ) {
				tmp << ",";
			}

			tmp << "(" << GlobalIdTrans::GlobalId2Long((*it)->gid) << "," << (*it)->uid << ")"; 
			// gids.push_back((*it)->gid);
		}
		sql << tmp.str();
		MCE_DEBUG("[AddLikeTask::handle] sql : " << tmp.str());
		try {
			QueryRunner(DB_DESC_ILIKEIT, CDbWServer).execute(sql);
		} catch (Ice::Exception& e) {
			MCE_WARN("[AddLikeTask::handle] QueryRunner::execute Ice::Exception " << e.what());
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("[AddLikeTask::handle] QueryRunner::execute mysqlpp::exception -> " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[AddLikeTask::handle] QueryRunner::execute exception " << e.what());
		} catch (...) {
			MCE_WARN("[AddLikeTask::handle] QueryRunner::execute exception ");
		}
	}

	// TaskManager::instance().execute(new AddLikeTask(shipSeq));
}

void ILikeItManagerI::removeLike(const LikeshipPtr& ship, const Ice::Current& crt) {

	// 同步ILikeItCache	
	//ILikeItCacheOldAdapter::instance().removeLikeship(ship);	// 兼容同步旧数据
	ILikeItCacheAdapter::instance().removeLikeship(ship);

	ostringstream tmp;
	tmp << ship->gid->type << "_" << ship->gid->id;
	MCE_DEBUG("[ILikeItManagerI::removeLike] remove " << ship->uid << " from " << tmp.str());

	// 加入sql序列
	_invokeRemove.invoke(ship);

	// 加入远程统计log
	ActiveTrack::instance().action(ship->uid, "ilikeit", "remove_like", tmp.str());

}

LikeInfoPtr ILikeItManagerI::removeLikeAndGet(const LikeshipPtr& ship, const Ice::Current& crt) {

	// 获取该对象已有数据
	LikeInfoPtr info = ILikeItCacheAdapter::instance().getLikeDetail(ship->gid, ship->uid, 8, true);
//	LikeInfoPtr info = ILikeItCacheOldAdapter::instance().getLikeDetail(ship->gid, ship->uid, 8, true); // 从旧服务中获取
	
	
	// 向前端返回一个拼成的‘假‘数据
	// 数量加减
	if ( info->selfCount <= 0 ) {
		MCE_DEBUG("[ILikeItManagerI::removeLikeAndGet] Without Self, remove none of " << info->totalCount );
		return info;
	}
	
	// 删除
	removeLike(ship);

	info->selfCount = 0;
	info->totalCount--;
	if ( info->detailList.size()<= 0 ) {
		return info;
	}

	int pos = -1;
	for ( size_t i = 0; i < info->detailList.size(); ++i ) {
		if ( info->detailList[i]->uid == ship->uid ) {
			pos = i;
			break;
		}
	}
	if ( (pos >= 0) && (pos < info->detailList.size()) ) {
		// 当已有该用户时,不对结果顺序进行移动 -- 090813 by zhanghan
		info->detailList[pos]->uid = -1;
		/* for ( size_t i = pos; i < info->detailList.size()-1; ++i ) {
			info->detailList[i] = info->detailList[i+1];
		}
		info->detailList.resize(info->detailList.size()-1);  */
		return info;
	} else {
		return info;
	}
}

LikeInfo2Ptr ILikeItManagerI::removeLikeAndGet2(const LikeshipPtr& ship, const Ice::Current& crt) {

	// 获取该对象已有数据
	LikeInfo2Ptr info = ILikeItCacheAdapter::instance().getLikeDetail2(ship->gid, ship->uid, 8, true);
//	LikeInfoPtr info = ILikeItCacheOldAdapter::instance().getLikeDetail(ship->gid, ship->uid, 8, true); // 从旧服务中获取
	
	
	// 向前端返回一个拼成的‘假‘数据
	// 数量加减
	if ( info->selfCount <= 0 ) {
		MCE_DEBUG("[ILikeItManagerI::removeLikeAndGet] Without Self, remove none of " << info->totalCount );
		return info;
	}
	
	// 删除
	removeLike(ship);

	info->selfCount = 0;
	info->totalCount--;
	if ( info->detailList.size()<= 0 ) {
		return info;
	}

	int pos = -1;
	for ( size_t i = 0; i < info->detailList.size(); ++i ) {
		if ( info->detailList[i]->uid == ship->uid ) {
			pos = i;
			break;
		}
	}
	if ( (pos >= 0) && (pos < info->detailList.size()) ) {
		// 当已有该用户时,不对结果顺序进行移动 -- 090813 by zhanghan
		info->detailList[pos]->uid = -1;
		/* for ( size_t i = pos; i < info->detailList.size()-1; ++i ) {
			info->detailList[i] = info->detailList[i+1];
		}
		info->detailList.resize(info->detailList.size()-1);  */
		return info;
	} else {
		return info;
	}
}




void ILikeItManagerI::removeLikeBatch(const LikeshipSeq& shipSeq, const Ice::Current& crt) {
	
	if ( shipSeq.size() <= 0 ) {
		MCE_WARN("[ILikeItManagerI::removeLikeBatch] 0 ship(s), don't add");
		return;
	}

	// 按表分组
	map<int, LikeshipSeq> mapGroup;	
	for (LikeshipSeq::const_iterator it = shipSeq.begin(); it != shipSeq.end(); ++it) {
		int mod = GlobalIdTrans::GlobalId2Long((*it)->gid) % TABLE_NUM;
		if ( mapGroup.find(mod) != mapGroup.end() ) {
			mapGroup[mod].push_back((*it));
		}
		else {
			LikeshipSeq seq;
			seq.push_back((*it));
			mapGroup.insert( make_pair<int, LikeshipSeq>(mod, seq) );
		}
	}	

	// 执行sql
	vector<GlobalIdPtr> gids;
	for (map<int, LikeshipSeq>::iterator itMap = mapGroup.begin(); itMap != mapGroup.end(); ++itMap) {
		Statement sql;
		ostringstream tmp;
		tmp << "DELETE FROM " << TABLE_LIKESHIP << (*itMap).first << " WHERE (" << FIELD_GlobalId << "," << FIELD_UID << ") IN (";
		for (LikeshipSeq::const_iterator it = (*itMap).second.begin(); it != (*itMap).second.end(); ++it) {
			if ( it != (*itMap).second.begin() ) {
				tmp << ",";
			}
			tmp << "(" << GlobalIdTrans::GlobalId2Long((*it)->gid) << "," << (*it)->uid << ")"; 
			gids.push_back((*it)->gid);
		}
		tmp << ")";
		sql << tmp.str();
		try {
			QueryRunner(DB_DESC_ILIKEIT, CDbWServer).execute(sql);
		} catch (Ice::Exception& e) {
			MCE_WARN("[RemoveLikeTask::handle] QueryRunner::execute Ice::Exception " << e.what());
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("[RemoveLikeTask::handle] QueryRunner::execute mysqlpp::exception -> " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[RemoveLikeTask::handle] QueryRunner::execute exception " << e.what());
		} catch (...) {
			MCE_WARN("[RemoveLikeTask::handle] QueryRunner::execute exception ");
		}
	}


	// TaskManager::instance().execute(new RemoveLikeTask(shipSeq));
}

// =============== Thread ==============================
// ------------- InvokeAdd -----------------------------
void InvokeAdd::invoke(const LikeshipPtr& ship) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	MCE_DEBUG("[InvokeAdd::invoke] add a likeship : type " << ship->gid->type 
			<< ", id " << ship->gid->id << ", uid " << ship->uid);
	_cache.push_back(ship);
	if (_cache.size() > 5) {
		_mutex.notify();
	}
}

void InvokeAdd::invoke(const LikeshipSeq& shipSeq) {
	if (shipSeq.empty()) {
		return;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.insert(_cache.end(), shipSeq.begin(), shipSeq.end());
	if (_cache.size() > 5) {
		_mutex.notify();
	}
}

void InvokeAdd::run() {
	while (true) {
		try {
			LikeshipSeq batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.size() < 5) {
					_mutex.wait();
				}
				batch.swap(_cache);
			}
			sleep(1);
			MCE_INFO("[InvokeAdd::run] add a likebatch task for " << batch.size() << " ship(s)");
			ILikeItManagerI::instance().addLikeBatch(batch);
		} catch (...) {
			MCE_WARN("[InvokeAdd::run]InvokeAdd main loop exception");
		}
	}
}

// ------------- InvokeRemove -----------------------------
void InvokeRemove::invoke(const LikeshipPtr& ship) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.push_back(ship);
	if (_cache.size() > 10) {
		_mutex.notify();
	}
}

void InvokeRemove::invoke(const LikeshipSeq& shipSeq) {
	if (shipSeq.empty()) {
		return;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_cache.insert(_cache.end(), shipSeq.begin(), shipSeq.end());
	if (_cache.size() > 10 ) {
		_mutex.notify();
	}
}

void InvokeRemove::run() {
	while (true) {
		try {
			LikeshipSeq batch;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_cache.size() < 10) {
					_mutex.wait();
				}
				batch.swap(_cache);
			}
			sleep(10);
			ILikeItManagerI::instance().removeLikeBatch(batch);
			MCE_INFO("[InvokeRemove::run] remove a likebatch task for " << batch.size() << " ship(s)");
		} catch (...) {
			MCE_WARN("[InvokeRemove::run]InvokeRemove main loop exception");
		}
	}
}
