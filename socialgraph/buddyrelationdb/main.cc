#include <iostream>

#include "socialgraph/socialgraphutil/calculator/component/readfriendlistfromdisk.h"

#include "socialgraph/buddyrelationdb/buddyrelationdb.h"

int main(int argc, char* argv[]) {
	xce::socialgraph::BuddyRelationDb buddyrelation;
	buddyrelation.Calc();
	//xce::socialgraph::ReadFriendListFromDisk* t = static_cast<xce::socialgraph::ReadFriendListFromDisk*>(buddyrelation.Get(0));
	//std::vector<int> list;
	//t->GetFriendList(238111132, list);
	//xce::PRINT_LIST(list);
}
