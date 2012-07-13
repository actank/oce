#include "AntispamRelationAdapter.h"

using namespace xce::antispamrelation::adapter;

void AntispamRelationAdapter::initialize() {
//	_managersOneway = locate<AntispamRelationManagerPrx>("M", MyUtil::ONE_WAY);
//	_managers = locate<AntispamRelationManagerPrx>("M", MyUtil::TWO_WAY);
}

void AntispamRelationAdapter::notify(MyUtil::Str2StrMap map) {
	getRelationMonitor(0)->notify(map);
}

RelationMonitorPrx AntispamRelationAdapter::getRelationMonitor(int id) {
	return locate<RelationMonitorPrx>(_managers, "Anti_Spam_RelationMonitor", id, MyUtil::TWO_WAY);
}

RelationMonitorPrx AntispamRelationAdapter::getRelationMonitorOneway(int id) {
	return locate<RelationMonitorPrx>(_managersOneway, "Anti_Spam_RelationMonitor", id, MyUtil::ONE_WAY);
}


/***********************************************************************************/
