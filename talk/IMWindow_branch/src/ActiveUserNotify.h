#ifndef ACTIVE_USER_NOTIFY_H_
#define ACTIVE_USER_NOTIFY_H_

#include <set>
#include <set>
#include "TaskManager.h" 
#include "Singleton.h" 
using namespace std;

class ActiveUserNotify : public MyUtil::Timer,
		public MyUtil::Singleton<ActiveUserNotify>{
public:
	static const int MAX_NOTIFY = 2000000;
	ActiveUserNotify() : Timer(60*1000){
		setDay();
		_count = 0;
		_inited = false;
		_notifyCount = 0;
    _index = -1;
    _notifydata="<br/><br/>&nbsp;&nbsp;<a href=\"http://page.renren.com/600003065/note/781547787\">【求投票】人人桌面向您拉票啦！</a><br/>&nbsp;&nbsp;同学们，年度软件评选又在火热进行中了！<br/>&nbsp;&nbsp;人人桌面需要您最最宝贵的一票！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href = \"http://page.renren.com/600003065/note/781547787\">立即投票>></a>";
    //_notifydata="<br/><a href=\"http://page.renren.com/600003065/note/781547787\">【活动】人人桌面入围年度软件评选！</a><br/>请为人人桌面投出您宝贵的一票吧！我们需要您的支持哦！&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href = \"http://page.renren.com/600003065/note/781547787\">点此去投票>>&nbsp;</a>_______________________________________<br/><a href=\"http://jisu.renren.com/?subver=25\">【人人极速相册1.1】稳定版：</a><br/>上传快、多、稳！本地管理网络相册！";
		//_notifydata = "<br/>您已额外获得一次今天的登录积分！<br/><br/>友情提醒：如果您每天都上人人网，也请帮您的同学、好友、家人注册一个人人网帐号吧！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"wwv.renren.com/xn.do?ss=17043&rt=1&g=notify\">【点击这里注册】</a>";
		//_notifydata = "<br/><a href=\"http://im.renren.com/huodong/zhounianqing.html?subver=44?04\">【周年庆活动】超值大奖天天送啦！！</a><br/>﹡11月15日-12月5日,登录人人桌面即可抽奖!<br/>﹡免费赢取人人网VIP、礼物会员、礼券、登录积分等大奖！中奖率100%！！<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href = \"http://im.renren.com/huodong/zhounianqing.html?subver=44?05\">立即抽奖>></a>";
	}
	void notify(int userid,float ver);
	void needNotify(int userid, float ver);
	void loadUsers();
	void updateSendNotifyCount();
	void setDay();
	void handle();
	void SetNotifyData(const string& notifydata);
  void setIndex(int index);
private:
	struct UserStat{
	  UserStat(int uid, float ver){
	    uid_ = uid;
	    ver_ = ver;
	    time_ = time(NULL);
	  }
	  int uid_;
	  float ver_;
	  time_t time_;
	};
	bool			_inited;
	set<int> 		_users;
	list<UserStat> 	_uids;
	
  int   _index;
	int		_day;
	int		_count;
	int		_notifyCount;
	string _notifydata;
	IceUtil::Mutex	_mutex;
};

#endif
