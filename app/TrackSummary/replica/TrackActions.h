#ifndef __TrackAction_h__
#define __TrackAction_h__

#include <ActiveTrack.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "Tracks.h"

#include "NotifySenderAdapter.h"
#include "AIGuideCacheAdapter.h"

namespace xce {
namespace activetrack {

using namespace ::com::xiaonei::wService;

class Action : virtual public Ice::Object{
public:
	virtual void execute(Ice::Int,std::string&,std::string&,TrackPtr){};
};
typedef IceUtil::Handle<Action> ActionPtr;

class LastURLAction : virtual public Action{
public:
	virtual void execute(Ice::Int,std::string&,std::string&,TrackPtr);
};

class AIGuideAction : virtual public Action{
public:
	AIGuideAction(Ice::PropertiesPtr);
	virtual void execute(Ice::Int,std::string&,std::string&,TrackPtr);
private:
	static std::string _FARMGAMEURL;
	static std::string _DOGURL;
	static std::string _CARKPARKURL;
	static std::string _GODFATHERURL;
	static std::string _TYCOONURL;
	int _startID;
	int getURLCode(std::string&);
	bool startsWith(std::string&,std::string&);
};

class AIGuideCacheTask : public MyUtil::Task {
public:
	AIGuideCacheTask(int userId, int appId):
		_userId(userId),
		_appId(appId) {
	}
	virtual void handle();
private:
	int _userId;
	int _appId;
};

class SendNotifyTask : public MyUtil::Task {
public:
	SendNotifyTask(int id, AIGuideStage stage):
		_id(id),
		_stage(stage){
	}
	virtual void handle();
private:
	int _id;
	AIGuideStage _stage;
};

}
}
#endif
