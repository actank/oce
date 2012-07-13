#include "UserContactWriterAdapter.h"
#include "Date.h"
using namespace xce::adapter::usercontact;
using namespace ::xce::userdesc;

UserContactWriterPrx UserContactWriterAdapter::getUserContactWriter(int id)
{
	return getProxy(id);
}

UserContactWriterPrx UserContactWriterAdapter::getUserContactWriterOneway(int id)
{
	return getProxyOneway(id);
}

UserContactWriterPrx UserContactWriterAdapter::getUserContact10sWriter(int id)
{
	return getProxy(id)->ice_timeout(10000);
}

void UserContactWriterAdapter::createUserContact(int userId, const UserContactInfoPtr& info) {
	if(userId <= 0) {
		return;
	}
	getUserContact10sWriter(userId)->createUserContact(info->demark());
}

void UserContactWriterAdapter::createUserContact(int userId, const MyUtil::Str2StrMap& props) {
	if(userId <= 0) {
		return;
	}
	getUserContact10sWriter(userId)->createUserContact(props);
}
void UserContactWriterAdapter::setUserContact(int userid, const UserContactInfoPtr& info) {
	if(userid <= 0) {
		return;
	}
	if(info->id() != 0 && userid != info->id()) {
		info->set_id(userid);
	}
	getUserContactWriter(userid)->setUserContact(userid, info->demark());
}

void UserContactWriterAdapter::setUserContact(int userId, const MyUtil::Str2StrMap& props) {
	if(userId<= 0) {
		return;
	}
	getUserContactWriter(userId)->setUserContact(userId, props);
}

void UserContactWriterAdapter::reload(int id, const Ice::Context& ctx)
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
	_idSet.insert(id);
	_mutex.notify();
}

void UserContactWriterAdapter::run()
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
			UserContactWriterPrx prx = getUserContactWriter(*it);
			if(prx)
			{
				try
				{
					prx->reload(*it);
				}catch(Ice::Exception& e)
				{
					MCE_WARN("UserContactWriterAdapter::run reload Exception id = " << *it <<" " << e.what());
				}
			}
		}
	}
}


