#include "CommonTask.h"
#include "PresenceHandlerI.h"
#include "XmppTools.h"
#include "PresenceAdapter.h"
#include "IceLogger.h"
#include "OnlineCenterAdapter.h"
#include "TalkDeliverAdapter.h"
#include "QueryRunner.h"
#include <boost/lexical_cast.hpp>
#include "CountCacheAdapter.h"
#include "UserRoomAdapter.h"
#include "OccupantsAdapter.h"
#include "MucDeliverAdapter.h"
#include "UserCacheAdapter.h"
#include "IMStormAdapter.h"

#include "FeedMemcProxy/client/talk_cache_client.h"
using namespace xce::talk;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace talk::online::adapter;
using namespace com::xiaonei::xce;
using namespace talk::userroom::adapter;
using namespace talk::occupants::adapter;
using namespace talk::mucdeliver::adapter;
using namespace xce::usercache;
//------------------------------------------------------
//  Task
//------------------------------------------------------
void PresenceNotifyTask::_notifyItsOtherAvatars(){
	MCE_DEBUG("PresenceNotifyTask::_notifyItsOtherAvatars --> ");
	for(size_t i=0; i<_activeAvatars.size(); i++){
		xml_document doc;
		xml_node presenceNode = doc.append_child();
		presenceNode.set_name("presence");
		presenceNode.append_attribute("from") = jidToString(_activeAvatars.at(i)->jid).c_str();

		if(_activeAvatars.at(i)->stat == OFFLINE){
			presenceNode.append_attribute("type") = "unavailable";
		}else{
			xml_node cNode = presenceNode.append_child();
			cNode.set_name("c");
			cNode.append_attribute("node") = "http://talk.xiaonei.com/xmpp/client/caps";
			cNode.append_attribute("xmlns") = "http://jabber.org/protocol/caps";

			if(!_parameters["DOING"].empty()){
				xml_node statusNode = presenceNode.append_child();
				statusNode.set_name("status");
				statusNode.append_child(node_pcdata).set_value(_parameters["DOING"].c_str());
			}
			if(!_parameters["HTMLDOING"].empty()){
				xml_node embHtmlNode = presenceNode.append_child();
				embHtmlNode.set_name("richstatus");
				embHtmlNode.append_child(node_pcdata).set_value(_parameters["HTMLDOING"].c_str());
			}
			if(!_parameters["VIP"].empty()){
				xml_node vipNode = presenceNode.append_child();
				vipNode.set_name("vip");
				vipNode.append_child(node_pcdata).set_value(_parameters["VIP"].c_str());
			}

			xml_node showNode = presenceNode.append_child();
			showNode.set_name("show");
			showNode.append_child(node_pcdata).set_value(onlineStatStr(_activeAvatars.at(i)->stat).c_str());
			cNode.append_attribute("ver") = _activeAvatars.at(0)->ver.c_str();
			cNode.append_attribute("ext") = _activeAvatars.at(0)->ext.c_str();

			xml_node xNode = presenceNode.append_child();
			xNode.set_name("x");
			xNode.append_attribute("xmlns") = "vcard-temp:x:update";
			xml_node photoNode = xNode.append_child();
			photoNode.set_name("PHOTO");
			xml_node extvalNode = photoNode.append_child();
			extvalNode.set_name("EXTVAL");
			extvalNode.append_child(node_pcdata).set_value(_parameters["PHOTO"].c_str());
		}

		for(size_t j=0; j<_selfOtherAvatars.size(); j++){
			presenceNode.append_attribute("to") = jidToString(_selfOtherAvatars.at(j)->jid).c_str();
			MessagePtr mPtr = new Message();
			mPtr->from = _activeAvatars.at(i)->jid;
			mPtr->to = _selfOtherAvatars.at(j)->jid;
			mPtr->type = PRESENCE_COMMON;
			mPtr->msg = doc.xml();
			MCE_DEBUG("~~~ " << mPtr->msg);
			_mSeq.push_back(mPtr);
		}
	}
}

