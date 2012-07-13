#include "OnlineStatSyncAdapter.h"
#include "PresenceAdapter.h"
//#include "OnlineBuddyAdapter.h"

using namespace talk::online::adapter;
using namespace mop::hi::svc::adapter;

DispatchManagerPrx OnlineStatSyncAdapter::getManagerOneway(int id) {
	return locate<DispatchManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}
//void                          presence1(const JidPtr& jid, OnlineStat stat, const string& ver, const string& ext);
void OnlineStatSyncAdapter::presence(const JidPtr& jid, com::xiaonei::talk::common::OnlineStat stat, const string& ver, const string& ext){
	if (jid) {
		getManagerOneway(jid->userId)->presence(jid, stat, ver, ext);
	}
}
void OnlineStatSyncAdapter::offline(const JidPtr& jid) {
	if (jid) {
		getManagerOneway(jid->userId)->offline(jid);
	}
}
void OnlineStatSyncAdapter::offlineBatch(const JidSeq& seq) {
	if (!seq.empty()) {
		getManagerOneway(seq.size())->offlineBatch(seq);
	}
}


void OnlineStatSyncAdapter::addBuddy(int host, int guest) {
/*	MyUtil::IntSeq ids;
	ids.push_back(host);
	ids.push_back(guest);
	JidSeqMap jmap = PresenceAdapter::instance().getIMOnlineUserJidSeqMap(ids);
	for (JidSeqMap::iterator it = jmap.begin(); it != jmap.end(); ++it) {
		try {
			if (it->first == host) {
				OnlineBuddySeq seq;
				for (size_t i = 0; i < it->second.size(); ++i) {
					OnlineBuddyPtr ob = new OnlineBuddy();
					ob->isOnline = true;
					ob->host = it->second.at(i);
					ob->buddyList.push_back(guest);
					seq.push_back(ob);
				}
				OnlineBuddyAdapter::instance().notifyBatch(seq);
			} else {
				OnlineBuddySeq seq;
				for (size_t i = 0; i < it->second.size(); ++i) {
					OnlineBuddyPtr ob = new OnlineBuddy();
					ob->isOnline = true;
					ob->host = it->second.at(i);
					ob->buddyList.push_back(host);
					seq.push_back(ob);
				}
				OnlineBuddyAdapter::instance().notifyBatch(seq);
			}
		} catch(...) {

		}
	}
*/
}
void OnlineStatSyncAdapter::removeBuddy(int host, int guest) {
/*
	MyUtil::IntSeq ids;
	ids.push_back(host);
	ids.push_back(guest);
	JidSeqMap jmap = PresenceAdapter::instance().getIMOnlineUserJidSeqMap(ids);
	for (JidSeqMap::iterator it = jmap.begin(); it != jmap.end(); ++it) {
		try {
			if (it->first == host) {
				OnlineBuddySeq seq;
				for (size_t i = 0; i < it->second.size(); ++i) {
					OnlineBuddyPtr ob = new OnlineBuddy();
					ob->isOnline = false;
					ob->host = it->second.at(i);
					ob->buddyList.push_back(guest);
					seq.push_back(ob);
				}
				OnlineBuddyAdapter::instance().notifyBatch(seq);
			} else {
				OnlineBuddySeq seq;
				for (size_t i = 0; i < it->second.size(); ++i) {
					OnlineBuddyPtr ob = new OnlineBuddy();
					ob->isOnline = false;
					ob->host = it->second.at(i);
					ob->buddyList.push_back(host);
					seq.push_back(ob);
				}
				OnlineBuddyAdapter::instance().notifyBatch(seq);
			}
		} catch(...) {

		}
	}
*/
}
