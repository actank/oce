/*
 * =====================================================================================
 *
 *       Filename:  UserTimeReaderAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时02分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#ifndef __USER_TIME_READER_ADAPTER_H__
#define __USER_TIME_READER_ADAPTER_H__

#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Markable.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace usertime{

using namespace ::xce::userdesc;
using namespace MyUtil;

const int CLUSTER = 100;

class UserTimeInfo;
typedef IceUtil::Handle<UserTimeInfo> UserTimeInfoPtr;

const std::string UID = "ID";
const std::string REGISTERTIME = "REGISTERTIME";
const std::string ACTIVATETIME = "ACTIVATETIME";
const std::string LASTLOGINTIME = "LASTLOGINTIME";
const std::string LASTLOGOUTTIME = "LASTLOGOUTTIME";

class UserTimeInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	 UserTimeInfoPtr parse(const UserTimeDataPtr& data) { id_ = data->id; registerTime_ = data->registerTime; activateTime_ = data->activateTime; 
															lastLoginTime_ = data->lastLoginTime; lastLogoutTime_ = data->lastLogoutTime; return this;}
	 int id() const { return id_; }
	 int registerTime() { return registerTime_; }
	 int activateTime() { return activateTime_; }
	 int lastLoginTime() { return lastLoginTime_; }
	 int lastLogoutTime() { return lastLogoutTime_; }
	 void set_id(int id) { id_ = id; mark(UID, boost::lexical_cast<std::string>(id)); }
	 void set_registerTime(int registerTime) { registerTime_ = registerTime_; mark(REGISTERTIME, getTimeStr(registerTime)); }
	 void set_activateTime(int activateTime) { activateTime_ = activateTime; mark(ACTIVATETIME, getTimeStr(activateTime)); }
	 void set_lastLoginTime(int lastLoginTime) { lastLoginTime_ = lastLoginTime; mark(LASTLOGINTIME, getTimeStr(lastLoginTime)); }
	 void set_lastLogoutTime(int lastLogoutTime) { lastLogoutTime_ = lastLogoutTime; mark(LASTLOGOUTTIME, getTimeStr(lastLogoutTime)); }

private:
	 int id_;
	 int registerTime_;
	 int activateTime_;
	 int lastLoginTime_;
	 int lastLogoutTime_;
	 string getTimeStr(int sec) {
		time_t temp_sec = sec;
		struct tm* ptr = localtime(&temp_sec);
		char time_str[22];
		strftime(time_str, 22, "%Y-%m-%d %H:%M:%S", ptr);
		return string(time_str);
	 }

};

class UserTimeReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserTimeReaderPrx>,
	public MyUtil::Singleton<UserTimeReaderAdapter>
{
public:
	UserTimeReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserTimeReaderPrx>("ControllerUserDescReader",120,300,new MyUtil::OceChannel,true,"T"){

	}
	UserTimeInfoPtr getUserTime(int userId, const Ice::Context& ctx = Ice::Context());
	UserTimeDataPtr getUserTimeData(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserTime(int id, const MyUtil::Str2StrMap& props, const Ice::Context& ctx = Ice::Context());
	void setData(int id, const Ice::ObjectPtr& data);
private:
	UserTimeReaderPrx getUserTimeReader(int userId);
};

}
}
}

#endif

