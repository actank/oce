#include "OccupantsBackupAdapter.h"
#include "XmppTools.h"

using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace talk::occupants;
using namespace talk::occupants::adapter;
using namespace MyUtil;

BackupManagerPrx OccupantsBackupAdapter::getManager(int timeout) {
	if (!_manager) {
		_manager = locate<BackupManagerPrx>("M", TWO_WAY);
	}
	if (timeout != 260) {
		return BackupManagerPrx::uncheckedCast(_manager->ice_timeout(timeout));
	} else {
		return _manager;
	}
}

BackupManagerPrx OccupantsBackupAdapter::getManagerOneway() {
	if (!_managerOneway) {
		_managerOneway = locate<BackupManagerPrx>("M", ONE_WAY);
	}
	return _managerOneway;
}

void OccupantsBackupAdapter::put(const OccupantPtr& o) {
	getManagerOneway()->put(o);
}
void OccupantsBackupAdapter::puts(const OccupantSeq& oseq){
	getManagerOneway()->puts(oseq);
}
void OccupantsBackupAdapter::remove(const RoomIdPtr& rid,const JidPtr& jid) {
	getManagerOneway()->remove(rid,jid);
}
void OccupantsBackupAdapter::removeAll(const RoomIdPtr& rid){
	getManagerOneway()->removeAll(rid);
}
OccupantSeq OccupantsBackupAdapter::get(const RoomIdPtr& rid) {
	return getManager(1000)->get(rid);
}
