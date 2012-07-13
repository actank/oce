#include "UserConfigAdapter.h"

#include <boost/lexical_cast.hpp>

using namespace xce::adapter;
using namespace std;
using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace boost;

UserConfigInfoPtr xce::adapter::UserConfigInfo::parse(const MyUtil::Str2StrMap& data) {
	Str2StrMap::const_iterator i;
	i = data.find("ID");
	if (i != data.end()) {
		userId_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("BASIC_CONFIG");
	if (i != data.end()) {
		basicConfig_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("POKE_CONFIG");
	if (i != data.end()) {
		pokeConfig_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("REQUEST_FRIEND_CONFIG");
	if (i != data.end()) {
		requestFriendConfig_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("PHOTO_CONFIG");
	if (i != data.end()) {
		photoConfig_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("MESSAGE_CONFIG");
	if (i != data.end()) {
		messageConfig_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("EMAIL_CONFIG");
	if (i != data.end()) {
		emailConfig_ = boost::lexical_cast<long>(i->second);
	}
	i = data.find("BROWSE_CONFIG");
	if (i != data.end()) {
		browseConfig_ = boost::lexical_cast<int>(i->second);
	}
	i = data.find("RECV_FEED_CONFIG");
	if (i != data.end()) {
		recvFeedConfig_ = i->second;
	}
	i = data.find("SEND_FEED_CONFIG");
	if (i != data.end()) {
		sendFeedConfig_ = i->second;
	}
	i = data.find("WANT_SEE_CSS");
	if (i != data.end()) {
		wantSeeCss_ = boost::lexical_cast<short>(i->second);
	}
	i = data.find("DISPLAY_MENU_LIST");
	if (i != data.end()) {
		displayMenuList_ = i->second;
	}
	i = data.find("MORE_MENU_LIST");
	if (i != data.end()) {
		moreMenuList_ = i->second;
	}
	i = data.find("PROFILE_PRIVACY");
	if (i != data.end()) {
		profilePrivacy_ = i->second;
	}
	i = data.find("STATUS_CONFIG");
	if (i != data.end()) {
		statusConfig_ = boost::lexical_cast<int>(i->second);
	}
  return this;
}

xce::adapter::UserConfigInfoPtr UserConfigAdapter::getUserConfig(int userid, const Ice::Context& ctx) {
  UserConfigInfoPtr info=new UserConfigInfo;
  info->parse(getUserConfigManager(userid)->getUserConfig(userid));
	return info;
}
 
void UserConfigAdapter::setUserConfig(int id, const UserConfigInfoPtr& info) {
  if (id <= 0) {
    return;
  }
  getUserConfigManager(id)->setUserConfig(id, info->demark());
}

void UserConfigAdapter::createUserConfig(int id, const UserConfigInfoPtr& info) {
  if (id <= 0) {
    return;
  }
  getUserConfig10sManager(id)->createUserConfig(info->demark());
}

void UserConfigAdapter::setUserConfig(int userId, const MyUtil::Str2StrMap& properties){
  if (userId <= 0) {
	return;
  }
  getUserConfig10sManager(userId)->setUserConfig(userId, properties);
}

void UserConfigAdapter::createUserConfig(int userId, const Str2StrMap& properties){
  if (userId <= 0) {
	return;
  }
  getUserConfig10sManager(userId)->createUserConfig(properties);
}

mop::hi::svc::model::UserConfigManagerPrx UserConfigAdapter::getUserConfig10sManager(int id) {
	return getProxy(id);
}

mop::hi::svc::model::UserConfigManagerPrx UserConfigAdapter::getUserConfigManager(int id) {
	return getProxy(id);
}

mop::hi::svc::model::UserConfigManagerPrx UserConfigAdapter::getUserConfigManagerOneway(int id) {
	return getProxyOneway(id);
}

void UserConfigAdapter::reload(int id, const Ice::Context& ctx)
{
     getUserConfigManager(id)->reload(id, ctx);
}
