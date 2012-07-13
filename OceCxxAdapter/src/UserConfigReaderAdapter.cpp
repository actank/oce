#include "UserConfigReaderAdapter.h"

using namespace xce::readeradapter;
using namespace std;
using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace boost;

//UserConfigInfo
//--------------------------------------------------------------
UserConfigInfoPtr UserConfigInfo::parse(const MyUtil::Str2StrMap& data) {
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

//原有的供外部调用的接口
//--------------------------------------------------------------
UserConfigInfoPtr UserConfigReaderAdapter::getUserConfig(int userId, const Ice::Context& ctx) {
	UserConfigInfoPtr info = new UserConfigInfo;
	info->parse(getUserConfigReader(userId)->getUserConfig(userId));
	return info;
}

//新加的供Writer/oldwr调用的接口
//--------------------------------------------------------------
void UserConfigReaderAdapter::setUserConfig(int userId, const MyUtil::Str2StrMap& properities) {
	if (userId <= 0) {
		return;
	}
	std::vector<UserConfigReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<UserConfigReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(10000)->setUserConfig(userId, properities);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserConfigReaderAdapter::setUserConfig Exception id = " << userId << " " << e.what());
		}
	}
}

void UserConfigReaderAdapter::setData(int userId, const Ice::ObjectPtr& data) {
	if (!data) {
		return;
	}

	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(userId, data));

	std::vector<UserConfigReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<UserConfigReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(10000)->setData(res);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserConfigReaderAdapter::setData Exception id=" << userId << " " << e.what());
		}
	}
}

//获取代理
//--------------------------------------------------------------
UserConfigReaderPrx UserConfigReaderAdapter::getUserConfigReader(int userId) {
	return getProxy(userId);
}