void PresenceNotifyTask::_notifyAvatarToBuddies(){
	int userId = _activeAvatars.at(0)->jid->userId;
	//MCE_INFO("PresenceNotifyTask::_notifyAvatarToBuddies --> userId : " << userId);
	for(size_t i=0; i<_activeAvatars.size(); i++){

		xml_document doc;
		xml_node presenceNode = doc.append_child();
		presenceNode.set_name("presence");
		presenceNode.append_attribute("from") = jidToString(_activeAvatars.at(i)->jid).c_str();

		if(_activeAvatars.at(i)->stat == OFFLINE || _activeAvatars.at(i)->stat == INVISIABLE){
			presenceNode.append_attribute("type") = "unavailable";
		}else{
			xml_node cNode = presenceNode.append_child();
			cNode.set_name("c");
			cNode.append_attribute("node") = "http://talk.xiaonei.com/xmpp/client/caps";
			cNode.append_attribute("xmlns") = "http://jabber.org/protocol/caps";

			//MCE_INFO("PresenceNotifyTask::_notifyAvatarToBuddies --> userId : " << userId << " " << _parameters["DOING"] << "    " << _parameters["HTMLDOING"]);
			if(!_parameters["DOING"].empty()){
				xml_node statusNode = presenceNode.append_child();
				statusNode.set_name("status");
				statusNode.append_child(node_pcdata).set_value(_parameters["DOING"].c_str());
			}
			if(!_parameters["HTMLDOING"].empty()){
				xml_node embHtmlNode = presenceNode.append_child();
				embHtmlNode.set_name("richstatus");
				embHtmlNode.append_child(node_pcdata).set_value(_parameters["HTMLDOING"].c_str());
			}
			if(!_parameters["VIP"].empty()){
				xml_node vipNode = presenceNode.append_child();
				vipNode.set_name("vip");
				vipNode.append_child(node_pcdata).set_value(_parameters["VIP"].c_str());
			}

			xml_node showNode = presenceNode.append_child();
			showNode.set_name("show");
			showNode.append_child(node_pcdata).set_value(onlineStatStr(_activeAvatars.at(i)->stat).c_str());

			cNode.append_attribute("ver") = _activeAvatars.at(i)->ver.c_str();
			cNode.append_attribute("ext") = _activeAvatars.at(i)->ext.c_str();

			xml_node xNode = presenceNode.append_child();
			xNode.set_name("x");
			xNode.append_attribute("xmlns") = "vcard-temp:x:update";
			xml_node photoNode = xNode.append_child();
			photoNode.set_name("PHOTO");
			xml_node extvalNode = photoNode.append_child();
			extvalNode.set_name("EXTVAL");
			extvalNode.append_child(node_pcdata).set_value(_parameters["PHOTO"].c_str());
		}

		for(AvatarMap::const_iterator it=_buddies.begin(); it!=_buddies.end(); ++it){
			for(size_t j=0; j<it->second.size(); j++){
				presenceNode.append_attribute("to") = jidToString(it->second.at(j)->jid).c_str();
				MessagePtr mPtr = new Message();
				mPtr->from = _activeAvatars.at(i)->jid;
				mPtr->to = it->second.at(j)->jid;
				mPtr->type = PRESENCE_COMMON;
				//string str = doc.xml();
				mPtr->msg = doc.xml();
				MCE_DEBUG("~~~ " << mPtr->msg);
				_mSeq.push_back(mPtr);
			}
		}
	}
}

