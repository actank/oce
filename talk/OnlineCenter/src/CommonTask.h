#ifndef COMMONTASK_H_
#define COMMONTASK_H_
#include "TaskManager.h"
#include "TalkCommon.h"
#include "User.h"
#include "OnlineManagerI.h"
using namespace com::xiaonei::talk::common;

class PresenceTask : public MyUtil::Task {
	AvatarPtr _avatar;
	//bool _login;
	OnlineStat _oldStat;
	OnlineStat _newStat;
	//string _status;
	//string _photo;
	MyUtil::Str2StrMap _parameters;
public:
	PresenceTask(const AvatarPtr& avatar, OnlineStat oldStat, OnlineStat newStat,
			const MyUtil::Str2StrMap& paras) :
		Task(), _avatar(avatar), _oldStat(oldStat), _newStat(newStat), _parameters(paras) {
	}
	;
	virtual void handle();
};

#endif
