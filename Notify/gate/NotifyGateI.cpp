#include "NotifyGateI.h"

#include <cstring>

#include <boost/lexical_cast.hpp>
#include <openssl/md5.h>
#include <boost/tokenizer.hpp>

#include "APNSNotifyAdapter.h"
#include "NotifyDispatcherAdapter.h"
#include "NotifyStormAdapter.h"
#include "Notify/util/schema_manager.h"
#include "Notify/util/MergeConfig.h"
#include "Notify/util/PrintUtil.h"
#include "WTransitAdapter.h"
#include "FeedNewsF46Adapter.h"
#include "NotifyDispatcherNAdapter.h"
#include "NotifyIndexAdapter.h"
#include "Notify/util/time_stat.h"
#include "FeedViewAdapter.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace xce::notify;
using namespace Ice;
using namespace boost;

using namespace xce::feed;
using talk::adapter::WTransitAdapter;
static const char SOURCE_SEPERATOR = '-';


static const char * kClubDispatchKey = "renrenclub$^%&^%&$%^&";

static void RemoveIndex(int uid, vector<Ice::Long> fail_ids) {
	try {
		NotifyIndexAdapter::instance().removeByIds(uid, fail_ids);

	} catch (Ice::Exception& e) {
		MCE_WARN("RemoveNotifies " << e << " uid=" << uid << " fail_ids size:" << fail_ids.size());
		return;
	}
	MCE_INFO("RemoveIndex. uid:" << uid << " fail_ids size:" << fail_ids.size());
}
static void RemoveIndex(vector<int> uids, vector<Ice::Long> fail_ids) {
	for (unsigned i = 0; i != uids.size(); ++i) {
		RemoveIndex(uids.at(i), fail_ids);
	}
}
class NotifyRequestFactory: public RequestFactory {
public:
	virtual RequestPtr Create(FCGX_Request * r) {
		char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
		if (path) {

			//      string path_s(path);
			//      MCE_DEBUG("NotifyRequestFactory::Create path:" << path_s);

			if (strcmp(path, "/get.notify") == 0 /*|| strcmp(path, "/ebpt/show") == 0*/) {
				return new GetRequest(r);
				//return new GetOldRequest(r);
			}
			if (strcmp(path, "/remove.notify") == 0) {
				return new RemoveRequest(r);
			}
			if (strcmp(path, "/dispatch.notify") == 0) {
				return new DispatchRequest(r);
			}
			if (strcmp(path, "/unreadcount.notify") == 0) {
				return new UnreadCountRequest(r);
			}
			if (strcmp(path, "/quitchat.notify") == 0) {
				return new QuitChatRequest(r);
			}
		}
		return NULL;
	}
};

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&NotifyGateI::instance(), service.createIdentity(
			"M", ""));

	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	string fcgi_socket = props->getPropertyWithDefault("Service."
			+ service.getName() + ".FcgiSocket", "0.0.0.0:9001");
	MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

#if 0
	if (FCGX_Init() != 0) {
		MCE_WARN("FCGX_Init err");
		return;
	}

	int socket = FCGX_OpenSocket(fcgi_socket.c_str(), 100);
	if(socket < 0) {
		MCE_WARN("FCGX_OpenSocket err");
		return;
	}

	for(int i = 0; i< 128; ++i)
	{
		FcgiTask * task = new FcgiTask(new NotifyRequestFactory());
		if(task->init(socket)) {
			TaskManager::instance().execute(task);
		} else {
			delete task;
			MCE_WARN("FCGX_InitRequest err " << i);
		}
	}
#else
	// fcgi_server memory managed as RequestFactory ...
	FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 128);
	fcgi_server->RegisterRequestFactory(new NotifyRequestFactory());
	fcgi_server->Start();
#endif

	MergeConfig::instance().Init();
}

NotifyGateI::NotifyGateI() {
	//ServiceI& service = ServiceI::instance();
	//Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	//_fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
	//MCE_DEBUG("Fcgi listens on : " << _fcgi_socket);
}

bool NotifyGateI::reloadSchema(const Ice::Current&) {
	MCE_INFO("NotifyGateI::reloadSchema()");
	return SchemaManager::instance().reload();
}

void NotifyGateI::QuitChat(::Ice::Int uid, const GroupIndexSeq& groups,
		const ::Ice::Current&) {
	MCE_INFO("NotifyGateI::QuitChat, uid:" << uid << "  groups size:" << groups.size());
	try {
		NotifyDispatcherNAdapter::instance().QuitChat(uid, groups);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyDispatcherNAdapter QuitChat ICE exception : " << e << " uid=" << uid);
	}
	return;
}


