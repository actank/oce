#include <time.h>
#include <iostream>
#include <stdlib.h>
#include "ActiveUserNotify.h"
#include "RestLogicAdapter.h"
#include "QueryRunner.h"
#include "FeedAssistantAdapter.h"
#include "LoginMsgHandlerAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include "IMWindowAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"
#include <boost/lexical_cast.hpp>
#include "util/cpp/MD5Util.h"
using namespace xce::talk;
using namespace talk::window;
using namespace talk::rest;
using namespace com::xiaonei::xce;
using namespace talk::adapter;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::talk;
using namespace xce::feed;
using namespace std;
using namespace MyUtil;

void ActiveUserNotify::SetNotifyData(const string& notifydata){
	IceUtil::Mutex::Lock lock(_mutex);
	_notifydata = notifydata;
}
  
void ActiveUserNotify::setIndex(int index) {
  _index = index;
}


void ActiveUserNotify::notify(int userid, float ver) {
	bool callrest = false;
	string notifydata = "";
	/*
	//--------------------------------------
	string act_key = "IEm*2jnxdl" + boost::lexical_cast<string>(userid);
	ostringstream res;
	unsigned char* md5 = MD5((unsigned char*)act_key.c_str(), act_key.size(), NULL);
	for(size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		res << hex << setw(2) << setfill('0') << (unsigned int)md5[i];
	}
	string act_para = res.str();
	MCE_INFO("ActiveUserNotify::notify --> userid:" << userid << " act_para:" << act_para);
	//--------------------------------------
	*/

//	MCE_DEBUG("ActiveUserNotify::handle --> notify " << userid << " ver:" << ver << " count:" << _notifyCount << " " << _count << " size:" << _users.size());
	if (ver < 4) {
		{
			IceUtil::Mutex::Lock lock(_mutex);
			notifydata = _notifydata;
			if (_notifyCount < MAX_NOTIFY) {
				callrest = true;
				_count++;
			}
		}
		try {
			srand(time(NULL));
			if (callrest) {
				MCE_INFO("ActiveUerNotify::notify --> send notify " << userid);
				//RestLogicAdapter::instance().activeUserNotify(userid);
			}
		} catch (Ice::Exception& e) {
			MCE_WARN("ActiveUerNotify::notify --> invoke RestLogic err, " << e);
		}

		try {
			MCE_INFO("ActiveUserNotify::handle --> send notice " << userid <<" ver:"<<ver);
			MyUtil::Date active_end("2010-11-01 01:00:00");
			time_t timestamp = active_end.time() - time(NULL);
			int active_day = 0, active_hour = 0;
			if(timestamp > 0){
				active_day = timestamp / (24 * 60 * 60);
				active_hour = (timestamp % (24 * 60 * 60)) / (60 * 60);
			}
			if(timestamp <= 0 || (active_day == 0 && active_hour == 0)){
				MessageMsgHandlerAdapter::instance().deliverAppNotice(
						userid,
						3205,
						"人人桌面",
						"http://app.xnimg.cn/application/20100225/17/55/Lb7Sl5d018153.gif",
						notifydata
						);
			}
			else{
				ostringstream os;
				os << "距离积分回馈活动截止还剩："<< active_day << "天" << active_hour <<"小时";
				MessageMsgHandlerAdapter::instance().deliverAppNotice(
						userid,
						3205,
						"人人桌面",
						"http://app.xnimg.cn/application/20100225/17/55/Lb7Sl5d018153.gif",
						os.str() +"<br/><br/>  即日起至10月31日，登录人人桌面             * 连续登录1-6天，登录积分翻三倍              * 连续登录7天以上，登录积分翻五倍             ______________________________________               如果您每天都上人人网，也帮您的亲朋好友        注册一个人人网帐号吧！&nbsp;<a href=\"wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击这里注册】</a>");
			}
		}
		catch (Ice::Exception& e) {
			MCE_WARN("ActiveUerNotify::notify-->MessageMsgHandlerAdapter::deliverAppNotice-->" << e);
		}
	}
	else {
		{
			IceUtil::Mutex::Lock lock(_mutex);
			notifydata = _notifydata;
			if (_notifyCount < MAX_NOTIFY) {
				callrest = true;
				_count++;
			}
		}
		try {
			srand(time(NULL));
			if (callrest) {
				MCE_INFO("ActiveUerNotify::notify --> call activeUserNotify2 " << userid);
				//RestLogicAdapter::instance().activeUserNotify2(userid);
			}
		} catch (Ice::Exception& e) {
			MCE_WARN("ActiveUerNotify::notify --> invoke RestLogic err, " << e);
		}

		try {
			MCE_INFO("ActiveUserNotify::handle --> send notice** " << userid <<" ver:"<<ver);

			MyUtil::Date active_end("2010-11-01 01:00:00");
			time_t timestamp = active_end.time() - time(NULL);
			int active_day = 0, active_hour = 0;
			if(timestamp > 0){
				active_day = timestamp / (24 * 60 * 60);
				active_hour = (timestamp % (24 * 60 * 60)) / (60 * 60);
			}
			MCE_DEBUG("timestamp = " << timestamp << " active_day = " << active_day << " active_hour = " << active_hour);
			if(timestamp <= 0 || (active_day == 0 && active_hour == 0)){
				MessageMsgHandlerAdapter::instance().deliverAppNotice(
						userid,
						3205,
						"人人桌面",
						"http://app.xnimg.cn/application/20100225/17/55/Lb7Sl5d018153.gif",
						//"<br/>您已额外获得一次今天的登录积分！<br/><br/>友情提醒：如果您每天都上人人网，也请帮您的同学、好友、家人注册一个人人网帐号吧！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击这里注册】</a>"
						//"<br/>您已额外获得一次今天的登录积分！<br/><br/>友情提醒：如果您每天都上人人网，也请帮您的同学、好友、家人注册一个人人网帐号吧！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击这里注册】</a>"
						//"<br/><a href=\"http://hui.renren.com/qixi/home?wc=201108005\">【七夕活动】执子之手，将子拖走！</a><br/><br/>再不表白就太晚了，再不相爱就太迟了！<br/>快来抓住TA的手，幸福的向前走吧！<br/>________________________________________<br/><a href=\"http://jisu.renren.com/?subver=25\">人人极速相册</a>1.0火热推出,单次上传500张!"
						notifydata  
						//"<br/><a href=\"http://jisu.renren.com/?subver=25\">【人人极速相册1.0】火热上线！</a><br/> 专享特权：单次上传500张照片！<br/>________________________________________<br/><br/><a href=\"http://hot.renren.com/?subver=5\">【人人热门分享】</a>-人人网全新软件公测<br/> 不开网页，无需登录，热门视频随意看！"
						//"<a href=\"http://im.renren.com/huodong/summer.html?subver=44\">【活动】人人桌面免费送礼券、积分啦！</a><br/>&nbsp;7月登录人人桌面<br/>﹡逢周一、周四送5个礼券！<br/>﹡每天独享登录积分翻3倍！<br/>________________________________________<br/>如果您经常上人人网，也请帮您的亲朋好友也注册一个人人网帐号吧！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点此注册】</a>"
						//"<br/>您已额外获得一次今天的登录积分！&nbsp;<br/>如果您每天都上人人网，也请帮您的好友、家人注册一个人人网帐号吧！&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点此注册】</a>______________________________________ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://huodong.renren.com/child2011\">儿童节捣蛋行动，一起来打小怪兽！ </a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;登录人人桌面，每天免费点亮1次刷新徽章！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://act.renren.com/child/imsend?sign=" + act_para + "\">【点亮徽章】</a>"
						//"<br/>您已额外获得一次今天的登录积分！&nbsp;<br/>如果您经常上人人网，请帮您的亲朋好友也注册一个人人网帐号吧！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点此注册】</a> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;______________________________________ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://page.renren.com/600594626/note/721426187\">【活动】感恩母亲节，一句话送祝福</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;感恩母亲节！为母亲留下祝福，<a href=\"http://page.renren.com/600594626\">2345</a>把祝福化作惊喜送给妈妈！"
						//"<br/>您已额外获得一次今天的登录积分！&nbsp;<br/><br/>如果您经常上人人网，请帮您的亲朋好友也注册一个人人网帐号吧！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点此注册】</a> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;______________________________________ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://invite.renren.com/activities/bailingVip?imnotify\">【礼物】恭喜获得VIP邀请资格！</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;特权功能：隐身访问、去广告、主页装扮"
						//"【活动】登录人人桌面送积分啦！！！<br/>同学们！即日起至4月12日：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* 连续登录1-6天，登录积分翻3倍！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* 连续登录7天及以上，登录积分翻5倍！！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;______________________________________ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://invite.renren.com/activities/bailingVip?imnotify\">【礼物】恭喜获得VIP邀请资格！</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;特权功能：隐身访问、去广告、主页装扮"
						//"<a href=\"http://im.renren.com/huodong/spring.html?subver=44\">【活动】登录人人桌面送积分啦！！！</a><br/>同学们！即日起至4月8日：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* 连续登录1-6天，登录积分翻3倍！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* 连续登录7天及以上，登录积分翻5倍！！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;______________________________________ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://invite.renren.com/activities/bailingVip?imnotify\">【官方礼物】</a>免费VIP大放送，你可获得特权&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://invite.renren.com/activities/bailingVip?imnotify\">免费VIP、隐身访问、去广告、免费主页装扮</a>"
						//"【活动】人人桌面入围年度软件评选！请投出你宝贵的一票吧，我们需要你！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://page.renren.com/600003065/note/500469354\">【查看详情】</a><br/>----------------------------------------<br/>友情提醒：如果您每天都上人人网，也请帮您的亲朋好友注册一个人人网帐号吧！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击注册】</a>"
						);
				//IMWindowAdapter::instance().UserAdReminder("/imadinfo/zhuomianhuodong.do", userid);
				/*MessageMsgHandler::instance().deliverAppNotice(
					userid,
					3205,
					"人人桌面",
					"http://app.xnimg.cn/application/20100225/17/55/Lb7Sl5d018153.gif",
					"<br/>友情提醒：由于近期QQ与360软件的不兼容，为了防止您与QQ好友失去联系，建议您邀请Ta们使用人人桌面！随时与好友保持联系，聊天更安全！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://im.renren.com/?subver=44\">【点击这里邀请】</a>");*/
			}
			else{
				ostringstream os;
				os << "距离积分回馈活动截止还剩："<< active_day << "天" << active_hour <<"小时";
				MessageMsgHandlerAdapter::instance().deliverAppNotice(
						userid,
						3205,
						"人人桌面",
						"http://app.xnimg.cn/application/20100225/17/55/Lb7Sl5d018153.gif",
						os.str() +"<br/><br/>  即日起至10月31日，登录人人桌面             * 连续登录1-6天，登录积分翻三倍              * 连续登录7天以上，登录积分翻五倍             ______________________________________               如果您每天都上人人网，也帮您的亲朋好友        注册一个人人网帐号吧！&nbsp;<a href=\"wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击这里注册】</a>");
			}
		} catch (Ice::Exception& e) {
			MCE_WARN("ActiveUerNotify::notify-->MessageMsgHandlerAdapter::deliverAppNotice-->" << e);
		}

	}
  //send newsfeed

	return;
  try {

    //            if(userid != 255414584 && userid != 234159611  && userid != 255329305 && userid!= 260190016 ) return;
    static int n = 0;
    if (!(n = ++n % 3)) {
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
    //                        os << "u(200865373)";
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
			MCE_WARN("ActiveUserNotify::notify-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
		}
    if (u) {

      static string title[1] = { "获得了额外积分！2.0新版，不开网页看视频！" };
      static string body[1] = { "劲爆八卦，好友快照，抢先播报，极速秒回！2.0新版还能小窗看分享，搜索好友新鲜事！" };

      static string deskUrl[1] = { "http://im.renren.com/bd/bd6.html?newsfeed" };
      static string downloadUrl[1] = {
          "http://im.renren.com/desktop/rrsetup-2.0-6.exe?newsfeed" };
      static int i = 0;
      //              ++i;
      //              i %= 3;

      ostringstream xml;
      xml << "<f v=\"3.0\" type=\"imUse\">" << "<time>" << time(NULL) * 1000
          << "</time>" << "<type>1001</type>" << "<from>" << "<id>" << userid
          << "</id>" << "<name>" << u->name << "</name>" << "<tinyimg>"
          << u->tinyurl << "</tinyimg>" << "</from>" << "<im>"
          << "<title>登录了&lt;a href=&quot;" << deskUrl[i]
          << "&quot;&gt;人人桌面&lt;/a&gt;，" << title[i] << "</title>" << "<body>"
          << body[i] << "</body>" << "<url>" << downloadUrl[i] << "</url>"
          << "</im>" << "</f>";
      seed->xml = xml.str();

      //MCE_INFO("ActiveUserNotify::handle --> send feed " << userid);
      //FeedAssistantAdapter::instance().dispatch(seed);
    }

  } catch (Ice::Exception& e) {
    MCE_WARN("ActiveUserNotify::notify --> dispatch feed err:" << e);
  }

}

void ActiveUserNotify::needNotify(int userid, float ver) {
  MCE_DEBUG("ActiveUserNotify::needNotify --> userid:"<<userid<<"   ver:"<<ver);
  if (!_inited) {
    return;
  }
//  if (ver>=4.049 && ver <4.050) {
//    return;
//  }
  return ;   //停止所有弹窗

  bool insert = false;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    if (_users.find(userid) == _users.end()) {
      _users.insert(userid);
      _uids.push_back(UserStat(userid, ver));
      insert = true;
    }
  }
  if (insert) {
    Statement sql;
    sql << "insert into activeuser_per_day(userid) values(" << userid << ")";
    QueryRunner("im_stat", CDbWServer).schedule(sql);
  }
}

