#ifndef OCCUPANTSBACKUP_H_
#define OCCUPANTSBACKUP_H_

#include "OccupantsBackup.h"
#include "TalkCommon.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace occupants {
namespace adapter {

using namespace com::xiaonei::talk::common;
using namespace ::talk::occupants;
using namespace MyUtil;

class OccupantsBackupAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, OccupantsBackupAdapter> {
public:
	
	void put(const OccupantPtr& o);
	void puts(const OccupantSeq& oseq);
	void remove(const RoomIdPtr& rid, const JidPtr& jid);
	void removeAll(const RoomIdPtr& rid);
	OccupantSeq get(const RoomIdPtr& rid);

protected:
	virtual string name() {
		return "OccupantsBackup";
	}
	virtual string endpoints() {
		return "@OccupantsBackup";
	}
	virtual size_t cluster() {
		return 0;
	}

	BackupManagerPrx getManager(int timeout = 260);
	BackupManagerPrx getManagerOneway();

	BackupManagerPrx _managerOneway;
	BackupManagerPrx _manager;

};

}
;
}
;
}
;
#endif /*OCCUPANTSBACKUP_H_*/