void ToWebpager(const xce::notify::NotifyBodyPtr& body,const vector<int> & webpagertargets,vector<long> & fail_ids){
	string txt = SchemaManager::instance().toReplyTxt(body, kPagerViewId, false,
			fail_ids);
	MCE_INFO("ToWebpager.target:" << PrintHelper::Print(webpagertargets) << " txt:" << txt );
	if (!txt.empty()) {
		try {
//			MCE_INFO("tmplog NotifyGateI::instantNotify.call webpager begin" );
			WTransitAdapter::instance().deliverNotifyWebpager(webpagertargets, txt);
		} catch (Ice::Exception& e) {
			MCE_WARN("WTransitAdapter to webpager nid:" << body->id << "  error : " << e);
		}
	} else {
		MCE_INFO("tmplog NotifyGateI::instantNotify.txt empty" );
	}
}

void ToPhone(const xce::notify::NotifyBodyPtr& body,const vector<int> & desktopiphonetargets,vector<long> & fail_ids){
	string txt = SchemaManager::instance().toReplyTxt(body, kPhoneViewId, false,
			fail_ids);
	MCE_INFO("ToPhone.target:" << PrintHelper::Print(desktopiphonetargets) << " txt:" << txt );
	if (!txt.empty()) {
		try {
//			MCE_INFO("tmplog NotifyGateI::instantNotify.call webpager begin" );
			WTransitAdapter::instance().deliverNotifyPhone(desktopiphonetargets, txt);
		} catch (Ice::Exception& e) {
			MCE_WARN("WTransitAdapter to webpager nid:" << body->id << "  error : " << e);
		}
	} else {
//		MCE_INFO("tmplog NotifyGateI::instantNotify.txt empty" );
	}
}

void ToDesktop(const xce::notify::NotifyBodyPtr& body,const vector<int> & desktopiphonetargets,vector<long> & fail_ids){
	string txt = SchemaManager::instance().toReplyTxt(body, kClientViewId, false,
			fail_ids);
	MCE_INFO("ToDesktop.target:" << PrintHelper::Print(desktopiphonetargets) << " txt:" << txt );
	if (!txt.empty()) {
		try {
//			MCE_INFO("ToDesktop.target:VVVVVVVVVVVVVV");
			WTransitAdapter::instance().deliverNotifyClient(desktopiphonetargets, txt);
		} catch (Ice::Exception& e) {
			MCE_WARN("WTransitAdapter to webpager nid:" << body->id << "  error : " << e);
		}
	} else {
//		MCE_INFO("tmplog NotifyGateI::instantNotify.txt empty" );
	}
}

void ToNotifyStorm(const xce::notify::NotifyBodyPtr& body,const vector<int> & desktopiphonetargets,vector<long> & fail_ids){
	string txt = SchemaManager::instance().toReplyTxt(body, kIphoneViewId, false,
			fail_ids);
	if (!txt.empty()) {
		string ids_str = NotifyGateI::Print(desktopiphonetargets);//TODO
		map<string, string> m;
		m["users"] = ids_str;
		m["msg"] = txt;
		m["type"] = lexical_cast<string> (body->type);
		try {
			NotifyStormAdapter::instance().notify("APNSTEST", m);
		} catch (Ice::Exception& e) {
			//				MCE_WARN("NotifyStormAdapter to iphone " << targets_exclude_ugcuids[0] << "  error : " << e);
		}
	}
}

void ToPlatform(const xce::notify::NotifyBodyPtr& body,const vector<int> & desktopiphonetargets,vector<long> & fail_ids){
	string txt = SchemaManager::instance().toReplyTxt(body, kWebViewId, false, fail_ids);
	if (!txt.empty()) {
		string ids_str = NotifyGateI::Print(desktopiphonetargets);
		map<string, string> m;
		m["users"] = ids_str;
		m["type"] = lexical_cast<string> (body->type);
		try {
			MCE_INFO("NotifyStormAdapter. to PLATFORM : nid:" << body->id << desktopiphonetargets.size() << " uids:" << ids_str);
			NotifyStormAdapter::instance().notify("PLATFORM_REPLY", m);
			/*      MCE_INFO("ids_str:" << ids_str);
			 apns::APNSNotifyAdapter::instance().addIphoneNotifyBySeq(
			 targets2_without_from, txt);*/
		} catch (Ice::Exception& e) {
			MCE_WARN("NotifyStormAdapter. to PLATFORM " << desktopiphonetargets[0] << "  error : " << e );
		}
	}
}

bool IsPageNotify(int type,int target){
	static const int kMinPageUid = 600000000;
	static const int kMaxPageUid = 700000000;
	return (
			(type >= 26 && type <= 30)
			&&
			(target >= kMinPageUid && target <=kMaxPageUid 	)
			) ;
}