void ActiveUserNotify::handle() {
  //load login users last week
  if (!_inited) {
    loadUsers();
    _inited = true;
    MCE_INFO("ActiveUserNotify::handle --> inited: " << _inited);
  }

	updateSendNotifyCount();
  int tmp = _day;
  setDay();
  MCE_INFO("ActiveUserNotify::handle --> day " << _day);
  if (tmp != _day) {
    if (_index == 0) {
      Statement sql;
//      sql << "truncate table activeuser_per_day";
      sql << "delete from activeuser_per_day";
      QueryRunner("im_stat", CDbWServer).schedule(sql);
  
      sql.clear();
      sql << "delete from send_notify_count";
//      sql << "truncate table send_notify_count";
      QueryRunner("im_stat", CDbWServer).schedule(sql);
    }
    IceUtil::Mutex::Lock lock(_mutex);
    _users.clear();
    _uids.clear();
    _count = 0;
    MCE_INFO("ActiveUserNotify::handle --> another day " << _day);
  }
  MCE_INFO("ActiveUserNotify::handle --> notify size " << _uids.size());
  while (true) {
		int uid = 0;
		float ver = 0;
		{
			IceUtil::Mutex::Lock lock(_mutex);
			if (_uids.empty()) {
				break;
			}
			UserStat us = _uids.front();
			if ((time(NULL) - us.time_) < 60) {
				break;
			}
			_uids.pop_front();
			uid = us.uid_;
			ver = us.ver_;
		}
		if(uid){
			notify(uid, ver);
		}
  }

}

void ActiveUserNotify::loadUsers() {
  Statement sql;
  sql << "select * from activeuser_per_day";
  mysqlpp::StoreQueryResult res = QueryRunner("im_stat", CDbRServer).store(sql);
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

void ActiveUserNotify::updateSendNotifyCount() {
  Statement sql;
  sql
      << "insert into send_notify_count(name, count) values('send_notify_count', "
      << _count << ") on duplicate key update count=count+values(count)";
  QueryRunner("im_stat", CDbWServer).store(sql);
  _count = 0;

  sql.clear();
  sql
      << "select count from send_notify_count where name='send_notify_count' limit 1";
  mysqlpp::StoreQueryResult res = QueryRunner("im_stat", CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    mysqlpp::Row row = res.at(0);
    _notifyCount = (int) row["count"];
  }
  MCE_INFO("ActiveUserNotify::loadSendNotifyCount --> " << _notifyCount);
}

void ActiveUserNotify::setDay() {
  time_t tmp = time(NULL);
  tm* t = localtime(&tmp);
  _day = t->tm_wday;
}
