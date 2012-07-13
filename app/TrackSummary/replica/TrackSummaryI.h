#ifndef __TrackSummaryI_h__
#define __TrackSummaryI_h__

#include <TrackSummary.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "Tracks.h"
#include "TrackActions.h"

namespace xce {
namespace activetrack {

class TrackFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int);
};
class TrackResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	TrackResultHandlerI(const TrackPtr result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	TrackPtr _result;
};

class TrackSummaryI: virtual public TrackSummary {
public:
	TrackSummaryI(Ice::PropertiesPtr);
	virtual void access(
			Ice::Int,
			const std::string&,
			const std::string&,
			const Ice::Current&);

	virtual MyUtil::Str2StrMap get(Ice::Int, const Ice::Current&);
	virtual void set(Ice::Int, const std::string&, const std::string&, const Ice::Current&);

private:
	TrackPtr locateTrack(Ice::Int);
	std::vector<ActionPtr> _actions;
};
typedef IceUtil::Handle<TrackSummaryI> TrackSummaryIPtr;

class ActionTask : virtual public MyUtil::Task {
public:
	ActionTask(Ice::Int ownerId,
			const std::string& ip,
			const std::string& url,
			ActionPtr action,
			TrackPtr userTrack):
				_ownerId(ownerId),_ip(ip),_url(url),_action(action),_userTrack(userTrack){

	};
	virtual void handle();
private:
	Ice::Int _ownerId;
	std::string _ip;
	std::string _url;
	ActionPtr _action;
	TrackPtr _userTrack;
};


}
}
#endif
