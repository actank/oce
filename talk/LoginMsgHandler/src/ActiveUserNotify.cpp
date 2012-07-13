#include <time.h>
#include <stdlib.h>
#include "ActiveUserNotify.h"
#include "RestLogicAdapter.h"
#include "WTransitAdapter.h"
#include "QueryRunner.h"
#include "FeedAssistantAdapter.h"
#include "LoginMsgHandlerAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"

using namespace xce::talk;

using namespace talk::rest;
using namespace com::xiaonei::xce;
using namespace talk::adapter;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::talk;
using namespace xce::feed;

void ActiveUserNotify::notify(int userid) {
	{
		try {
			MCE_INFO("ActiveUserNotify::handle --> notify " << userid
					<< " count:" << _notifyCount << " " << _count << " size:" << _users.size());
			srand(time(NULL));
			if(_notifyCount < MAX_NOTIFY){
				MCE_INFO("ActiveUerNotify::notify --> send notify " << userid);
				RestLogicAdapter::instance().activeUserNotify(userid);
				_count++;
			}
		} catch (Ice::Exception& e) {
			MCE_WARN("ActiveUerNotify::notify --> invoke RestLogic err, " << e);
		}

		try {
			MCE_INFO("ActiveUserNotify::handle --> send notice " << userid);

			WTransitAdapter::instance().deliverAppNotice(
					userid,
					3205,
					"人人桌面",
					"http://app.xnimg.cn/application/20100225/17/55/Lb7Sl5d018153.gif",
					"<br/>您已额外获得一次今天的登录积分！<br/><br/>友情提醒：如果您每天都上人人网，也请帮您的同学、好友、家人注册一个人人网帐号吧！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击这里注册】</a>");
		} catch (Ice::Exception& e) {
			MCE_WARN("ActiveUerNotify::notify --> invoke RestLogic err, " << e);
		}

		try {

//			if(userid != 255414584 && userid != 234159611  && userid != 255329305 && userid!= 260190016 ) return;
			static int n = 0;
			if(!(n = ++n % 3 )){
				return;
			}
			MCE_INFO("Send NewsFeed TO: f("<<userid << ")");		
			
			FeedSeedPtr seed = new FeedSeed();
			seed->feed = 0;
			seed->miniMerge = 0;
			seed->newsMerge = 0;
			seed->source = userid;
			seed->psource = userid;
			seed->actor = userid;
			seed->type = 0;
			seed->type |= 0x50000000;
			seed->type |= 1001;
			seed->ptype = seed->type;
			seed->time = time(NULL);
			seed->baseWeight = 0;
			ostringstream os;
			os << "f(" << userid << ")";
//						os << "u(200865373)";
			seed->extraProps["expr"] = os.str();
			seed->extraProps["publish_mini"] = "0";
			seed->extraProps["publish_news"] = "1";
			seed->extraProps["send_config"] = "0";
			seed->extraProps["update_time"] = "0";
			seed->extraProps["base_weight_id"] = "8";
			TalkUserPtr u;
			try{
				u = TalkCacheClient::instance().GetUserByIdWithLoad(userid);
			}catch(Ice::Exception& e){
				MCE_WARN("call TalkCache.GetUserByIdWithLoad err:"<<e << ", line:"<<__LINE__);
			}
			if (u) {

				static string title[1] = { "额外获得了一次登录积分！"};
				static string
						body[1] = {
								"用人人桌面，抢先回复好友新鲜事，帮你更快赚取积分！定时功能提醒收菜，上班娱乐两不误！" };

				static string deskUrl[1] = {
						"http://im.renren.com/desktop5.html?double" };
				static string
						downloadUrl[1] = {
								"http://im.renren.com/desktop/ver26/rrsetup.exe?double" };
				static int i = 0;
//				++i;
//				i %= 3;

				ostringstream xml;
				xml << "<f v=\"2.0\" type=\"imUse\">" << "<time>" << time(NULL)
						* 1000 << "</time>" << "<type>1001</type>" << "<from>"
						<< "<id>" << userid << "</id>" << "<name>"
						<< u->name << "</name>" << "<tinyimg>" << u->tinyurl
						<< "</tinyimg>" << "</from>" << "<im>"
						<< "<title>刚刚登录&lt;a href=&quot;" << deskUrl[i]
						<< "&quot;&gt;人人桌面&lt;/a&gt;，" << title[i]
						<< "</title>" << "<body>" << body[i] << "</body>"
						<< "<url>" << downloadUrl[i] << "</url>" << "</im>"
						<< "</f>";
				seed->xml = xml.str();

				MCE_INFO("ActiveUserNotify::handle --> send feed " << userid);
				FeedAssistantAdapter::instance().dispatch(seed);
			}

		} catch (Ice::Exception& e) {
			MCE_WARN("ActiveUserNotify::notify --> dispatch feed err:" << e);
		}

	}
}