//void NotifyGateI::deliverWebpager(const NotifyContentPtr& content,
//		const MyUtil::IntSeq& targets, const Ice::Current&) {
//	vector<Ice::Long> fail_ids;
//	if (targets.empty())
//		return;
//	NotifyBodyPtr body = new NotifyBody(content);
//	if (!body)
//		return;
//
//	MCE_INFO("NotifyGateI::deliverWebpager : size: " << targets.size() << " first: " << targets[0] << " nid: " << content->id << " schema: " << content->type );
//
//	string txt = SchemaManager::instance().toReplyTxt(body, kPagerViewId, false,
//			fail_ids);
//	if (!txt.empty()) {
//		try {
//			MCE_DEBUG("deliverWebpager to webpager : " << targets.size() << "-" << targets[0] << "-" << txt);
//			talk::adapter::WTransitAdapter::instance().deliverNotifyWebpager(targets,
//					txt);
//		} catch (Ice::Exception& e) {
//			MCE_WARN("deliverWebpager to webpager " << targets[0] << "  error : " << e);
//		}
//	}
//	if (!fail_ids.empty()) {
//		RemoveIndex(targets, fail_ids);
//	}
//}

string PrintContent(const NotifyContentPtr& cnt){
	ostringstream os;
	os << " nid:" << cnt->id <<" from:" << cnt->from << " type:" << cnt->type;
	return os.str();
}
void NotifyGateI::deliverWebpager(const NotifyContentPtr& content,
		const MyUtil::IntSeq& targets, const Ice::Current&) {
	NotifyBodyPtr body = new NotifyBody(content);
	if (!body) {
		MCE_WARN("NotifyGateI::deliverWebpager: body null");
		return;
	}
	vector<int> targets_without_page;
	BOOST_FOREACH(int i,targets){
		if(!IsPageNotify(content->type,i)){
			targets_without_page.push_back(i);
		}
	}
	vector<long> fail_ids;
	ToWebpager(body, targets_without_page, fail_ids);
	if (!fail_ids.empty()) {
		RemoveIndex(targets, fail_ids);
	}
	MCE_INFO("NotifyGateI::deliverWebpager." << PrintContent(content)
			<< " targets:" << PrintHelper::Print(targets)
			<< " targets_without_page:" << PrintHelper::Print(targets_without_page));
}

void NotifyGateI::instantNotify(const NotifyContentPtr& content,
		const MyUtil::IntSeq& targets, const Ice::Current&) {
	NotifyBodyPtr body = new NotifyBody(content);
	if (!body) {
		MCE_WARN("NotifyGateI::instantNotifyAll: body null");
		return;
	}
	vector<int> targets_without_page;
	BOOST_FOREACH(int i,targets){
		if(!IsPageNotify(content->type,i)){
			targets_without_page.push_back(i);
		}
	}
	vector<long> fail_ids;
	ToWebpager(body, targets_without_page, fail_ids);
	ToDesktop(body, targets_without_page, fail_ids);
	ToPhone(body, targets_without_page, fail_ids);
	ToNotifyStorm(body, targets_without_page, fail_ids);
	ToPlatform(body, targets_without_page, fail_ids);
	if (!fail_ids.empty()) {
		RemoveIndex(targets, fail_ids);
	}
	MCE_INFO("NotifyGateI::instantNotify." << PrintContent(content)
			<< " targets:" << PrintHelper::Print(targets)
			<< " targets_without_page:" << PrintHelper::Print(targets_without_page)
	);
}

void NotifyGateI::BodySeq2StrSeq(const NotifyBodySeq & n, Ice::Int view,
		StrSeq & s, vector<Ice::Long> &fail_ids, bool is_kaixin) {
	for (size_t i = 0; i < n.size(); ++i) {
		// TODO : kaixin or renren?
		string txt = SchemaManager::instance().toReplyTxt(n.at(i), view, is_kaixin,
				fail_ids);
		if (!txt.empty()) {
			s.push_back(txt);
		}
	}
}

void NotifyGateI::BodySeq2StrSeqMerge(const NotifyBodySeq & n, Ice::Int view,
		StrSeq & s, vector<Ice::Long> &fail_ids, bool is_kaixin) {
	if (n.empty()) {
		return;
	}
	typedef map<pair<Int, Long> , NotifyBodySeq> MergeMap;
	MergeMap mm;
	map<time_t, string> time2str;
	size_t cnt = 0;
	for (size_t i = 0; i < n.size(); ++i) {
		Int type = n.at(i)->type & 0x8FFFFFFF;
		Long source = n.at(i)->source;
		if (!MergeConfig::instance().MustMerge(type, view)) {
			string txt = SchemaManager::instance().toReplyTxt(n.at(i), view,
					is_kaixin, fail_ids);
			if (!txt.empty()) {
				time2str[n.at(i)->time] = txt;
			}
			++cnt;
		} else {
			mm[make_pair(type, source)].push_back(n.at(i));
		}
	}
	if (!mm.empty()) {
		for (MergeMap::iterator i = mm.begin(); i != mm.end(); ++i) {
			string txt = SchemaManager::instance().toReplyTxtMerge(i->second, view,
					is_kaixin, fail_ids);
			if (!txt.empty()) {
				time_t max_time = 0;
				for (size_t k = 0; k != i->second.size(); k++) {
					max_time = i->second.at(k)->time > max_time ? i->second.at(k)->time
							: max_time;
				}
				time2str[max_time] = txt;
			}
		}
	}

	for (map<time_t, string>::reverse_iterator i = time2str.rbegin(); i
			!= time2str.rend(); ++i) {
		s.push_back(i->second);
	}

	return;
}

