#ifndef MUCTALKRECORDADAPTER_H__
#define MUCTALKRECORDADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "MucTalkRecord.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace muc {


using namespace MyUtil;

class MucTalkRecordAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::TalkChannel, MucTalkRecordAdapter> {
public:
  MucTalkRecordAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

	void RecordTalkMsg(int userid, int groupid, const string& msg);
	MucTalkMessageSeq GetRecentlyMsg(int groupid);

	MucTalkRecordManagerPrx getManager(int id);
	MucTalkRecordManagerPrx getManagerOneway(int id);
private:

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@MucTalkRecord";
  }
  virtual size_t cluster() {
    return 10;
  }
  vector<MucTalkRecordManagerPrx> _managersOneway;
  vector<MucTalkRecordManagerPrx> _managers;
};

};
};
};
};


#endif
