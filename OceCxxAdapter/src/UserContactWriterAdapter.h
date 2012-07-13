#ifndef __USER_CONTACT_WRITER_ADAPTER_H__
#define __USER_CONTACT_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace usercontact{

using namespace ::xce::userdesc;
using namespace __gnu_cxx;

#ifndef USER_CONTACT_INFO
#define USER_CONTACT_INFO

const int CLUSTER = 100;
const string UID = "ID";
const string MSN = "MSN";
const string MOBILEPHONE = "MOBILEPHONE";
const string FIXPHONE = "FIXPHONE";
const string QQ = "QQ";
const string HOMEPAGE = "HOMEPAGE";

class UserContactInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	int id() const { return id_; }
	string msn() const { return msn_; }
	string mobilephone() const { return mobilephone_; }
	string homepage() const { return homepage_; }
	long qq() const { return qq_; }
	string fixphone() const { return fixphone_; }
	void set_id(int id) { id_ = id; mark(UID, boost::lexical_cast<std::string>(id)); }
	void set_fixphone(const string& fixphone) { fixphone_ = fixphone; mark(FIXPHONE, fixphone); }
	void set_homepage(const string& homepage) { homepage_ = homepage; mark(HOMEPAGE, homepage); }
	void set_mobilephone(const string& mobilephone) { mobilephone_ = mobilephone; mark(MOBILEPHONE, mobilephone); }
	void set_qq(long qq) { qq_ = qq; mark(QQ, boost::lexical_cast<std::string>(qq)); }
	void set_msn(const string& msn) { msn_ = msn; mark(MSN, msn); }

private:
	int id_;
	string msn_;
	string mobilephone_;
	string fixphone_;
	long qq_;
	string homepage_;
};

typedef IceUtil::Handle<UserContactInfo> UserContactInfoPtr;
#endif

class UserContactWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<UserContactWriterPrx>,
	public MyUtil::Singleton<UserContactWriterAdapter>, IceUtil::Thread
{
public:
	UserContactWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserContactWriterPrx>("ControllerUserDescWriter",120,300,new MyUtil::OceChannel,true,"C"){
		start();
	}

	void setUserContact(int userid, const UserContactInfoPtr& info);
	void setUserContact(int userId, const MyUtil::Str2StrMap& props);
	void createUserContact(int userId, const UserContactInfoPtr& info);
	void createUserContact(int userId, const MyUtil::Str2StrMap& props);
	void reload(int id, const Ice::Context& ctx = Ice::Context());

protected:

	void run();

private:

	UserContactWriterPrx getUserContactWriter(int userId);
	UserContactWriterPrx getUserContactWriterOneway(int userId);
	UserContactWriterPrx getUserContact10sWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}

#endif