void NotifyGateI::BodySeq2StrSeqX(const NotifyBodySeq & n, Ice::Int view,
		StrSeq & s, vector<Ice::Long> &fail_ids, bool is_kaixin) {
	if (n.empty()) {
		return;
	}

	typedef map<pair<Int, Long> , NotifyBodySeq> MergeMap;
	MergeMap mm;
	map<time_t, vector<string> > time2str;
	size_t cnt = 0;
	for (size_t i = 0; i < n.size(); ++i) {
		Int type = n.at(i)->type & 0x8FFFFFFF;
    if(type == 193){
      fail_ids.push_back(n.at(i)->id);
      MCE_INFO("NotifyGateI::BodySeq2StrSeqX.kill type:" << type <<  " nid:" << n.at(i)->id);
    }
		Long source = n.at(i)->source;
		if (!SchemaManager::instance().MustMerge(type, view)) {
			string txt = SchemaManager::instance().toReplyTxt(n.at(i), view,
					is_kaixin, fail_ids);
			if (!txt.empty()) {
				time2str[n.at(i)->time].push_back(txt);
				//        MCE_INFO("NotifyGateI::BodySeq2StrSeqX. no merge uid:" << n.at(i)->id << " type:" << type << " txt not empty");
			} else {
				//      MCE_INFO("NotifyGateI::BodySeq2StrSeqX. no merge uid:" << n.at(i)->id << " type:" << type << " txt empty");
			}
			++cnt;
		} else {
			//      MCE_INFO("NotifyGateI::BodySeq2StrSeqX. merge uid:" << n.at(i)->id << " type:" << type );
			mm[make_pair(type, source)].push_back(n.at(i));
		}
	}
	if (!mm.empty()) {
		for (MergeMap::iterator i = mm.begin(); i != mm.end(); ++i) {
			string txt = SchemaManager::instance().toReplyTxtMerge(i->second, view,
					is_kaixin, fail_ids);
			int type = i->second.at(0)->type;
			if (!txt.empty()) {
				time_t max_time = 0;
				for (size_t k = 0; k != i->second.size(); k++) {
					max_time = i->second.at(k)->time > max_time ? i->second.at(k)->time
							: max_time;
				}
				time2str[max_time].push_back(txt);
				//        MCE_INFO("NotifyGateI::BodySeq2StrSeqX. txt not empty. type:" << type << " id:" << i->second.at(0)->id );
			} else {
				//        MCE_INFO("NotifyGateI::BodySeq2StrSeqX. txt empty. type:" << type << " id:" << i->second.at(0)->id );
			}
		}
	}

	for (map<time_t, vector<string> >::reverse_iterator i = time2str.rbegin(); i
			!= time2str.rend(); ++i) {
		s.insert(s.end(), i->second.begin(), i->second.end());
		//    s.push_back(i->second);
	}

	return;
}

StrSeq NotifyGateI::getNotifySeq(Ice::Int uid, Ice::Int view,
		Ice::Long last_nid, Ice::Int limit, const Ice::Current&) {
	MCE_INFO("NotifyGateI::getNotifySeq. uid:" << uid << " view:" << view << " last_nid:" << last_nid
			<< " limit:" << limit);
	StrSeq ret;
	NotifyBodySeq seq;
	vector<Ice::Long> fail_ids;
	try {
		seq = NotifyIndexAdapter::instance().getNotifySeq(uid, last_nid, limit);
		BodySeq2StrSeq(seq, view, ret, fail_ids);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getNotifySeq exception : " << e);
	}
	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	MCE_INFO("NotifyGateI::getNotifySeq. uid:" << uid << " NotifyBodySeq size:" << seq.size() <<" result size:" << ret.size());
	return ret;
}

StrSeq NotifyGateI::getNotifySeqX(Ice::Int uid, Ice::Int view,
		Ice::Long last_nid, Ice::Int limit, const Ice::Current&) {
	StrSeq ret;
	NotifyBodySeq seq;
	vector<Ice::Long> fail_ids;
	try {
		seq = NotifyIndexAdapter::instance().getNotifySeqX(uid, view, last_nid,
				limit);
		BodySeq2StrSeqX(seq, view, ret, fail_ids);

	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getNotifySeq exception : " << e << " uid:" << uid);
	}
	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	MCE_INFO("NotifyGateI::getNotifySeqX. uid:" << uid << " NotifyBodySeq size:" << seq.size() <<" result size:" << ret.size());
	return ret;
}

