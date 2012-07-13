#include "OnlineStateFilterI.h"
#include <exception>
#include <IceUtil/IceUtil.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::onlinestatefilter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&OnlineStateFilterManagerI::instance(),service.createIdentity("M", ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OnlineStateFilter.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OnlineStateFilter.Interval", 5);
        ServerStateSubscriber::instance().initialize("ControllerOnlineStateFilter", &OnlineStateFilterManagerI::instance(), mod, interval);
	OnlineStateFilterManagerI::instance().initialize();
}

namespace xce{
namespace onlinestatefilter {

OnlineStateFilterManagerI::OnlineStateFilterManagerI() {
}

void OnlineStateFilterManagerI::initialize() {
	MCE_INFO("OnlineStateFilterManagerI::initialize -> start ...");

	//TODO: read onlinestate for all users from local file

	MCE_INFO("OnlineStateFilterManagerI::OnlineStateFilterManagerI");
	for (int i=0;i<ONLINESTATE_FILTER_SIZE;++i) {
		_online_state.reset(i);
	}
	MCE_INFO("OnlineStateFilterManagerI::OnlineStateFilterManagerI -> done");
}

ByteSeq OnlineStateFilterManagerI::getOnlineStateMinMax(int min, int max, const Ice::Current& current){
	MCE_INFO("OnlineStateFilterManagerI::getOnlineStateMinMax -> min: "<<min << " max: "<<max
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	ByteSeq result;
	if (max <= min) {
		MCE_WARN("OnlineStateFilterManagerI::getOnlineStateMinMax -> min >= max");
		return result;
	}
	int n=(max-min)/8;
	MCE_DEBUG("OnlineStateFilterManagerI::getOnlineStateMinMax -> n: "<<n);
	for (int i=0;i<n;i++){
		result.push_back(Ice::Byte());
	}
	for(int i = min; i < max; ++i) {
		if (_online_state[i]) {
			result[(i-min)/8]|=(1<<(i-min)%8);
		}
	}
	MCE_DEBUG("OnlineStateFilterManagerI::getOnlineStateMinMax -> size: "<<result.size());
	return result;
}

Int2BoolMap OnlineStateFilterManagerI::getOnlineStateBatch(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	Int2BoolMap res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		res[*it] = _online_state[*it];
		op << *it << "(" << res[*it] << ") ";
	}
	MCE_INFO("OnlineStateFilterManagerI::getOnlineStateBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

MyUtil::IntSeq OnlineStateFilterManagerI::getOnlineIdsFrom(const MyUtil::IntSeq& ids, const Ice::Current& current){
	ostringstream op;
	IntSeq res;
	for( MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ){
		if( _online_state[*it] ){
			res.push_back( *it );
			op << *it << "(" << true << ") ";
		}
	}
	MCE_INFO("OnlineStateFilterManagerI::getOnlineStateBatch -> " << op.str()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

bool OnlineStateFilterManagerI::isOnlineStateTrue(int id, const Ice::Current& current){
	MCE_INFO("OnlineStateFilterManagerI::isOnlineStateTrue -> id: "<<id
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return _online_state[id];
}

void OnlineStateFilterManagerI::updateOnlineState(int userid,int onlinestate, const Ice::Current& current){
	MCE_INFO("OnlineStateFilterManagerI::updateOnlineState -> userid: "<<userid<<" onlinestate: "<<onlinestate);
	if (onlinestate) {
		_online_state.set(userid);
	}  else {
		_online_state.reset(userid);
	}
}

void OnlineStateFilterManagerI::updateOnlineStateBatch(const ::MyUtil::Int2IntMap& onlinestatemap, const Ice::Current& current){
	ostringstream op;
        for (Int2IntMap::const_iterator it = onlinestatemap.begin(); it != onlinestatemap.end(); ++it) {
		if (it->second) {
			_online_state.set(it->first);
		}  else {
			_online_state.reset(it->first);
		}
		op << it->first << "(" << it->second << ") ";
        }
	MCE_INFO("OnlineStateFilterManagerI::updateOnlineStateBatch -> " << op.str() );
}

/*void OnlineStateFilterManagerI::run() {
	//TODO: write _online_state to local file
}*/

}
}
