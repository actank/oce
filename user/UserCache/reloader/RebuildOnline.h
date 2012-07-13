/*
 * RebuildOnline.h
 *
 *  Created on: 2008-9-10
 *      Author: bbc
 */

#ifndef REBUILDONLINE_H_
#define REBUILDONLINE_H_

#include "UtilCxx/src/TaskManager.h"
#include "Util.h"

namespace xce {
namespace usercache {

const uint CACHE_SIZE = 5000;
const uint REBUILD_SIZE = 1000;
const uint WAIT_TIME = 5;
const int TASK_LEVEL_SETONLINE = 2;

class RebuildOnline: virtual public MyUtil::Singleton<RebuildOnline>,
		virtual public IceUtil::Thread {
public:
	void rebuild(int id);
	void rebuild(const MyUtil::IntSeq& ids);
	virtual void run();
private:
	friend class MyUtil::Singleton<RebuildOnline>;
	RebuildOnline();

	MyUtil::IntSeq _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class SetOnlineTask : virtual public MyUtil::Task {
public:
	SetOnlineTask(MyUtil::IntSeq& ids);
	virtual void handle();
private :
	MyUtil::IntSeq _ids;
};

}
}

#endif /* REBUILDONLINE_H_ */