StrSeq NotifyGateI::getReplySeqX(Ice::Int uid, Ice::Int view,
		Ice::Long last_nid, Ice::Int limit, const Ice::Current&) {
	StrSeq ret;
	NotifyBodySeq seq;
	vector<Ice::Long> fail_ids;
	try {
		seq = NotifyIndexAdapter::instance().getReplySeqX(uid, view, last_nid,
				limit);
		BodySeq2StrSeqX(seq, view, ret, fail_ids);

	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getReplySeqX exception : " << e << " uid:" << uid);
	}
	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	MCE_INFO("NotifyGateI::getReplySeqX. uid:" << uid << " NotifyBodySeq size:" << seq.size() <<" result size:" << ret.size());
	return ret;
}

StrSeq NotifyGateI::getReplySeq(Ice::Int uid, Ice::Int view,
		Ice::Long last_nid, Ice::Int limit, const Ice::Current&) {
	MCE_INFO("NotifyGateI::getReplySeq. uid:" << uid << " view:" << view << " last_nid:" << last_nid
			<< " limit:" << limit);
	StrSeq ret;
	NotifyBodySeq seq;
	vector<Ice::Long> fail_ids;
	try {
		seq = NotifyIndexAdapter::instance().getReplySeq(uid, last_nid, limit);
		BodySeq2StrSeq(seq, view, ret, fail_ids);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getReplySeq exception : " << e << " uid:" << uid << " view:" << view << " last_nid:" << last_nid
				<< " limit:" << limit );
	}
	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	return ret;
}

StrSeq NotifyGateI::getByType(Ice::Int uid, Ice::Int view,
		const MyUtil::IntSeq& types, Ice::Long last_nid, Ice::Int limit,
		const Ice::Current&) {
	MCE_INFO("NotifyGateI::getByType. uid:" << uid << " view:" << view << " last_nid:" << last_nid
			<< " limit:" << limit << " types size:" << types.size());
	StrSeq ret;
	NotifyBodySeq seq;
	try {
		seq = NotifyIndexAdapter::instance().getByType(uid, types, last_nid, limit);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getByType exception : " << e<< " uid:" << uid << " view:" << view
				<< " last_nid:" << last_nid << " limit:" << limit << " types size:" << types.size());
		return ret;
	}
	vector<Ice::Long> fail_ids;
	BodySeq2StrSeq(seq, view, ret, fail_ids);
	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	return ret;
}

StrSeq NotifyGateI::getNotifySeq2(Ice::Int uid, Ice::Int view, Ice::Int begin,
		Ice::Int limit, const Ice::Current&) {
	StrSeq ret;
	NotifyBodySeq seq;
	TimeStat t1, ttotal;
	float index_t = 0.0f, tostr_t = 0.0f, total_t;
	try {
		seq = NotifyIndexAdapter::instance().getNotifySeq2(uid, begin, limit);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getNotifySeq2 exception : " << e << " uid:" << uid << " view:" << view
				<< " begin:" << begin << " limit:" << limit);
		return ret;
	}
	index_t = t1.getTime();
	vector<Ice::Long> fail_ids;
	BodySeq2StrSeq(seq, view, ret, fail_ids);

	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	tostr_t = t1.getTime();
	total_t = ttotal.getTime();
	ostringstream oss;
	oss << " index:" << index_t << " tostr:" << tostr_t << " total:" << total_t;
	MCE_INFO("NotifyGateI::getNotifySeq2 seq.size(): " << seq.size() << " ret.size()=" << ret.size() << " uid:" << uid << " view:" << view
			<< " begin:" << begin << " limit:" << limit << " #COST#" << oss.str() );
	return ret;
}

StrSeq NotifyGateI::getReplySeq2(Ice::Int uid, Ice::Int view, Ice::Int begin,
		Ice::Int limit, const Ice::Current&) {
	MCE_INFO("NotifyGateI::getReplySeq2. uid:" << uid << " view:" << view << " begin:" << begin
			<< " limit:" << limit);
	StrSeq ret;
	NotifyBodySeq seq;
	TimeStat t1, ttotal;
	float index_t = 0.0f, tostr_t = 0.0f, total_t;
	try {
		seq = NotifyIndexAdapter::instance().getReplySeq2(uid, begin, limit);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyGateI::getNotifySeq exception : " << e << " uid:" << uid << " view:" << view
				<< " begin:" << begin << " limit:" << limit);
		return ret;
	}
	index_t = t1.getTime();
	vector<Ice::Long> fail_ids;
	BodySeq2StrSeq(seq, view, ret, fail_ids);

	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	tostr_t = t1.getTime();
	total_t = ttotal.getTime();
	ostringstream oss;
	oss << " index:" << index_t << " tostr:" << tostr_t << " total:" << total_t;
	MCE_INFO("NotifyGateI::getReplySeq2 seq.size(): " << seq.size() << " ret.size()=" << ret.size() << " uid:" << uid << " view:" << view
			<< " begin:" << begin << " limit:" << limit << " #COST#" << oss.str());
	return ret;
}

