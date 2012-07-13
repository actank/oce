#include "DispatchI.h"
#include "OnlineCenterAdapter.h"
#include "BuddyByIdReplicaAdapter.h"

using namespace talk::online;
using namespace talk::online::adapter;
using namespace xce::buddy::adapter;
using namespace MyUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&DispatchManagerI::instance(), service.createIdentity("M", ""));
	PipePool::instance().initialize(new PipeFactoryI);
}

void DispatchManagerI::presence(const JidPtr& jid, OnlineStat stat, const string& ver,
								const string& ext, const Ice::Current&) {
	MCE_DEBUG("DispatchManagerI::online  --> userId:"<<jid->userId<<"   index:"
			<<jid->index);

	AvatarPtr avatar = new Avatar;
	avatar->jid = jid;
	avatar->stat = stat;
	avatar->ver = ver;
	avatar->ext = ext;
	TaskManager::instance().execute(new NotifyTask(avatar));
}

void DispatchManagerI::offline(const JidPtr& jid, const Ice::Current&) {
	MCE_DEBUG("DispatchManagerI::offline -->  userid:"<<jid->userId<<"   index:"
			<<jid->index);
	AvatarPtr avatar = new Avatar;
	avatar->jid = jid;
	avatar->stat = OFFLINE;

	TaskManager::instance().execute(new NotifyTask(avatar));
}

void DispatchManagerI::offlineBatch(const JidSeq& jids, const Ice::Current&) {
	for (size_t i = 0; i < jids.size(); ++i) {
		offline(jids.at(i));
	}
}

void NotifyTask::handle() {
	MCE_DEBUG("NotifyTask::handle() --> run");
	int cluster = OnlineCenterAdapter::instance().getCluster();
	if (cluster <= 0) {
		cluster = 1;
	}
	MyUtil::IntSeq ids;
	try {
		ids = BuddyByIdCacheAdapter::instance().getFriendListAsc(_avatar->jid->userId, 5000);
	} catch(Ice::Exception& e){
		MCE_WARN("NotifyTask::handle()  -> getFriendSeq uid ->"<<_avatar->jid->userId << " err:"<< e);
		return;
	}catch(...) {
		MCE_WARN("NotifyTask::handle()  -> getFriendSeq uid ->"<<_avatar->jid->userId);
		return;
	}
	MCE_DEBUG("NotifyTask::handle --> friends " << ids.size());
	map<int, MyUtil::IntSeq> idmap;
	for(int i=0; i<cluster; i++){
		idmap[i] = MyUtil::IntSeq();
	}
	for (size_t i = 0; i < ids.size(); ++i) {
		idmap[ids.at(i) % cluster].push_back(ids.at(i));
	}

	for (map<int , MyUtil::IntSeq>::iterator it = idmap.begin(); it
			!= idmap.end(); ++it) {
		BuddyOnlineNotifyPtr notify = new BuddyOnlineNotify;
		notify->host = _avatar;
		notify->buddyList = it->second;
		PipePool::instance().push("OnlineBuddy", it->first, notify);

	}
}

void NotifyPipe::handle(const ObjectSeq& seq){
	MCE_DEBUG("NotifyPipe::handle");
	if (seq.empty()) {
			return;
		} else {
			MCE_DEBUG("NotifyPipe::handle --> objectseq size = " <<seq.size());
		}

		BuddyOnlineNotifySeq oseq;
		for (size_t i = 0; i < seq.size(); ++i) {
			oseq.push_back(BuddyOnlineNotifyPtr::dynamicCast(seq.at(i)));
		}
		try {
			OnlineCenterAdapter::instance().notify(getIndex(),oseq);
		} catch(Ice::Exception& e) {
			MCE_WARN("NotifyPipe::handle --> deliver err:"<<e);
		}
}
