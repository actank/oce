#include "FriendFinderNewComerWorkerAdapter.h"

using namespace xce::socialgraph::adapter;
using namespace xce::socialgraph;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= FriendFinderNewComerWorkerAdapter =================================

void FriendFinderNewComerWorkerAdapter::apply( int applicant,int acceptor ){
        Info information;
        information.operation = APPLY;
        information.hostId = applicant;
        information.friendIds.push_back(acceptor);
        getProxy(applicant)->Report(information);
}
void FriendFinderNewComerWorkerAdapter::accept( int acceptor,int applicant ){
        Info information;
        information.operation = ACCEPT;
        information.hostId = acceptor;
        information.friendIds.push_back(applicant);
        getProxy(acceptor)->Report(information);
}
void FriendFinderNewComerWorkerAdapter::deny( int hostId,int guestId ){
        Info information;
        information.operation = DENY;
        information.hostId = hostId;
        information.friendIds.push_back(guestId);
        getProxy(hostId)->Report(information);
}
void FriendFinderNewComerWorkerAdapter::ignore( int userId,const MyUtil::IntSeq& ids ){
        Info information;
        information.operation = IGNORE;
        information.hostId = userId;
        information.friendIds.insert(information.friendIds.end(),ids.begin(),ids.end());
        getProxy(userId)->Report(information);
}
void FriendFinderNewComerWorkerAdapter::rebuild( int userId ){
	getProxy( userId )->Rebuild( userId );
}
void FriendFinderNewComerWorkerAdapter::addNewComer( int userId ){
        Info information;
        information.operation = ADD;
        information.hostId = userId;
        getProxy(userId)->Report(information);
}

void FriendFinderNewComerWorkerAdapter::addNewComerWithTime( int userId, int time ){
        Info information;
        information.operation = ADD;
        information.hostId = userId;
        information.friendIds.push_back(time);
        getProxy(userId)->Report(information);
}

void FriendFinderNewComerWorkerAdapter::updateInfo(int userId)
{
        Info information;
        information.operation = UPDATE;
        information.hostId = userId;
        getProxy(userId)->Report(information);
}
