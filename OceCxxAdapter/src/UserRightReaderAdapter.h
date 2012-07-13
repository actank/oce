#ifndef __USER_RIGHT_READER_ADAPTER_H__
#define __USER_RIGHT_READER_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Markable.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace userright {

using namespace xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;
class UserRightInfo;
typedef IceUtil::Handle<UserRightInfo> UserRightInfoPtr;

class UserRightInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	UserRightInfoPtr parse(const UserRightDataPtr& ptr) { id_ = ptr->uID; auth_ = ptr->auth; rights_ = ptr->rights; return this; }
	int id() const { return id_; }
	int auth() const { return auth_; }
	int rights() const { return rights_; }
	void set_id(int id) { id_ = id; mark(xce::userbase::CURID, boost::lexical_cast<std::string>(id)); }
	void set_auth(int auth) { auth_ = auth; mark(xce::userbase::CURAUTH, boost::lexical_cast<std::string>(auth)); }
	void set_rights(int rights) { rights_ = rights; mark(xce::userbase::CURCOMMONRIGHTS, boost::lexical_cast<std::string>(rights)); }
private:
	int id_;
	int auth_;
	int rights_;
};

class UserRightReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserRightReaderPrx>,
	public MyUtil::Singleton<UserRightReaderAdapter>
{
public:
	UserRightReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserRightReaderPrx>("ControllerUserBaseReader",120,300,new MyUtil::OceChannel, true,"URM"){
	}
	UserRightInfoPtr getUserRight(int id, const Ice::Context& ctx = Ice::Context());
	void setUserRight(int id, const MyUtil::Str2StrMap& props);
	void setData(int id, const Ice::ObjectPtr& data);
	UserRightReaderPrx getUserRightReaderPrx(int userId);

};

}
}
}
#endif

