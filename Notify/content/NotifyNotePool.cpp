#include "NotifyNotePool.h"

#include <map>
#include <vector>
#include <string>
//extern "C"{
#include "quicklz.h"
//}
#include "QueryRunner.h"

using namespace std;
using namespace xce::notify;
using namespace MyUtil;
using namespace com::xiaonei::xce;

const static int TIMEOUT = 100;

////////////////////// NotifyCache ///////////////////////////////////////

NotifyContentPtr NotifyCache::getNotifyContent(Ice::Long replyId){
	//在cache中取一个NotifyContent
	NotifyContentPtr reply;

	int replyIndex = _getNotifyContentIndex(replyId);

	IceUtil::Mutex::Lock lock(_mutexPool[_getMutexIndex(replyId)]);

	if(	_cache[replyIndex]
	    && _cache[replyIndex]->id == replyId)
		return _cache[replyIndex];
	else return NULL;
}

void NotifyCache::putNotifyContent(const NotifyContentPtr& reply){
	if( reply ){
		int index = _getNotifyContentIndex(reply->id);
		string compressed = NotifyCache::instance().compress(reply->props);
		if(compressed.empty()){
			MCE_DEBUG("NotifyCache::putNotifyContent --> no compress");
			reply->type &= 0x7FFFFFFF;
		} else {
			MCE_DEBUG("NotifyCache::putNotifyContent -->  compress");
			reply->props.swap(compressed);
			MCE_DEBUG("NotifyCache::putNotifyContent --> type="<<reply->type);
			reply->type |= 0x80000000;
			MCE_DEBUG("NotifyCache::putNotifyContent --> type="<<reply->type);
		}

		IceUtil::Mutex::Lock lock(_mutexPool[_getMutexIndex(reply->id)]);
		_loadTime[index] = time(NULL);
		_cache[index] = reply;
	}
}

void NotifyCache::addNotifyContent(const NotifyContentPtr& reply){
	if( reply )
	{
		int index = _getNotifyContentIndex(reply->id);
		IceUtil::Mutex::Lock lock(_mutexPool[_getMutexIndex(reply->id)]);
		_loadTime[index] = time(NULL);
		_cache[index] = reply;
	}else
	{
		MCE_INFO("NotifyCache::addNotifyContent NULL!!");
	}
}

bool NotifyCache::removeNotifyContent(Ice::Long replyId){
	int replyIndex = _getNotifyContentIndex(replyId);
	IceUtil::Mutex::Lock lock(_mutexPool[_getMutexIndex(replyId)]);
	if(	_cache[replyIndex]  && _cache[replyIndex]->id == replyId){
	  _cache[replyIndex] = NULL;
	  MCE_INFO(" NotifyCache::deleted from the cache");
	  return true;
	}else{
		MCE_INFO(" NotifyCache::not in the cache!");
		return false;
	}
}

void NotifyCache::cleanCacheByTime(){
	for( int k=0; k<REPLY_CACHE_SIZE; ++k ){
    // TODO : 此处可以简单的优化, 减少锁的次数
		IceUtil::Mutex::Lock lock(_mutexPool[_getMutexIndex(k*CLUSTER)]);
		if( _cache[k] && difftime(time(NULL),_loadTime[k]) > 3600*24*3 ){
			_cache[k] = NULL;
			_loadTime[k] = 0;
		}
	}
}

////////////////////////// NotifyFactory /////////////////////////////////////////////

ObjectResultPtr NotifyFactory::createNotifyContent( const MyUtil::LongSeq& replyIds ){
	map<int, MyUtil::LongSeq> group;
	for (size_t h = 0; h < replyIds.size(); ++h) {
		group[replyIds.at(h) % _getTableCount()].push_back(replyIds.at(h));
	}
	map<int, MyUtil::LongSeq>::iterator iter;
	ObjectResultPtr result = new ObjectResult;
	ResultHandlerI handler(result);
	for (iter = group.begin(); iter != group.end(); ++iter) {
		Statement sql;
		sql<<" SELECT * FROM "<<_getTableName(iter->first)
		   <<" WHERE id in ( ";
		for (size_t h = 0; h < (iter->second).size(); ++h) {
			if(h!=0) sql << "," ;
			sql << (iter->second).at(h);
		}
		sql << ") order by time";
		QueryRunner(_getInstance(), CDbRServer, _getExpr(iter->first)).query(sql, handler);
	}
	return result;
}

