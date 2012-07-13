#ifndef __USER_CONFIG_READER_ADAPTER_H__
#define __USER_CONFIG_READER_ADAPTER_H__

#include "UserConfig.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <Markable.h>
#include <boost/lexical_cast.hpp>

namespace xce {
namespace readeradapter {

class UserConfigInfo;

typedef IceUtil::Handle<UserConfigInfo> UserConfigInfoPtr;

class UserConfigInfo: virtual public Ice::Object, public MyUtil::Markable {
public:
	UserConfigInfoPtr parse(const MyUtil::Str2StrMap& data);

	int userId() const {
		return userId_;
	}
	int basicConfig() const {
		return basicConfig_;
	}
	int pokeConfig() const {
		return pokeConfig_;
	}
	int requestFriendConfig() const {
		return requestFriendConfig_;
	}
	int photoConfig() const {
		return photoConfig_;
	}
	int messageConfig() const {
		return messageConfig_;
	}
	long emailConfig() const {
		return emailConfig_;
	}
	int browseConfig() const {
		return browseConfig_;
	}
	string displayMenuList() const {
		return displayMenuList_;
	}
	string moreMenuList() const {
		return moreMenuList_;
	}
	short wantSeeCss() const {
		return wantSeeCss_;
	}
	string sendFeedConfig() const {
		return sendFeedConfig_;
	}
	string recvFeedConfig() const {
		return recvFeedConfig_;
	}
	string profilePrivacy() const {
		return profilePrivacy_;
	}
	int statusConfig() const {
		return statusConfig_;
	}

	void set_userId(int userId) {
		userId_ = userId;
		mark("ID", boost::lexical_cast<string>(userId));
	}
	void set_basicConfig(int basicConfig) {
		basicConfig_ = basicConfig;
		mark("BASIC_CONFIG", boost::lexical_cast<string>(basicConfig));
	}
	void set_pokeConfig(int pokeConfig) {
		pokeConfig_ = pokeConfig;
		mark("POKE_CONFIG", boost::lexical_cast<string>(pokeConfig));
	}
	void set_requestFriendConfig(int requestFriendConfig) {
		requestFriendConfig_ = requestFriendConfig;
		mark("REQUEST_FRIEND_CONFIG", boost::lexical_cast<string>(requestFriendConfig));
	}
	void set_photoConfig(int photoConfig) {
		photoConfig_ = photoConfig;
		mark("PHOTO_CONFIG", boost::lexical_cast<string>(photoConfig));
	}
	void set_messageConfig(int messageConfig) {
		messageConfig_ = messageConfig;
		mark("MESSAGE_CONFIG", boost::lexical_cast<string>(messageConfig));
	}
	void set_emailConfig(long emailConfig) {
		emailConfig_ = emailConfig;
		mark("EMAIL_CONFIG", boost::lexical_cast<string>(emailConfig));
	}
	void set_browseConfig(int browseConfig) {
		browseConfig_ = browseConfig;
		mark("BROWSE_CONFIG", boost::lexical_cast<string>(browseConfig));
	}
	void set_displayMenuList(const string& displayMenuList) {
		displayMenuList_ = displayMenuList;
		mark("DISPLAY_MENU_LIST", displayMenuList);
	}
	void set_moreMenuList(const string& moreMenuList) {
		moreMenuList_ = moreMenuList;
		mark("MORE_MENU_LIST", moreMenuList);
	}
	void set_wantSeeCss(short wantSeeCss) {
		wantSeeCss_ = wantSeeCss;
		mark("WANT_SEE_CSS", boost::lexical_cast<string>(wantSeeCss));
	}
	void set_sendFeedConfig(const string& sendFeedConfig) {
		sendFeedConfig_ = sendFeedConfig;
		mark("SEND_FEED_CONFIG", sendFeedConfig);
	}
	void set_recvFeedConfig(const string& recvFeedConfig) {
		recvFeedConfig_ = recvFeedConfig;
		mark("RECV_FEED_CONFIG", recvFeedConfig);
	}
	void set_profilePrivacy(const string& profilePrivacy) {
		profilePrivacy_ = profilePrivacy;
		mark("PROFILE_PRIVACY", profilePrivacy);
	}
	void set_statusConfig(int statusConfig) {
		statusConfig_ = statusConfig;
		mark("STATUS_CONFIG", boost::lexical_cast<string>(statusConfig));
	}
private:
	int userId_;
	int basicConfig_;
	int pokeConfig_;
	int requestFriendConfig_;
	int photoConfig_;
	int messageConfig_;
	long emailConfig_;
	int browseConfig_;
	string displayMenuList_;
	string moreMenuList_;
	short wantSeeCss_;
	string sendFeedConfig_;
	string recvFeedConfig_;
	string profilePrivacy_;
	int statusConfig_;
};

class UserConfigReaderAdapter: public MyUtil::ReplicatedClusterAdapterI<mop::hi::svc::model::UserConfigReaderPrx>,
		public MyUtil::Singleton<UserConfigReaderAdapter> {
public:
	UserConfigReaderAdapter() :
		MyUtil::ReplicatedClusterAdapterI<mop::hi::svc::model::UserConfigReaderPrx>("ControllerUserConfigReader", 120, 300) {
	}

	UserConfigInfoPtr getUserConfig(int userId, const Ice::Context& ctx = Ice::Context());

	void setUserConfig(int userId, const MyUtil::Str2StrMap& properties);
	void setData(int userId, const Ice::ObjectPtr& userData);

private:
	mop::hi::svc::model::UserConfigReaderPrx getUserConfigReader(int userId);
};

}
}

#endif

