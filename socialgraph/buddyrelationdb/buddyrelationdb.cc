#include "socialgraph/buddyrelationdb/buddyrelationdb.h"

#include "socialgraph/socialgraphutil/calculator/component/readfriendlistfromdisk.h"

namespace xce {
namespace socialgraph {

BuddyRelationDb::BuddyRelationDb() {
}

BuddyRelationDb::~BuddyRelationDb() {
}

void BuddyRelationDb::Detail() {
	ReadFriendListFromDisk* instance = new ReadFriendListFromDisk("/data/xce/Project/buddyrelationdb");
	SetComponent(0, instance);
	LoadComponent(0);
}

}
}
