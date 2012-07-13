/*
 * =====================================================================================
 *
 *       Filename:  UserPassportAdapter.h
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


#ifndef __USER_URL_ADAPTER_H__
#define __USER_URL_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Markable.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace userurl{
using namespace xce::userbase;
using namespace MyUtil;

class UserUrlInfo;

typedef IceUtil::Handle<UserUrlInfo> UserUrlInfoPtr;

class UserUrlInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
  UserUrlInfoPtr parse(const UserUrlDataPtr& data) {id_ = data->id;largeUrl_ = data->largeUrl;mainUrl_ = data->mainUrl;headUrl_ = data->headUrl;tinyUrl_ = data->tinyUrl;return this;}
  int id() const {return id_;}
  std::string largeUrl() const {return largeUrl_;}
  std::string mainUrl() const {return mainUrl_;}
  std::string headUrl() const {return headUrl_;}
  std::string tinyUrl() const {return tinyUrl_;}
  void set_id(int id) {id_ = id;mark("ID", boost::lexical_cast<std::string>(id));}
  void set_largeUrl(const std::string& largeUrl) {largeUrl_ = largeUrl; mark("LARGEURL", largeUrl);}
  void set_tinyUrl(const std::string& tinyUrl) {tinyUrl_ = tinyUrl; mark("TINYURL", tinyUrl);}
  void set_headUrl(const std::string& headUrl) {headUrl_ = headUrl; mark("HEADURL", headUrl);}
  void set_mainUrl(const std::string& mainUrl) {mainUrl_ = mainUrl; mark("MAINURL", mainUrl);}
private:
  int id_;
  std::string largeUrl_;
  std::string mainUrl_;
  std::string headUrl_;
  std::string tinyUrl_;
};

const int CLUSTER = 100;

class UserUrlAdapter : public MyUtil::ReplicatedClusterAdapterI<UserUrlManagerPrx>,
	public MyUtil::Singleton<UserUrlAdapter>
{
public:
	UserUrlAdapter() : MyUtil::ReplicatedClusterAdapterI<UserUrlManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel, true,"U"){
	}
	UserUrlInfoPtr getUserUrl(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserUrl(int userid, const UserUrlInfoPtr& info);
  void createUserUrl(int userid, const UserUrlInfoPtr& info);
	void reload(int id, const Ice::Context& ctx = Ice::Context());

	UserUrlManagerPrx getUserUrlManager(int userId);
	UserUrlManagerPrx getUserUrl10sManager(int userId);
	UserUrlManagerPrx getUserUrlManagerOneway(int userId);
};

}
}
}
#endif