void PresenceNotifyTask::_notifyOtherAvatarsToBuddies(){
	MCE_DEBUG("PresenceNotifyTask::_notifyOtherAvatarsToBuddies --> ");
	bool invisible = false;
	for(size_t i=0; i<_activeAvatars.size(); i++){
		if(_activeAvatars.at(i)->stat == INVISIABLE){
			invisible = true;
			break;
		}
	}

	xml_document doc;
	xml_node presenceNode = doc.append_child();
	presenceNode.set_name("presence");
	xml_node cNode = presenceNode.append_child();
	cNode.set_name("c");

	if(invisible){
		presenceNode.append_attribute("type") = "unavailable";
	}else{
		cNode.append_attribute("node") = "http://talk.xiaonei.com/xmpp/client/caps";
		cNode.append_attribute("xmlns") = "http://jabber.org/protocol/caps";

		if(!_parameters["DOING"].empty()){
			xml_node statusNode = presenceNode.append_child();
			statusNode.set_name("status");
			statusNode.append_child(node_pcdata).set_value(_parameters["DOING"].c_str());
		}
		if(!_parameters["HTMLDOING"].empty()){
			xml_node embHtmlNode = presenceNode.append_child();
			embHtmlNode.set_name("richstatus");
			embHtmlNode.append_child(node_pcdata).set_value(_parameters["HTMLDOING"].c_str());
		}
		if(!_parameters["VIP"].empty()){
			xml_node vipNode = presenceNode.append_child();
			vipNode.set_name("vip");
			vipNode.append_child(node_pcdata).set_value(_parameters["VIP"].c_str());
		}

		xml_node xNode = presenceNode.append_child();
		xNode.set_name("x");
		xNode.append_attribute("xmlns") = "vcard-temp:x:update";
		xml_node photoNode = xNode.append_child();
		photoNode.set_name("PHOTO");
		xml_node extvalNode = photoNode.append_child();
		extvalNode.set_name("EXTVAL");
		extvalNode.append_child(node_pcdata).set_value(_parameters["PHOTO"].c_str());
	}

	for(size_t i=0; i<_selfOtherAvatars.size(); i++){
		presenceNode.append_attribute("from") = jidToString(_selfOtherAvatars.at(i)->jid).c_str();
		cNode.append_attribute("ver") = _selfOtherAvatars.at(i)->ver.c_str();
		cNode.append_attribute("ext") = _selfOtherAvatars.at(i)->ext.c_str();
		if(presenceNode.child("show")){
			presenceNode.remove_child("show");
		}
		if(!invisible){
			xml_node showNode = presenceNode.append_child();
			showNode.set_name("show");
			showNode.append_child(node_pcdata).set_value(onlineStatStr(_selfOtherAvatars.at(i)->stat).c_str());
		}
		for(AvatarMap::const_iterator it=_buddies.begin(); it!=_buddies.end(); ++it){
			for(size_t j=0; j<it->second.size(); j++){
				presenceNode.append_attribute("to") = jidToString(it->second.at(j)->jid).c_str();
				MessagePtr mPtr = new Message();
				mPtr->from = _selfOtherAvatars.at(i)->jid;
				mPtr->to = it->second.at(j)->jid;
				mPtr->type = PRESENCE_COMMON;
				//string str = doc.xml();
				mPtr->msg = doc.xml();
				MCE_DEBUG("~~~ " << mPtr->msg);
				_mSeq.push_back(mPtr);
			}
		}
	}
}

