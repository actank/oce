#include "NotifyContent.h"

#include <map>
#include <vector>
#include <string>

using namespace std;
using namespace xce::notify;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&NotifyContentManagerI::instance(),
			service.createIdentity("M", ""));
	MyUtil::TaskManager::instance().scheduleRepeated(new CleanNotifyCacheTimer);
}


//先在cache中找，如果找不到从数据库中加载，然后放到cache里面
NotifyContentDict NotifyContentManagerI::getNotifyDict(const MyUtil::LongSeq& replyIds, const Ice::Current& crt){
	string loadtype= "default";
	map<string,string>::const_iterator it = crt.ctx.find("loadtype");
	if(it != crt.ctx.end()){
		loadtype = it->second;
	}
	NotifyContentDict result;
	MyUtil::LongSeq missIds;
	//先在cache中找
	for (size_t h = 0; h < replyIds.size(); ++h) {
		Ice::Long replyId;
		replyId = replyIds.at(h);
		NotifyContentPtr reply;
		reply = NotifyCache::instance().getNotifyContent(replyId);
		if (reply) {//在内存中有记录
			result[replyId] = reply;
			continue;
		}
		missIds.push_back(replyId);
	}
	if (missIds.empty()) {
		MCE_INFO("NotifyContentManagerI::getNotifyDict. all from memeory. result size = "<<result.size()
				<< " request id:" << replyIds.size() << " miss:" << missIds.size()
		);
		return result;
	}

	//从数据库加载
	// TODO : 这里不作加载，加载在 NotifyCache 中进行
	ObjectResultPtr replyContents;
	TimeStat ts;
	string name;
	if(false){
		try{
			replyContents = NotifyFactory::instance().createNotifyContent(missIds);
		} catch (mysqlpp::Exception e){
			MCE_WARN("NotifyContentManagerI::getNotifyDict --> db error:"<<e.what());
			return result;
		}
		for(size_t k=0; k<missIds.size(); k++){
			Ice::Long replyId;
			ObjectMap::iterator iter;
			replyId = missIds.at(k);
			iter = replyContents->data.find(replyId);
			if( iter != replyContents->data.end() ){//从db中加载成功
				NotifyContentPtr replyContent = NotifyContentPtr::dynamicCast(iter->second);
				result[replyId] = replyContent;
				NotifyCache::instance().addNotifyContent(replyContent);
			}
		}
		name = " .single. ";
	}else{
		NotifyContentDict ndict = NotifyFactory::instance().createNotifyContentParal(missIds);
		for(NotifyContentDict::iterator it = ndict.begin(); it != ndict.end();++it){
			result[it->first] = it->second;
			if( it->second )
			{//查出来的有可能是空的
				NotifyCache::instance().addNotifyContent(it->second);
			}
		}
		name = " .paral. ";
	}
	MCE_INFO("NotifyContentManagerI::getNotifyDict. with db load. result size = "<<result.size() << name
			<< " request id:" << replyIds.size() << " miss:" << missIds.size()
//			<< " miss ids:" << PrintHelper::Print(missIds)
			<< " dbcost:" << ts.getTime());

	return result;
}

void NotifyContentManagerI::removeNotifies(const MyUtil::LongSeq &NotifyIds,const Ice::Current&)
{
	MCE_INFO("removeNotifies. NotifyIds.size() =" << NotifyIds.size());
	for(size_t i = 0; i<NotifyIds.size() ;i++){
		NotifyCache::instance().removeNotifyContent(NotifyIds.at(i));
	}
	NotifyFactory::instance().deleteNotifyContent(NotifyIds);
}

void NotifyContentManagerI::addNotify(const NotifyContentPtr& reply, const Ice::Current& ){
	MCE_INFO("addNotify. nid=" << reply->id);
	NotifyCache::instance().putNotifyContent(reply);
}

void CleanNotifyCacheTimer::handle() {
	MCE_INFO("CleanNotifyCacheTimer cleanCacheByTime");
	NotifyCache::instance().cleanCacheByTime();
}
