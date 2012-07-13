/*
 * =====================================================================================
 *
 *       Filename:  UserDoingAdapter.h
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


#ifndef __USER_DOING_ADAPTER_H__
#define __USER_DOING_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace userdoing {

using namespace ::xce::userbase;
using namespace MyUtil;

class UserDoingInfo;
typedef IceUtil::Handle<UserDoingInfo> UserDoingInfoPtr;

class UserDoingInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	UserDoingInfoPtr parse(const UserDoingDataPtr& data) { id_ = data->id; doing_ = data->doing; doingTime_ = data->doingTime; return this; }
	int id() const { return id_; }
	string doing() const { return doing_; }
	int dointTime() const { return doingTime_; }
	void set_id(int id) { id_ = id; mark(xce::userbase::CUDID, boost::lexical_cast<std::string>(id)); }
	void set_doing(const string& doing) { doing_ = doing; mark(xce::userbase::CUDDOING, doing); }
	void set_dointTime(int doingTime) { doingTime_ = doingTime; mark(xce::userbase::CUDDOINGTIME, boost::lexical_cast<std::string>(doingTime)); }

private:
	int id_;
	string doing_;
	int doingTime_;
};

class UserDoingAdapter : public MyUtil::ReplicatedClusterAdapterI<UserDoingManagerPrx>,
	public MyUtil::Singleton<UserDoingAdapter>
{
public:
	UserDoingAdapter() : MyUtil::ReplicatedClusterAdapterI<UserDoingManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel,true,"UDM"){
	}
	UserDoingInfoPtr getUserDoing(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserDoing(int userId, const UserDoingInfoPtr& ptr);
	void createUserDoing(int userId, const UserDoingInfoPtr& ptr);
	void reload(int userId);
	UserDoingManagerPrx getUserDoing10sManager(int userId);	
	UserDoingManagerPrx getUserDoingManager(int userId);
	UserDoingManagerPrx getUserDoingManagerOneway(int userId);
};

}
}
}

#endif

