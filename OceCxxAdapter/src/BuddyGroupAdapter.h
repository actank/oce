#ifndef BUDDYGROUPADAPTER_H_
#define BUDDYGROUPADAPTER_H_

#include "BuddyGroup.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {

using namespace com::xiaonei::model;
using namespace std;
using namespace MyUtil;

class BuddyGroupAdapter : 
	public MyUtil::ReplicatedClusterAdapterI<BuddyGroupManagerPrx>, public MyUtil::Singleton<BuddyGroupAdapter>{
public:
	void addGroup(int host, const string& groupName);
	void renameGroup(int host, const string& oldName, const string& newName);
	void removeBuddy(int host, int buddy);
	void removeGroup(int host, const string& groupName);
	void addBuddyToGroup(int host, const string& groupName, int buddy);
	int changeBuddyGroups(int host, int buddy, const MyUtil::StrSeq& nameSeq);
	BuddiedGroupsMap getBuddiedGroupsMap(int host, int timeout = 260);

	Str2IntMap getAllGroupsBuddyCount(int host);

	MyUtil::StrSeq getGroupList(int host);
	MyUtil::IntSeq getBuddyList(int host, const string& groupName, int begin, int limit);	

	void load(int host);	
private:
	BuddyGroupAdapter():
		MyUtil::ReplicatedClusterAdapterI <BuddyGroupManagerPrx>("ControllerBuddyGroup", 120, 300){
        }
	friend class MyUtil::Singleton<BuddyGroupAdapter>; 
};

}
;
}
;
}
;
}
;

#endif /*BUDDYGROUPADAPTER_H_*/
