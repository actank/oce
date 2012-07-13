#ifndef __USER_DOING_READER_ADAPTER_H__
#define __USER_DOING_READER_ADAPTER_H__

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

class UserDoingReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserDoingReaderPrx>,
	public MyUtil::Singleton<UserDoingReaderAdapter>
{
public:
	UserDoingReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserDoingReaderPrx>("ControllerUserBaseReader",120,300,new MyUtil::OceChannel,true,"UDM"){
	}
	UserDoingInfoPtr getUserDoing(int id, const Ice::Context& ctx = Ice::Context());
	void setUserDoing(int id, const MyUtil::Str2StrMap& props);
	void setData(int id, const Ice::ObjectPtr& data);
	UserDoingReaderPrx getUserDoingReaderPrx(int id);
};

}
}
}

#endif