void ActiveUserNotify::needNotify(int userid) {
	if(!_inited){
		return;
	}
	bool insert = false;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		if (_users.find(userid) == _users.end()) {
			_users.insert(userid);
			_uids.push_back(make_pair<int, int> (userid, time(NULL)));
			insert = true;
		}
	}
	if(insert){
		Statement sql;
		sql << "insert into activeuser_per_day(userid) values(" << userid << ")";
		QueryRunner("im_stat", CDbWServer).schedule(sql);
	}
}

void ActiveUserNotify::handle() {
	//load login users last week
	if(!_inited){
		loadUsers();
		_inited = true;
	}
	updateSendNotifyCount();		
	int tmp = _day;
	setDay();
	MCE_INFO("ActiveUserNotify::handle --> day " << _day);
	if (tmp != _day) {
		Statement sql;
		sql << "truncate table activeuser_per_day";
		QueryRunner("im_stat", CDbWServer).schedule(sql);
		
		sql.clear();
		sql << "truncate table send_notify_count";
		QueryRunner("im_stat", CDbWServer).schedule(sql);
		
		IceUtil::Mutex::Lock lock(_mutex);
		_users.clear();
		_count = 0;
		MCE_INFO("ActiveUserNotify::handle --> another day " << _day);
	} 
	MCE_INFO("ActiveUserNotify::handle --> notify size " << _uids.size());
	while (true) {
		IceUtil::Mutex::Lock lock(_mutex);
		if (_uids.empty()) {
			break;
		}
		pair<int, int> p = _uids.front();
		if ((time(NULL) - p.second) < 60) {
			break;
		}
		_uids.pop_front();
		notify(p.first);
	}

}

void ActiveUserNotify::loadUsers() {
	Statement sql;
	sql << "select * from activeuser_per_day";
	mysqlpp::StoreQueryResult res = QueryRunner("im_stat", CDbRServer).store(
			sql);
	if (!res) {
		return;
	}
	IceUtil::Mutex::Lock lock(_mutex);
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row row = res.at(i);
		int userid = (int) row["userid"];
		_users.insert(userid);
	}
	MCE_INFO("ActiveUserNotify::loadUsers --> loaded " << _users.size());
}

void ActiveUserNotify::updateSendNotifyCount(){
	Statement sql;
	sql << "insert into send_notify_count(name, count) values('send_notify_count', " << _count << ") on duplicate key update count=count+values(count)";
	QueryRunner("im_stat", CDbWServer).store(sql);
	_count = 0;

	sql.clear();
	sql << "select count from send_notify_count where name='send_notify_count' limit 1";
	mysqlpp::StoreQueryResult res = QueryRunner("im_stat", CDbRServer).store(sql);
	if (res && res.num_rows() > 0) {
		mysqlpp::Row row = res.at(0);
		_notifyCount = (int)row["count"];
	}
	MCE_INFO("ActiveUserNotify::loadSendNotifyCount --> " << _notifyCount);
}

void ActiveUserNotify::setDay() {
	time_t tmp = time(NULL);
	tm* t = localtime(&tmp);
	_day = t->tm_wday;
}
