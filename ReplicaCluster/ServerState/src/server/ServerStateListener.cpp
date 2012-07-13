#include "ServerStateListener.h"

using namespace std;
using namespace xce::serverstate;
using namespace MyUtil;

ServerStateSeq ServerStateListener::fetch() {

	SbjStatePtr state = new SbjState();
    	SbjStateSeq tmp1 = MyUtil::SubjectObserverI::instance().fetch(_name, state);
	ServerStateSeq tmp2;
	for ( SbjStateSeq::iterator it = tmp1.begin(); it != tmp1.end(); ++it ) {
		ServerStateInfoPtr pTmp = ServerStateInfoPtr::dynamicCast(*it);
		tmp2.push_back(pTmp);		
	}
	_ssss.swap(tmp2);
	return _ssss;
}

ServerStateSeq ServerStateListener::getSeq(){
    return _ssss;
}