StrSeq NotifyGateI::getByType2(Ice::Int uid, Ice::Int view,
		const MyUtil::IntSeq& types, Ice::Int begin, Ice::Int limit,
		const Ice::Current&) {
	StrSeq ret;
	NotifyBodySeq seq;
	TimeStat t1, ttotal;
	float index_t = 0.0f, tostr_t = 0.0f, total_t;
	try {
		seq = NotifyIndexAdapter::instance().getByType2(uid, types, begin, limit);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyIndexAdapter::getByType2() exception : " << e << " uid:" << uid << " view:" << view
				<< " begin:" << begin << " limit:" << limit);
		return ret;
	}
	index_t = t1.getTime();
	vector<Ice::Long> fail_ids;
	BodySeq2StrSeq(seq, view, ret, fail_ids);

	if (!fail_ids.empty()) {
		RemoveIndex(uid, fail_ids);
	}
	tostr_t = t1.getTime();
	total_t = ttotal.getTime();
	ostringstream oss;
	oss << " index:" << index_t << " tostr:" << tostr_t << " total:" << total_t;
	MCE_INFO("getByType2 seq.size(): " << seq.size() << " ret.size()=" << ret.size() << " uid:" << uid << " view:" << view
			<< " begin:" << begin << " limit:" << limit << " types size:" << types.size() << " #COST#" << oss.str());
	return ret;
}

bool RemoveRequest::response() {
	//
	// remove.notify?uid=1234&nid=12&source=12-123
	//
	if (!_fcgi_out) {
		MCE_WARN("remove notify fcgi_out null");
		return false;
	}

	if (!Request::response()) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
		return false;
	}

	MCE_DEBUG("ticket verify ok");

	Ice::Long nid = getPropertyLong("nid");

	Ice::Int type = -1;
	Ice::Long source = -1;
	vector<Long> nids;
	string url_nl = getProperty("nl");
	string url_source = getProperty("source");

	try {
		boost::tokenizer<> tok(url_nl);
		for (boost::tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
			nids.push_back(lexical_cast<Long> (*tit));
		}
	} catch (...) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html; charset=utf-8\r\n\r\n-1");
		MCE_DEBUG("seperate notify id list fail");
		return false;
	}

	/*  //TODO delete
	 MCE_DEBUG("nids.size:" << nids.size());
	 for (size_t i = 0; i != nids.size(); ++i) {
	 MCE_DEBUG(nids.at(i));
	 }*/

	size_t pos = url_source.find_first_of(SOURCE_SEPERATOR);
	if (pos != string::npos) {
		try {
			type = boost::lexical_cast<Ice::Int>(url_source.substr(0, pos));
			source = boost::lexical_cast<Ice::Long>(url_source.substr(pos + 1));
		} catch (...) {
			FCGX_FPrintF(_fcgi_out,
					"Content-type: text/html; charset=utf-8\r\n\r\n-1");
			return false;
		}
	}

	if (nids.empty() && nid == -1 && source == -1) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html; charset=utf-8\r\n\r\n-1");
		return false;
	}
	try {
		if (!nids.empty()) {
			MCE_DEBUG("call removeByIds");
			NotifyIndexAdapter::instance().removeByIds(_login_uid, nids);
		} else if (source >= 0) {
			MCE_DEBUG("call removeBySource");
			NotifyIndexAdapter::instance().removeBySource(_login_uid, type, source);
		} else if (nid >= 0) {
			MCE_DEBUG("call removeById");
			NotifyIndexAdapter::instance().removeById(_login_uid, nid);
		}
	} catch (Ice::Exception& e) {
		MCE_DEBUG("NotifyIndexAdapter remove exception : " << e << " uid=" << _login_uid);
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-2");
		return false;
	}
	FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n0");
	return true;
}

