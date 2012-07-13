#include "StatManagerI.h"
#include "IceLogger.h"
//#include "OnlineBuddyAdapter.h"
#include "QueryRunner.h"
#include "XmppTools.h"
//#include "XFeedDispatcherAdapter.h"
#include "OnlineCenterAdapter.h"
#include <boost/lexical_cast.hpp>
#include "RestLogicAdapter.h"
#include "OccupantsAdapter.h"
#include "WTalkProxyAdapter.h"
#include "PTalkProxyAdapter.h"

#include "RFeed.h"
#include "FeedAssistantAdapter.h"

using namespace talk::stat;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::talk::util;
//using namespace xce::xfeed;
using namespace talk::online::adapter;
using namespace talk::rest;
using namespace talk::occupants::adapter;
using namespace talk::adapter;
using namespace xce::feed;
using namespace com::xiaonei::talk::cache;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&StatManagerI::instance(),
			service.createIdentity("M", ""));
	TaskManager::instance().schedule(new ControlTimer());
}

void StatManagerI::login(const MyUtil::IntSeq& ids, const Ice::Current&) {
	MCE_DEBUG("StatManagerI::login --> ids.size:" << ids.size());
	for (size_t i = 0; i < ids.size(); ++i) {
		bool succ = addLoginUser(ids.at(i));
		/*if (succ) {
		 addNotifyId(ids.at(i));

		 //addNotifyId(128487631);
		 }*/
		StatPtr st = new Stat;
		st->loginCount = 1;
		setStat(ids.at(i), st);
		//		if(ids.at(i) == 200865373){
		//			RestMediatorAdapter::instance().useImNotify(ids.at(i));
		//		}
	}
}

void StatManagerI::wLogin(const MyUtil::IntSeq& ids, const Ice::Current&) {
	for (size_t i = 0; i < ids.size(); ++i) {
		addWloginUser(ids.at(i));
	}

}

void StatManagerI::totalMsgCount(Ice::Int count, const Ice::Current&) {
	addMsgCount(count);
}
void StatManagerI::chatingMsgCount(Ice::Int count, const Ice::Current&) {
	addChatingMsgCount(count);
}

void StatManagerI::versionStat(int userId, const string& ver,
		const string& subver, const Ice::Current&) {
	addVersionDistribution(userId, ver, subver);
	//float version = 0.0;
	//try{
	//	version = boost::lexical_cast<float>(ver);
	//}catch(...){}
	//if(version >= 3.521){
	addNotifyId(userId, ver, subver);
	//}
}

void StatManagerI::mucMsgCount(int msgCount, int msgBroadcastCount,
		const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	_mucMsgCount += msgCount;
	_mucMsgBroadcastCount += msgBroadcastCount;
}

void StatManagerI::msgStat(const JidPtr& from, const JidPtr& to,
		const Ice::Current&) {

	MCE_INFO("STAT_MSG:  FROM:" << from->userId << "  ENDPOINT:"
			<< from->endpoint << "  -->   TO:" << to->userId << "  ENDPOINT:"
			<< to->endpoint);

	StatPtr st = new Stat;
	st->msgCount = 1;
	setStat(from->userId, st);

	st = new Stat;
	st->revMsgCount = 1;
	setStat(to->userId, st);

	addMsgFromAll(1);
	if (!from->endpoint.empty()) {
		if (from->endpoint[0] == 'W') {
			addMsgFromPager(1);
		} else if (from->endpoint[0] == 'T') {
			addMsgFromClient(1);
		} else if (from->endpoint[0] == 'P') {
			addMsgFromPhone(1);
		}
	}

}

void StatManagerI::incSetStatusCount(int c, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	_setStatusCount += c;
}

void StatManagerI::incReplyStatusCount(int c, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	_replyStatusCount += c;
}

void StatManagerI::incReplyPhotoCount(int c, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	_replyPhotoCount += c;
}
void StatManagerI::incReplyBlogCount(int c, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	_replyBlogCount += c;
}
void StatManagerI::incReplyShareCount(int c, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(_mutex);
	_replyShareCount += c;
}
//-------------------------------

void UserStatSaveTask::handle() {
	StatMap stmap;
	StatManagerI::instance().popStatMap(stmap);
	if (stmap.empty()) {
		return;
	}

	Statement sql;
	string
			head =
					"insert into user_stat (userid,login_count,send_msg_count,rev_msg_count,online_buddy_count) values";
	int count = 0;
	for (StatMap::iterator it = stmap.begin(); it != stmap.end(); ++it) {
		count++;
		if (count == 1) {
			sql.clear();
			sql << head;
		}
		int onlineBuddy = 0;
		try {
			onlineBuddy = OnlineCenterAdapter::instance().getBuddyCount(
					it->first);
		} catch (Ice::Exception & e) {
			MCE_WARN("UserStatSaveTask::handle --> get OnlineBuddyCount err :"
					<< e);
		}
		if (count > 1) {
			sql << ",";
		}
		sql << "(" << it->first << "," << it->second->loginCount << ","
				<< it->second->msgCount << "," << it->second->revMsgCount
				<< "," << onlineBuddy << ")";
		if (count == 5000) {
			try {
				QueryRunner("im_stat", CDbWServer).schedule(sql);
			} catch (...) {
				MCE_WARN("UserStatSaveTask::handle --> do sql err");
			}
			sql.clear();
			count = 0;
		}
	}
	if (!sql.empty()) {
		try {
			QueryRunner("im_stat", CDbWServer).schedule(sql);
		} catch (...) {
			MCE_WARN("UserStatSaveTask::handle --> do sql err2");
		}
	}

}

