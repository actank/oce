#include "ClusterStateListener.h"

using namespace std;
using namespace xce::clusterstate;
using namespace MyUtil;

void ClusterStateListener::NotifyClient(const ServerStateMap& seqInfo) {
    	
	ServerStateMap tmp(seqInfo);
	ClusterStateInfoPtr  css = new ClusterStateInfo();
    	css->states.swap(tmp);

    	MyUtil::SubjectObserverI::instance().notifyall(_name , css); 
}