void PresenceNotifyTask::handle() {
	StatFunc statF("PresenceNotifyTask::handle");
	float v = task_ts.getTime();
	MCE_INFO("PresenceNotifyTask::handle --> befor handle cost_time = " << v << " late : " << v - 100.0);
	//TimeStat ts;
	string doing = _parameters["DOING"];
	string htmlDoing = _parameters["HTMLDOING"];
	string photo = _parameters["PHOTO"];
	if(doing.empty() || photo.empty()){
		MyUtil::IntSeq ids;
		ids.push_back(_activeAvatars.at(0)->jid->userId);
		TalkUserMap usermaps;
		try {
			TimeStat talkcache_ts;
			usermaps = TalkCacheClient::instance().GetUserBySeqWithLoad(_activeAvatars.at(0)->jid->userId, ids);
			MONITOR("SignForTimeuse::PresenceNotifyTask::handle-->TalkCacheClient::GetUserBySeqWithLoad-->" << talkcache_ts.getTime());
		} catch(Ice::Exception& e) {
			MCE_WARN("PresenceNotifyTask::handle-->TalkCacheClient::GetUserBySeqWithLoad--> "<<e);
		}
		TalkUserMap::iterator it = usermaps.find(_activeAvatars.at(0)->jid->userId);
		if(it!=usermaps.end()){
			_user = it->second;
		}
		if(doing.empty()){
			_parameters["DOING"] = _user->statusOriginal.c_str();
		}
		if(photo.empty()){
			_parameters["PHOTO"] = buildHeadUrl(_user->headurl).c_str();
		}
	}

	int userId = _activeAvatars.at(0)->jid->userId;
	//MCE_INFO("PresenceNotifyTask::handle --> " << userId << " " << doing << " " << htmlDoing << " " << photo);
	IntSeq ids;
	ids.push_back(userId);
	map<int, UserCachePtr> userVips;
	try{
		userVips = UserCacheAdapter::instance().GetUserCache(ids);
	}catch(Ice::Exception& e){
		MCE_WARN("PresenceHandlerI::presence-->UserCacheAdapter::GetUserCache-->userId:" << userId << ":" << e);
	}
	//MCE_INFO("## get vip " << ids.size() << " " << userVips.size());

	int vip = 0;
	UserCachePtr vipInfo;
	map<int, UserCachePtr>::iterator it = userVips.find(userId);
	if(it != userVips.end()){
		vipInfo = it->second;
		//MCE_INFO("## vip info " << userId << " " << vipInfo->level() << " " << vipInfo->vip() << " " << vipInfo->ever_vip());
		vip = vipInfo->level();
		if(vip == 0){
			vip = 1;
		}
		if(vipInfo->IsVipMember()){
		}else if(vipInfo->IsEverVipMember()){
			vip = -vip;
		}else{
			vip = 0;
		}
	}
	string strVip;
	try{
		strVip = boost::lexical_cast<string>(vip);
	}catch(...){
		MCE_WARN("PresenceHandlerI::presence --> lexical cast err");
	}
	_parameters["VIP"] = strVip;

	MCE_DEBUG("PresenceNotifyTask::handle --> notify type " << _type);
	if(!_parameters["HTMLDOING"].empty()){
		//如果用户修改了自己的doing，则要通知给他自己
		MCE_INFO("PresenceNotifyTask::handle --> userId : " << userId << " motify doing so notify self htmldoing = " << _parameters["HTMLDOING"]);
		_selfOtherAvatars.insert(_selfOtherAvatars.begin(), _activeAvatars.begin(), _activeAvatars.end());	
	}
	_parameters["HTMLDOING"] = _user->statusShifted;
	bool same = (_parameters["DOING"] == _user->statusOriginal);
	//MCE_INFO("PresenceNotifyTask::handle --> userId : " << userId << " after  _parameters[HTMLDOING] = _user->statusShifted= " << _parameters["HTMLDOING"]);
	ostringstream os;
	os << "PresenceNotifyTask::handle --> check " << userId << " doing : " << _parameters["DOING"] << " statusShifted : " << _user->statusOriginal << " same : " << same << " type : " << _type;
	switch(_type){
		case AllAvatarsToBuddies:
			//通知本人的其他副本给其好友
			_notifyOtherAvatarsToBuddies();
			os << " 通知本人的其他副本给其好友  ";
		case AvatarToAll:
			//通知本人当前副本给其好友
			_notifyAvatarToBuddies();
			os << " 通知本人当前副本给其好友 ";
		case AvatarToItsOtherAvatars:
			//通知本人当前副本给本人的其他副本
			_notifyItsOtherAvatars();
			os << " 通知本人当前副本给本人的其他副本 ";
		default:
			break;
	}
	//MCE_INFO(os.str());
	//MCE_INFO("PresenceNotifyTask::handle --> userId : "<< userId <<  "deliver size : " << _mSeq.size() << " notify type : " << _type);
	try{
		TalkDeliverAdapter::instance().deliver(_mSeq);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PresenceNotifyTask::handle-->TalkDeliverAdapter::deliver-->" << e);
	}
	//MCE_INFO("PresenceNotifyTask::handle--> Taskchannel = " << _activeAvatars.at(0)->jid->userId % 13 << " cost_time = " << ts.getTime());
	
	
	//计算出自己的在线状态
	if(OFFLINE != _activeAvatars.at(0)->stat){
		_selfOtherAvatars.insert(_selfOtherAvatars.begin(), _activeAvatars.begin(), _activeAvatars.end());	
	}else{
		for(AvatarSeq::iterator ait = _selfOtherAvatars.begin(); ait != _selfOtherAvatars.end(); ++ait){
			if((*ait)->jid->index == _activeAvatars.at(0)->jid->index){
				_selfOtherAvatars.erase(ait);
				break;
			}
		}
	}
	OnlineStat onlineStat = _activeAvatars.at(0)->stat;
	int activetype = 0;
	if(ONLINE == onlineStat || OFFLINE == onlineStat || INVISIABLE == onlineStat){//如果是上下线的消息 获取actor的在线状态
		OnlineStat activestat = ONLINE;
		for(AvatarSeq::iterator ait = _selfOtherAvatars.begin(); ait != _selfOtherAvatars.end(); ++ait){
			if ((*(ait))->stat < activestat) {
				activestat = (*(ait))->stat;
			}
			
			if((*(ait))->jid->endpoint[0] == 'W'){
				activetype |= 2;
			}
			else if((*(ait))->jid->endpoint[0] == 'T'){
				activetype |= 4;
			}
			else if((*(ait))->jid->endpoint[0] == 'P'){
				activetype |= 8;
			}
			//MCE_INFO("PresenceNotifyTask::handle--> actor:" << userId << " activejid" << jidToString((*(ait))->jid) << " activetype:" << activetype);
		}
		//给 手机 新的接入点 推送上下线消息 获得 新版手机在线的好友的ID
		JidSeqMap buddyjids;
		try{
			buddyjids = OnlineCenterAdapter::instance().getBuddyJids(userId, 8);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceNotifyTask::handle-->OnlineCenterAdapter.getBuddyJids-->error:" << e);
		}
		if(buddyjids.empty()){
			return;
		}
		MCE_INFO("PresenceNotifyTask::handle-->actor:" << userId << " buddy.size:" << buddyjids.size());
		IntSeq buddyStormIds;
		for(JidSeqMap::iterator mit = buddyjids.begin(); mit != buddyjids.end(); ++mit){
			//MCE_INFO("PresenceNotifyTask::handle-->actor:" << userId << " buddyjids.size:" << mit->second.size());
			for(JidSeq::iterator sit = mit->second.begin(); sit != mit->second.end(); ++sit){
				//MCE_INFO("PresenceNotifyTask::handle--> jid:" << jidToString((*sit)));
				if(!((*sit)->endpoint.find("PIMStorm"))){
					buddyStormIds.push_back((*sit)->userId);
				}
			}
		}
		if(buddyStormIds.empty()){ //若没有新的接入点的好友在线，没有必要推送
			return;
		}
		try{
			if(INVISIABLE == activestat){
				activestat = OFFLINE;
			}
			if(OFFLINE == activestat){
				activetype = 0;
			}
			IMStormAdapter::instance().UserOnlineStatChange(userId, activestat, activetype, buddyStormIds);
		}catch(Ice::Exception& e){
			MCE_WARN("PresenceNotifyTask::handle-->IMStormAdapter::UserOnlineStatChange-->error:" << e);
		}


		ostringstream os;
		for(AvatarSeq::iterator ait = _activeAvatars.begin(); ait != _activeAvatars.end(); ++ait){
			os << jidToString((*ait)->jid) << "-" << (*ait)->stat << ",";
		}os << " | ";
		for(AvatarSeq::iterator ait = _selfOtherAvatars.begin(); ait != _selfOtherAvatars.end(); ++ait){
			os << jidToString((*ait)->jid) << "-" << (*ait)->stat << ",";
		}
		MCE_INFO("PresenceNotifyTask::handle-->uid:" << _activeAvatars.at(0)->jid->userId << " os:" << os.str() << " activestat:" << activestat << " activetype:" << activetype);
	}
	
}

