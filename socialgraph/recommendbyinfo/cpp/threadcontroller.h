#ifndef THREAD_CONTROLLER_H_
#define THREAD_CONTROLLER_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce {
namespace socialgraph {

//---------------------------------------------------------------------------------------

/*@brief load view_count*/
class ViewCountThread: virtual public IceUtil::Thread {
public:
	ViewCountThread() {}
	virtual void run();
};

//---------------------------------------------------------------------------------------

/*@brief load university info*/
class LoadUnivThread: virtual public IceUtil::Thread {
public:
	LoadUnivThread() {}
	virtual void run();
};

//---------------------------------------------------------------------------------------

/*@brief load high_school info */
class LoadHighSchoolThread: virtual public IceUtil::Thread {
public:
	LoadHighSchoolThread() {}
	virtual void run();
};

//---------------------------------------------------------------------------------------

/*@brief load junior_high_school info */
class LoadJuniorSchoolThread: virtual public IceUtil::Thread {
public:
	LoadJuniorSchoolThread() {}
	virtual void run();
};

//---------------------------------------------------------------------------------------

/*@brief load elementary_school info*/
class LoadElementarySchoolThread: virtual public IceUtil::Thread {
public:
	LoadElementarySchoolThread() {}
	virtual void run();
};

}
}

#endif
