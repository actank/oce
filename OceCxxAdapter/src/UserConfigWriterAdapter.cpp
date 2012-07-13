#include "UserConfigWriterAdapter.h"

using namespace xce::adapter;
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
void UserConfigWriterAdapter::setUserConfig(int userId, const UserConfigInfoPtr& info) {
	if (userId <= 0) {
		return;
	}
	getUserConfigWriter(userId)->setUserConfig(userId, info->demark());
}

void UserConfigWriterAdapter::createUserConfig(int userId, const UserConfigInfoPtr& info) {
	if (userId <= 0) {
		return;
	}
	getUserConfig10sWriter(userId)->createUserConfig(info->demark());
}

//修改接口。修改实现，功能不变
//--------------------------------------------------------------
void UserConfigWriterAdapter::reload(int userId) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(userId);
	_mutex.notify();
}

void UserConfigWriterAdapter::run()
{
	while(true)
	{
		MyUtil::IntSeq ids;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
			if(_idSet.empty())
			{
				_mutex.wait();
			}
			ids.insert(ids.end(), _idSet.begin(), _idSet.end());
			_idSet.clear();
		}
		for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
		{
			UserConfigWriterPrx prx = getUserConfigWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserConfigWriterAdapter::run reload Exception id=" << *it <<" " << e.what());
				}
			}
		}
	}
}

//获取代理
//--------------------------------------------------------------
UserConfigWriterPrx UserConfigWriterAdapter::getUserConfig10sWriter(int userId) {
	return getProxy(userId);
}

UserConfigWriterPrx UserConfigWriterAdapter::getUserConfigWriter(int userId) {
	return getProxy(userId);
}
