#ifndef __USER_STAGE_READER_ADAPTER_H__
#define __USER_STAGE_READER_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace userstage {

using namespace ::xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;
class UserStageInfo;
typedef IceUtil::Handle<UserStageInfo> UserStageInfoPtr;

class UserStageInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	UserStageInfoPtr parse(const UserStageDataPtr& data) { id_ = data->id; univ_ = data->univ; stage_ = data->stage; return this; }
	int id() const { return id_; }
	int univ() const { return univ_; }
	int stage() const { return stage_; }
	void set_univ(int univ) { univ_ = univ; mark(xce::userbase::CUSGUNIV, boost::lexical_cast<std::string>(univ));}
	void set_id(int id) { id_ = id; mark(xce::userbase::CUSGID, boost::lexical_cast<std::string>(id));}
	void set_stage(int stage) { stage_ = stage; mark(xce::userbase::CUSGSTAGE, boost::lexical_cast<std::string>(stage));}
private:
	int id_;
	int univ_;
	int stage_;
};

class UserStageReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserStageReaderPrx>,
	public MyUtil::Singleton<UserStageReaderAdapter>
{
public:
	UserStageReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserStageReaderPrx>("ControllerUserBaseReader",120,300,new MyUtil::OceChannel,true,"USGM"){
	}
	UserStageInfoPtr getUserStage(int id, const Ice::Context& ctx = Ice::Context());
	void setUserStage(int id, const MyUtil::Str2StrMap& props);
	void setData(int id, const Ice::ObjectPtr& data);
	UserStageReaderPrx getUserStageReaderPrx(int userId);
};

}
}
}

#endif