NotifyContentDict NotifyFactory::createNotifyContentParal( const MyUtil::LongSeq& replyIds ){
	map<int, MyUtil::LongSeq> group;
	for (size_t h = 0; h < replyIds.size(); ++h) {
		group[replyIds.at(h) % _getTableCount()].push_back(replyIds.at(h));
	}
	map<int, MyUtil::LongSeq>::iterator iter;
	ParaLoaderPtr loader = new ParaLoader();

	for (iter = group.begin(); iter != group.end(); ++iter) {
		ostringstream oss;
		oss << " SELECT * FROM "<<_getTableName(iter->first)
				   <<" WHERE id in ( ";
		for (size_t h = 0; h < (iter->second).size(); ++h) {
			if(h!=0){
				oss << "," ;
			}
			oss << (iter->second).at(h);
		}
		oss << ") order by time";
		loader->addQuery(_getInstance(),oss.str(), _getExpr(iter->first),iter->second);
	}
	int not_exist = -1;
	NotifyContentDict res = loader->query(not_exist);
	return res;
}
void NotifyFactory::deleteNotifyContent( const MyUtil::LongSeq& replyIds )
{
	map<int, MyUtil::LongSeq> group;
	for (size_t h = 0; h < replyIds.size(); ++h) {
		group[replyIds.at(h) % _getTableCount()].push_back(replyIds.at(h));
	}
	map<int, MyUtil::LongSeq>::iterator iter;
	iter = group.begin();
	try{
		while( iter != group.end() )
		{
			Statement sql;
			sql << " DELETE FROM "<<_getTableName(iter->first)
					   <<" WHERE id in ( ";
			for (size_t h = 0; h < (iter->second).size(); ++h){
				if( h )sql << "," ;
				sql << (iter->second).at(h);
			} sql << ") ";
			//警告 下句执行SQL语句 不知道对不对 ？？？？
			QueryRunner(_getInstance(), CDbRServer, _getExpr(iter->first)).store(sql);
			iter++;
		}
	}catch (std::exception& e) {
		MCE_WARN("ParaLoader::SqlExecuteTask --> err, " << e.what());
	}
}
string NotifyCache::compress(const string& data) const{
	int dataLen = data.length();
	char* compressed = new char[dataLen+400];
	char* scratch = new char[QLZ_SCRATCH_COMPRESS];
	memset(scratch, 0, QLZ_SCRATCH_COMPRESS);
	int compressedSize = qlz_compress((void*)data.data(), compressed, dataLen, scratch);
	string ret;
	if(compressedSize >= dataLen){
		//ret = "";
		MCE_DEBUG("NotifyFactory::ResultHandlerI --> not need compress   "<<data);
	} else {
    //ret = compressed;
		ret.insert(0,compressed,compressedSize); // 防止0字符导致buffer被切断
		MCE_DEBUG("NotifyFactory::ResultHandlerI --> can  compress   "<<data);
	}
	delete compressed;
	delete scratch;
	return ret;
}


bool NotifyFactory::ResultHandlerI::handle(mysqlpp::Row& row) const{
	NotifyContentPtr reply = new NotifyContent;
	reply->id = (Ice::Long) row["id"];
	reply->source = (Ice::Long)row["source"];
	reply->owner = (int)row["owner"];
	reply->from = (int)row["from_id"];
	reply->type = (int)row["type"];
	reply->time = (time_t) mysqlpp::DateTime(row["time"]);

	string props = row["props"].data();
	string compressed = NotifyCache::instance().compress(props);
	if(compressed.empty()){
		MCE_DEBUG("NotifyFactory::ResultHandlerI::handle --> no compress");
		reply->props.swap(props);
		reply->type &= 0x7FFFFFFF;
	} else {
		MCE_DEBUG("NotifyFactory::ResultHandlerI::handle -->  compress");
		reply->props.swap(compressed);
		MCE_DEBUG("NotifyFactory::ResultHandlerI::handle --> type="<<reply->type);
		reply->type |= 0x80000000;
		MCE_DEBUG("NotifyFactory::ResultHandlerI::handle --> type="<<reply->type);
	}
	_result->data[reply->id] = reply;
	return true;
}