void MinSaveTask::handle() {
	map<string, int> data;
	data["total_msg_count"] = StatManagerI::instance().getMsgCount();
	data["chating_msg_count"] = StatManagerI::instance().getChatingMsgCount();
	data["offline_msg_count"] = data["total_msg_count"]
			- data["chating_msg_count"];

	data["from_client_msg_count"] = StatManagerI::instance().getMsgFromClient();
	data["from_pager_msg_count"] = StatManagerI::instance().getMsgFromPager();
	data["from_phone_msg_count"] = StatManagerI::instance().getMsgFromPhone();
	data["from_all_msg_count"] = StatManagerI::instance().getMsgFromAll();

	data["set_status_count"] = StatManagerI::instance().getSetStatusCount();
	data["reply_status_count"] = StatManagerI::instance().getReplyStatusCount();
	data["reply_photo_count"] = StatManagerI::instance().getReplyPhotoCount();
	data["reply_blog_count"] = StatManagerI::instance().getReplyBlogCount();
	data["reply_share_count"] = StatManagerI::instance().getReplyShareCount();

	ostringstream os;
	Statement sql;
	os << "insert into xntalk_snap (name, time,value) values";
	for (map<string, int>::iterator it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin())
			os << ",";

		os << "('" << it->first << "',from_unixtime(" << time(NULL) << "),"
				<< it->second << ")";
	}
	sql << os.str();
	MCE_INFO("MinSaveTask::handle --> sql = " << os.str());
	QueryRunner("im_stat", CDbWServer).schedule(sql);

}
void VersionDistributionTask::handle() {

	vector<VersionStatData> res;
	StatManagerI::instance().popVersionDistribution(res);
	if (res.empty()) {
		return;
	}
	int count = 0;
	Statement sql;
	//sql << "insert into xntalk_version_dist(version,userid,time) values ";
	for (vector<VersionStatData>::iterator it = res.begin(); it != res.end(); ++it) {
		if (count == 0) {
			sql.clear();
			sql
					<< "insert into xntalk_version_dist(version,userid,time) values ";
		} else {
			sql << ",";
		}
		++count;
		sql << "('" << (*it)._version;
		if (!(*it)._subver.empty()) {
			sql << "_" << (*it)._subver;
		}
		sql << "'," << (*it)._userId << ", '" << (*it)._date << "')";
		if (count % 1000 == 0) {
			QueryRunner("im_stat", CDbWServer).schedule(sql);
			count = 0;
		}

	}
	if (count) {
		QueryRunner("im_stat", CDbWServer).schedule(sql);
	}

}
void NewUserNotifyTask::handle() {

	map<int, pair<string, string> > ids;
	StatManagerI::instance().popNotifyIds(ids);
	if (ids.empty()) {
		return;
	}
	//	map<int, string> updateIds = ids;
	MCE_DEBUG("NewUserNotifyTask::handle --> ids.size = " << ids.size());

	Statement sql;
	sql << "select userid from first_user where userid in (";
	for (map<int, pair<string, string> >::iterator it = ids.begin(); it
			!= ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << it->first;
	}
	sql << ")";
#ifndef NEWARCH
	mysqlpp::Result res = QueryRunner("im_stat", CDbRServer).store(sql);
#else
	mysqlpp::StoreQueryResult res = QueryRunner("im_stat", CDbRServer).store(sql);
#endif

#ifndef NEWARCH
	for (size_t i = 0; i < res.size(); ++i) {
#else
		for (size_t i= 0; i<res.num_rows(); ++i) {
#endif
		mysqlpp::Row row = res.at(i);
		ids.erase((int) row["userid"]);
	}
	if (ids.empty()) {
		return;
	}

	sql.clear();
	sql << "insert into first_user (userid,time,version, subver) values";
	for (map<int, pair<string, string> >::iterator it = ids.begin(); it
			!= ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << "(" << it->first << ",from_unixtime(" << time(NULL) << "), '"
				<< it->second.first << "', '" << it->second.second << "')";
	}
	sql
			<< " on duplicate key update version=values(version), subver=values(subver)";
	try {
		QueryRunner("im_stat", CDbWServer).schedule(sql);
	} catch (...) {
		MCE_WARN("NewUserNotifyTask::handle --> do insert db err");
	}
	for (map<int, pair<string, string> >::iterator it = ids.begin(); it
			!= ids.end(); ++it) {
		try {
		/*	FeedSeedPtr seed = new FeedSeed();
			seed->feed = 0;
			seed->miniMerge = 0;
			seed->newsMerge = 0;
			seed->source = it->first;
			seed->psource = it->first;
			seed->actor = it->first;
			seed->type = 0;
			seed->type |= 0x50000000;
			seed->type |= 1001;
			seed->ptype = seed->type;
			seed->time = time(NULL);
			seed->baseWeight = 0;
			ostringstream os;
			os << "f("<<it->first<<")";
//			os << "u(200865373)";
			seed->extraProps["expr"] = os.str();
			seed->extraProps["publish_mini"] = "0";
			seed->extraProps["publish_news"] = "1";
			seed->extraProps["send_config"] = "0";
			seed->extraProps["update_time"] = "0";
			seed->extraProps["base_weight_id"] = "8";
			
			//MyUtil::IntSeq uids;
			//uids.push_back(it->first);
			//std::map<int, UserCache> uc =
			//		UserCacheAdapter::instance().GetUserCache(uids);
			
			TalkUserPtr u = TalkCacheClient::instance().GetUserByIdWithLoad(it->first);
			if (u) {


				static string title[3] = {"拥有好多人人网玩不到的超级功能啦！",
						"不开网页就能回复你的状态！",
						"能使用定时功能来摘你的菜啦！"};
				static string body[3] = {"一键登录，快速切换帐号玩应用，挂人人网更方便！同时支持好友聊天，定时启动应用游戏，你还在等什么？快来下载吧！",
						"挂人人桌面，不用开网页，你就可以查看、回复所有新鲜事，直接与好友聊天，同时登录多款应用游戏！",
						"上班太忙，又忘了收菜？挂人人桌面，便可设置应用定时启动，补充体力、喂养宠物，一个也不会忘记！"};

				static string deskUrl[3] = {"http://im.renren.com/desktop5.html?firstfeed01",
						"http://im.renren.com/desktop5.html?firstfast01",
						"http://im.renren.com/desktop5.html?firststolen01"
				};
				static string downloadUrl[3] = {
						"http://im.renren.com/desktop/ver26/rrsetup.exe?firstfeed=dl01",
						"http://im.renren.com/desktop/ver26/rrsetup.exe?firstfast=dl01",
						"http://im.renren.com/desktop/ver26/rrsetup.exe?firststolen=dl01"
				};
				static int i = 0;
				++i;
				i %= 3;

				ostringstream xml;
				xml << "<f v=\"2.0\" type=\"imUse\">" << "<time>" << time(NULL)*1000
						<< "</time>" << "<type>1001</type>" << "<from>"
						<< "<id>" << it -> first << "</id>" << "<name>"
						<< u->name << "</name>"
						<< "<tinyimg>" << u->tinyurl
						<< "</tinyimg>" << "</from>" << "<im>"
						<< "<title>刚刚安装了&lt;a href=&quot;"<<deskUrl[i]<<"&quot;&gt;人人桌面&lt;/a&gt;，"<<title[i]<<"</title>"
						<< "<body>"<<body[i]<<"</body>"
						<< "<url>"<<downloadUrl[i]<<"</url>"
						<< "</im>" << "</f>";
				seed->xml = xml.str();

				FeedAssistantAdapter::instance().dispatch(seed);
			}*/
			MCE_INFO("NewUserNotifyTask::handle --> new user " << it->first);
			RestLogicAdapter::instance().useImNotify(it->first);
		} catch (Ice::Exception& e) {
			MCE_WARN("NewUserNotifyTask::handle --> err:" << e);
		} catch (...) {
			MCE_WARN("NewUserNotifyTask::handle --> err");
		}
	}
}

void MucStatTask::handle() {

	map<string, int> data;
	data["muc_msg_count"] = StatManagerI::instance().getMucMsgCount();
	data["muc_broadcast_msg_count"]
			= StatManagerI::instance().getMucMsgBroadcastCount();
	data["muc_room_count"] = OccupantsAdapter::instance().getRoomCount();
	data["muc_user_count"] = OccupantsAdapter::instance().getUserCount();

	Statement sql;
	sql << "insert into xntalk_snap(name, time, value) values";
	for (map<string, int>::iterator it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin())
			sql << ",";
		sql << "('" << it->first << "',from_unixtime(" << time(NULL) << "),"
				<< it->second << ")";
	}

	QueryRunner("im_stat", CDbWServer).schedule(sql);
}

void OnlineUserCountTask::handle() {
	map<string, int> data;
	OnlineCountPtr count = WTalkProxyAdapter::instance().getOnlineCount();
	OnlineCountPtr phoneCount = PTalkProxyAdapter::instance().getOnlineCount();
	data["pager_onlineuser_count"] = count->user;
	data["pager_onlineavatar_count"] = count->avatar;
	data["phone_onlineuser_count"] = phoneCount->user;
	data["phone_onlineavatar_count"] = phoneCount->avatar;

	Statement sql;
	sql << "insert into xntalk_snap(name, time, value) values";
	for (map<string, int>::iterator it = data.begin(); it != data.end(); ++it) {
		if (it != data.begin())
			sql << ",";
		sql << "('" << it->first << "',from_unixtime(" << time(NULL) << "),"
				<< it->second << ")";
	}
	QueryRunner("im_stat", CDbWServer).schedule(sql);
}