void BatchOfflineNotifyTask::handle() {
	StatFunc statF("BatchOfflineNotifyTask::handle");
	MessageSeq res;
	for (size_t i = 0; i < _fromSeq.size(); ++i) {

		JidSeqMap onlineMap;
		try {
			onlineMap = OnlineCenterAdapter::instance().getBuddyJidsAndSelf(_fromSeq.at(i)->userId, 4, 4);
		} catch(Ice::Exception& e) {
			MCE_WARN("BatchOfflineNotifyTask::handle-->OnlineCenterAdapter::getBuddyJidsAndSelf-->"<< e);
		} catch(...) {
			MCE_WARN("BatchOfflineNotifyTask::handle --> get online buddies jidseqmap err");
		}
		/*JidSeq self;
		try {
			self = OnlineCenterAdapter::instance().getUserJids(_fromSeq.at(i)->userId);

		} catch(Ice::Exception& e) {
			MCE_WARN("BatchOfflineNotifyTask::handle --> get user jidseq err :"<<e);
		} catch(...) {
			MCE_WARN("BatchOfflineNotifyTask::handle --> get user jidseq err");
		}*/
		JidSeqMap::iterator it = onlineMap.find(_fromSeq.at(i)->userId);
		if(it!=onlineMap.end()){
			for(JidSeq::iterator jIt = it->second.begin(); jIt != it->second.end(); ++jIt){
				if((*jIt)->index == _fromSeq.at(i)->index){
					it->second.erase(jIt);
					break;
				}
			}
			if(it->second.empty()){
				onlineMap.erase(it);
			}
		}
		/*for (JidSeq::iterator it = self.begin(); it != self.end();) {
			if ((*it)->index == _fromSeq.at(i)->index || (*it)->index <=1) {
				it = self.erase(it);
				if (self.empty()) {
					break;
				}

			} else {
				++it;
			}
		}
		if (!self.empty()) {
			onlineMap[_fromSeq.at(i)->userId] = self;
		}*/

		for (JidSeqMap::iterator it = onlineMap.begin(); it != onlineMap.end(); ++it) {
			for (size_t j = 0; j < it->second.size(); ++j) {

				if (it->second.at(j)->index <=1) {
					continue;
				}

				MessagePtr mPtr = new Message();
				mPtr -> from = _fromSeq.at(i);
				mPtr -> to = it->second.at(j);
				mPtr -> type = PRESENCE_OFFLINE;

				ostringstream msg;
				msg << "<presence type='unavailable' " << " from='"
						<<jidToString(_fromSeq.at(i))<<"' " << " to='"
						<<jidToString(it->second.at(j))<<"' " <<" />";

				mPtr->msg = msg.str();

				res.push_back(mPtr);
			}
		}
	}

	MCE_DEBUG("call OfflineNotifyHandler::handle  offline message size:"
			<<res.size());

	if (!res.empty()) {
		try{
			TalkDeliverAdapter::instance().deliver(res);
		}catch (Ice::Exception& e){
			MCE_WARN("BatchOfflineNotifyTask::handle-->TalkDeliverAdapter::deliver-->err" << e);
		}
	} 
}

