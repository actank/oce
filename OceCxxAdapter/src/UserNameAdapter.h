/*
 * =====================================================================================
 *
 *       Filename:  UserBaseAdapter.h
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


#ifndef __USER_NAME_ADAPTER_H__
#define __USER_NAME_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Markable.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace username {

using namespace ::xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;

class UserNameInfo;

typedef IceUtil::Handle<UserNameInfo> UserNameInfoPtr;

class UserNameInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
  UserNameInfoPtr parse(const UserNameDataPtr& data) {id_ = data->uID;name_ = data->name; nickname_ = data->nickname;return this;}
  std::string name() const {return name_;}
  std::string nickname() const {return nickname_;}
  int id() const {return id_;}
  void set_id(int id) {id_ = id;mark(xce::userbase::CUNID, boost::lexical_cast<std::string>(id));}
  void set_name(const string& name) {name_ = name;mark(xce::userbase::CUNNAME, name);}
  void set_nickname(const string& nickname) {nickname_ = nickname;mark(xce::userbase::CUNNICKNAME, nickname);}
private:
  int id_;
  std::string name_;
  std::string nickname_;
};

class UserNameAdapter : public MyUtil::ReplicatedClusterAdapterI<UserNameManagerPrx>,
	public MyUtil::Singleton<UserNameAdapter> {

public:
	UserNameAdapter() : MyUtil::ReplicatedClusterAdapterI<UserNameManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel,true,"UNM"){
	}
	void reload(int id, const Ice::Context& ctx = Ice::Context());
	
  UserNameInfoPtr getUserName(int uid, const Ice::Context& ctx = Ice::Context());
  void setUserName(int id, const UserNameInfoPtr& info);
  void createUserName(int id, const UserNameInfoPtr& info);
		
	UserNameManagerPrx getUserNameManager(int userId);
	UserNameManagerPrx getUserName10sManager(int userId);
	UserNameManagerPrx getUserNameManagerOneway(int userId);
};

}
}
}

#endif