bool GetRequest::response() {
	//
	// get.notify?type=1,2,3&view=123&nid=123&limit=10
	//
	//
	float index_t = 0.0f;
	float tostr_t = 0.0f;
	float remove_t = 0.0f;
	TimeStat t1, ttotal;
	if (!_fcgi_out) {
		MCE_WARN("get notify fcgi_out null");
		return false;
	}

	if (!Request::response()) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
		return false;
	}

	/*    try{
	 MCE_INFO("GetRequest::response. call FeedNewsAdapter ");
	 FeedNewsAdapter::instance().load(_login_uid);
	 }catch(...){
	 MCE_INFO("GetRequest::response. call FeedNewsAdapter,error");
	 }
	 MCE_DEBUG("X get response uid:" << _login_uid << "," << __LINE__);*/

	string emptyRet = "Content-type: text/html\r\n\r\n[]";

	MCE_DEBUG("ticket verify ok");

	NotifyBodySeq seq;

	string type_str = getProperty("type");
	Ice::Int view = getPropertyInt("view");
	Ice::Long nid = getPropertyLong("nid");
	Ice::Int limit = getPropertyInt("limit");
	Ice::Int reply = getPropertyInt("reply");

	if (view < 0) {
		MCE_DEBUG("get.notify param error. view = " << view);
		FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
		return false;
	}

	if (limit <= 0)
		limit = 10;

	MyUtil::IntSeq typeSeq;
	if (!type_str.empty()) {
		try {
			size_t pos = 0;
			size_t end;
			// TODO : user boost::split
			while ((end = type_str.find(",", pos)) != string::npos) {
				typeSeq.push_back(boost::lexical_cast<int>(type_str.substr(pos, end
						- pos)));
				pos = end + 1;
			}

			typeSeq.push_back(boost::lexical_cast<int>(type_str.substr(pos,
					type_str.length() - pos)));
		} catch (boost::bad_lexical_cast & e) {
			MCE_WARN("bad_lexical_cast. line : " << __LINE__ << "," << e.what());
		}
	}

	try {
		if (typeSeq.empty()) {
			if (reply > 0) {
				seq = NotifyIndexAdapter::instance().getReplySeqX(_login_uid, view,
						nid, limit);

			} else {
				seq = NotifyIndexAdapter::instance().getNotifySeqX(_login_uid, view,
						nid, limit);
			}
		} else {
			seq = NotifyIndexAdapter::instance().getByType(_login_uid, typeSeq, nid,
					limit);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyIndexAdapter get exception : " << e << " uid=" << _login_uid);
	}

	index_t = t1.getTime();

	MCE_DEBUG("seq.size() :  " << seq.size());

	string ss;
	ss = "Content-type: text/html; charset=utf-8\r\n\r\n[";

	/////////////yjw替换原来的字符串生成代码//////////////////
	int json_count = 0;
	bool is_kaixin = (host().find("kaixin") != string::npos);

	StrSeq ret;
	vector<Ice::Long> fail_ids;
	NotifyGateI::instance().BodySeq2StrSeqX(seq, view, ret, fail_ids, is_kaixin);

	for (size_t h = 0; h < ret.size(); ++h) {
		string txt = ret.at(h);
		if (++json_count > 1)
			ss += ',';
		ss += txt;
	}
	ss += ']';
	////////////////////////////////////////
	tostr_t = t1.getTime();

	FCGX_FPrintF(_fcgi_out, "%s", ss.c_str());

	try {
//		MCE_INFO("#### call FeedNewsAdapter.begin");
		if(_login_uid % 10 < 10 ){
//		 xce::feed::FeedNewsAdapter::instance().load(_login_uid);
		}
		    //xce::feed::FeedViewAdapter::instance().Test(_login_uid,"ABC",0,0,20);
//			MCE_INFO("#### call FeedNewsAdapter.end");
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedNewsAdapter Test exception : " << e << " uid=" << _login_uid);
	}

	if (!fail_ids.empty()) {
		RemoveIndex(_login_uid, fail_ids);
	}
	remove_t = t1.getTime();
	ostringstream oss;
	oss << " index:" << index_t << " tostr:" << tostr_t << " remove:" << remove_t
			<< " total:" << ttotal.getTime();
	MCE_INFO("GetRequest::response. uid: " << _login_uid << " get.notify body_size=" << ss.size()
			<< " notify_count=" << seq.size() << " ret size:" << ret.size()
			<< " type: " << type_str
			<< " nid: " << nid
			<< " limit: " << limit
			<< " reply: " << reply
			<< " #COST# " << oss.str());
	return true;
}

bool checkMd5(string uid, const string & key) {
	string orig = uid;
	uid += kClubDispatchKey;
	unsigned char* md5 = MD5((unsigned char*) uid.c_str(), uid.size(), NULL);

	stringstream res;
	for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		res << hex << setw(2) << setfill('0') << (unsigned int) md5[i];
	}
	MCE_INFO("checkMd5.input seed:"<< orig <<",generate md5:" << res.str() << ",input key:" << key );
	return res.str() == key;
}