void ExitAllRoomTask::handle() {
	StatFunc statF(__FUNCTION__);
	MessageSeq res;
	MCE_DEBUG("ExitAllRoomTask::handle --> _fromSeq.size:"<<_fromSeq.size());
	for (size_t i = 0; i < _fromSeq.size(); ++i) {
		RoomIdSeq roomIds;
		try{

//	ConnectionPoolManager::instance().initialize(ServiceI::instance().getAdapter());
		roomIds = UserRoomAdapter::instance().getRoomIds(_fromSeq.at(i)->userId, "");
		}
		catch(Ice::Exception& e){
			MCE_WARN("ExitAllRoomTask::handle-->UserRoomAdapter::getRoomIds-->" << e);
			}
		MCE_DEBUG("ExitAllRoomTask::handle --> rid.size:"<<roomIds.size());
		for(size_t j = 0; j < roomIds.size(); ++j){
			OccupantPtr occupant;
			try{
			occupant = OccupantsAdapter::instance().exitRoom(roomIds.at(j), _fromSeq.at(i));
			}catch(Ice::Exception& e){
				MCE_WARN("ExitAllRoomTask::handle-->OccupantsAdapter::exitRoom-->" << e);
				}
			if(!occupant){
				MCE_DEBUG(__FUNCTION__ << " --> invoke Occupants::exitRoom error, " << roomIdToString(roomIds.at(j)));
				continue;
			}

			string strRoomId = roomIdToString(roomIds.at(j));
			MessageSeq mseq;
			ostringstream os;
			os << "<presence from='"<< roomJidToString(occupant->rJid) <<"' to='"<<jidToString(_fromSeq.at(i))<<"' type='unavailable'>"
			 << "<status>offline</status>"
			 << "<x xmlns='http://jabber.org/protocol/muc#user'>"
			 << "<item affiliation='none' role='none'/>"
			 << "</x>"
			 << "</presence>";

			MessagePtr msg = new Message();
			msg->from = _fromSeq.at(i);
			msg->to = _fromSeq.at(i);
			msg->type = PRESENCE_COMMON;
			msg->msg = os.str();
			mseq.push_back(msg);

			MCE_DEBUG(__FUNCTION__ << " --> invoke Occupants::getOccupants --> begin" );
			OccupantSeq oseq;
			try{
			oseq = OccupantsAdapter::instance().getOccupants(roomIds.at(j));
			}
			catch(Ice::Exception& e){
				MCE_WARN("ExitAllRoomTask::handle-->OccupantsAdapter::getOccupants-->" << e);
				}
			MCE_DEBUG(__FUNCTION__ << " --> invoke Occupants::getOccupants --> " << oseq.size());

			for(size_t k = 0; k < oseq.size(); ++k) {
				ostringstream os;
				os << "<presence from='"<< roomJidToString(occupant->rJid) <<"' to='"<<jidToString(oseq.at(k)->fJid)<<"' type='unavailable'>"
				 << "<status>offline</status>"
				 << "<x xmlns='http://jabber.org/protocol/muc#user'>"
				 << "<item affiliation='none' role='none'/>"
				 << "</x>"
				 << "</presence>";

				MessagePtr msg = new Message();
				msg->from = _fromSeq.at(i);
				msg->to = oseq.at(k)->fJid;
				msg->msg = os.str();
				msg->type = PRESENCE_COMMON;

				mseq.push_back(msg);
//				MCE_DEBUG(__FUNCTION__ << " --> msg:"<< msg->msg);
			}

			MCE_DEBUG(__FUNCTION__ << " --> deliver size=" << mseq.size());
			try{
			MucDeliverAdapter::instance().deliver(mseq);
			}
			catch(Ice::Exception& e){
				MCE_WARN("ExitAllRoomTask::handle-->MucDeliverAdapter::deliver-->" << e);
				}
		}
	}
}