void ParaLoader::addQuery(const string& dbInstance, const string& sql,
				const string& pattern, const vector<Ice::Long>& fids) {
  SqlExecuteTask* t = new SqlExecuteTask(this);
  t->init(dbInstance, sql, pattern, fids);
  _taskSeq.push_back(t);
  ++_count;
}

map<Ice::Long,NotifyContentPtr> ParaLoader::query(int & not_exist) {
  int total = _count;
  for (size_t i = 0; i < _taskSeq.size(); ++i) {
    //    MCE_INFO("ParaLoader::query --> execute task i = " << i);
    MyUtil::TaskManager::instance().execute(_taskSeq.at(i));
  }
  _taskSeq.clear();

  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
  if (!timedWait(IceUtil::Time::milliSeconds(TIMEOUT))) {
    MCE_WARN("ParaLoader::query -> total:" << total << " miss:" << _count << "  fail!");
  } else {
//    MCE_INFO("ParaLoader::query -> total:" << total << " miss:" << _count << "  success");
  }
  not_exist = not_exist_;
  return ndict_;
}

void ParaLoader::SqlExecuteTask::handle() {

  mysqlpp::StoreQueryResult res;
  try {
    TimeStat ts;
    res = QueryRunner(_dbInstance, CDbRServer, _pattern).store(_sql);

  } catch (std::exception& e) {
    MCE_WARN("ParaLoader::SqlExecuteTask --> err, " << e.what());
//    MCE_WARN("ParaLoader::SqlExecuteTask::handle --> query return null");
    _loader->finish(0);//读数据库失败，认为feed都存在
    return;
  }
  if (!res) {
    MCE_WARN("ParaLoader::SqlExecuteTask::handle --> query return null");
    _loader->finish(0);//读数据库失败，认为feed都存在
    return;
  }
  //MCE_INFO("ParaLoader::SqlExecuteTask::handle -->1 cost:" << ts.getTime() << " rows:" << res.num_rows() << " expr:" << _pattern);
  NotifyContentDict ndict;
  for (size_t i = 0; i < res.num_rows(); ++i){
    mysqlpp::Row row = res.at(i);

    NotifyContentPtr reply = new NotifyContent;
		reply->id = (Ice::Long) row["id"];
		reply->source = (Ice::Long) row["source"];
		reply->owner = (int) row["owner"];
		reply->from = (int) row["from_id"];
		reply->type = (int) row["type"];
		reply->time = (time_t) mysqlpp::DateTime(row["time"]);

		string props = row["props"].data();
		string compressed = NotifyCache::instance().compress(props);
		if (compressed.empty()) {
			MCE_DEBUG("NotifyFactory::ResultHandlerI::handle --> no compress");
			reply->props.swap(props);
			reply->type &= 0x7FFFFFFF;
		} else {
			MCE_DEBUG("NotifyFactory::ResultHandlerI::handle -->  compress");
			reply->props.swap(compressed);
			MCE_DEBUG("NotifyFactory::ResultHandlerI::handle --> type="<<reply->type);
			reply->type |= 0x80000000;
			MCE_DEBUG("NotifyFactory::ResultHandlerI::handle --> type="<<reply->type);
		}
//		_result->data[reply->id] = reply;
		ndict[reply->id] = reply;

		fids_.erase(reply->id);
  }
  for(set<Ice::Long>::iterator it = fids_.begin(); it != fids_.end();it++)
	  ndict[*it] = NULL;

  _loader->finish(ndict, fids_.size());
}