bool DispatchRequest::response() {
	//
	// dispatch.notify?schema=32&type=1
	// POST BODY : source=123&from=1231&....
	//
	if (!_fcgi_out) {
		MCE_WARN("get notify fcgi_out null");
		return false;
	}

	//Request::response(); // 不调用基类response()

	MyUtil::Str2StrMap dispatch_map;

	string schema = getProperty("schema");
	string type = getProperty("type");
	string post_body = UriDecode(getProperty("post_body"));
	MCE_INFO("DispatchRequest::response. schema:" << schema << " type:" << type);
	if (schema.empty() || type.empty() || post_body.empty()) {
		MCE_DEBUG("dispatch.notify param error");
		FCGX_FPrintF(_fcgi_out,
				"Content-type: text/html\r\n\r\n-2,schema type or post is empty");
		return false;
	}

	dispatch_map.insert(make_pair("schema_id", schema));
	dispatch_map.insert(make_pair("type", type));

	std::vector<std::string> strs;
	boost::split(strs, post_body, boost::is_any_of("&"), boost::token_compress_on);
	for (size_t i = 0; i < strs.size(); ++i) {
		size_t pos = strs[i].find('=');
		if (pos != string::npos) {
			dispatch_map.insert(make_pair(strs[i].substr(0, pos), strs[i].substr(pos
					+ 1)));
		}
	}

	MyUtil::Str2StrMap::iterator it = dispatch_map.find("key");

	if (it == dispatch_map.end()) {
		//kClubDispatchKey
		string out = "Content-type: text/html\r\n\r\n-4,md5 key is absent";
		FCGX_FPrintF(_fcgi_out, out.c_str());
		return false;
	} else {
		MyUtil::Str2StrMap::iterator f = dispatch_map.find("from");
		MyUtil::Str2StrMap::iterator t = dispatch_map.find("to");
		MyUtil::Str2StrMap::iterator tp = dispatch_map.find("type");
		if (it == dispatch_map.end() || f == dispatch_map.end() || t
				== dispatch_map.end() || tp == dispatch_map.end()) {
			FCGX_FPrintF(_fcgi_out,
					"Content-type: text/html\r\n\r\n-5,from,to or type is absent");
			return false;
		}
		string tmp;
		tmp = f->second + "|" + t->second + "|" + tp->second;
		if (!checkMd5(tmp, it->second)) {
			FCGX_FPrintF(_fcgi_out,
					"Content-type: text/html\r\n\r\n-6,MD5 not match,");
			return false;
		}
	}

	dispatch_map.erase("key");

	try {
		NotifyDispatcherAdapter::instance().dispatch(dispatch_map);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyDispatcherAdapter dispatch exception : " << e <<" uid=" << _login_uid );
		string out = "Content-type: text/html\r\n\r\n-3,call notify fail";
		out += e.what();
		FCGX_PutS(out.c_str(), _fcgi_out);
		return false;
	}

	FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n0,success");
	return true;
}

bool UnreadCountRequest::response() {
	//
	// unreadcount.notify?uid=1234
	//
	if (!_fcgi_out) {
		MCE_WARN("pager count fcgi_out null");
		return false;
	}

	if (!Request::response()) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
		return false;
	}

	string emptyRet = "Content-type: text/html\r\n\r\n[]";

	MCE_DEBUG("ticket verify ok");

	Ice::Int count = 0;
	try {
		count = NotifyIndexAdapter::instance().getUnreadCount(_login_uid);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyIndexAdapter get pager count exception : " << e << " uid=" << _login_uid);
	}

	stringstream ss;
	ss << "Content-type: text/html; charset=utf-8\r\n\r\n";
	ss << count;

	FCGX_FPrintF(_fcgi_out, ss.str().c_str());
	return true;
}

bool QuitChatRequest::response() {
	//
	// quitchat.notify?uid=1234
	//
	if (!_fcgi_out) {
		MCE_WARN("pager count fcgi_out null");
		return false;
	}

	if (!Request::response()) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
		return false;
	}

	MCE_DEBUG("ticket verify ok");

	string groups_str = getProperty("groups");
	GroupIndexSeq groups;
	try {
		tokenizer<> tok(groups_str);
		int size = 0;
		for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
			++size;
		}
		if (size == 0 || size % 3 != 0) {
			MCE_WARN("QuitChatRequest::response wrong fomat! groups_str:" << groups_str);
			FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
			return false;
		}
		for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end();) {
			GroupIndex g;
			g.source = lexical_cast<Ice::Long> (*tit++);
			g.stype = lexical_cast<Ice::Int> (*tit++);
			g.actor = lexical_cast<Ice::Int> (*tit++);
			groups.push_back(g);
		}
	} catch (...) {
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html; charset=utf-8\r\n\r\n-2");
		return false;
	}

	MCE_DEBUG("QuitChat handle, uid:" << _login_uid <<":groups size:" << groups.size());
	try {
		NotifyDispatcherNAdapter::instance().QuitChat(_login_uid, groups);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyDispatcherNAdapter QuitChat FCGI exception : " << e << " uid=" << _login_uid);
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-3");
		return false;
	}
	FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n0");
	return true;
}
string NotifyTargetHandler::ToString(const NotifyTargetPtr & t){
	ostringstream os;
	os << " uids:" << PrintHelper::Print(t->uids)
			<< " minigroups:" << PrintHelper::Print(t->minigroups)
			<< " minigroupMembers:" <<PrintHelper::Print( t->minigroupMembers)
			<< " ugcUids:" <<PrintHelper::Print( t->ugcUids)
			<< " ugcMinigroups:" << PrintHelper::Print(t->ugcMinigroups)
			<< " ugcBlockUids:" <<PrintHelper::Print( t->ugcBlockUids);
	return os.str();
}

