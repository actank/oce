#include "UserDescMasterToSlaveAdapter.h"

using namespace xce::adapter::userdescmastertoslave; 


void UserContactMasterToSalveAdapter::create(const int userId)
{
	Add(userId, new CallCreateHandle(userId, getAllProxySeq(userId)));
}

void UserContactMasterToSalveAdapter::set(const int userId, const MyUtil::Str2StrMap& props)
{
	Add(userId, new CallSetHandle(userId, props, getAllProxySeq(userId)));
}

void UserContactMasterToSalveAdapter::reload(const int userId)
{
	Add(userId, new CallReloadHandle(userId, getAllProxySeq(userId)));
}

void UserTimeMasterToSalveAdapter::create(const int userId)
{
	Add(userId, new CallCreateHandle(userId, getAllProxySeq(userId)));
}

void UserTimeMasterToSalveAdapter::set(const int userId, const MyUtil::Str2StrMap& props)
{
	Add(userId, new CallSetHandle(userId, props, getAllProxySeq(userId)));
}

void UserTimeMasterToSalveAdapter::reload(const int userId)
{
	Add(userId, new CallReloadHandle(userId, getAllProxySeq(userId)));
}

