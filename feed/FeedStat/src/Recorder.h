#ifndef __FEEDSTAT_RECORDER_H__
#define __FEEDSTAT_RECORDER_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "TaskManager.h"
#include "QueryRunner.h"
namespace xce{
namespace feed{
using namespace MyUtil;
using namespace com::xiaonei::xce;

struct ObjectCount{
	ObjectCount(){
		dispatchCount = 0;
		replyCount = 0;
		deliverCount = 0;
	}
	int dispatchCount;
	int replyCount;
	int deliverCount;
};

const int INTERVAL = 60 * 1000;
const string DB_INSTANCE = "im_stat";
const string DB_TABLE = "feed_record";

class Recorder : public Timer{
public:
	Recorder():Timer(INTERVAL, 1){
	}
	virtual void handle();
	void addDispatch(int stype);
	void addReply(int stype);
	void addDeliver(int stype, int count);
private:
	typedef map<int, ObjectCount> stype2Object;
	stype2Object _data;
	IceUtil::Mutex _mutex;
};
typedef IceUtil::Handle<Recorder> RecorderPtr;
};
};
#endif
