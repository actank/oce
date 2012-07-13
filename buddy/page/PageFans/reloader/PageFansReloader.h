#ifndef __PAGEFANSRELOADER_H__
#define __PAGEFANSRELOADER_H__

#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "PageFans.h"
#include "../data/PageFansCommon.h"

namespace xce{
namespace buddy{

const static int TASK_LEVEL_RELOAD = 1;
const static int TASK_LEVEL_ADDFAN = 2;
const static int TASK_LEVEL_REMOVEFAN = 3;
const static int TASK_LEVEL_UPDATEFANWITHSTATE = 4;
const static int TASK_LEVEL_UPDATEFANSWITHSTATE = 5;
const static int TASK_LEVEL_ADDBLOCK = 6;
const static int TASK_LEVEL_REMOVEBLOCK = 7;

class PageFansReloaderI : virtual public PageFansReloader,
        virtual public MyUtil::Singleton<PageFansReloaderI> {
public:
        PageFansReloaderI(){}
		virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());
		virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current()); 
		virtual bool isValid(const Ice::Current& = ::Ice::Current());

		//-- for fans ------
		virtual ::Ice::Int addFan(const PageEntryPtr& pe, const Ice::Current& = Ice::Current());
		::Ice::Int addFanWork(const PageEntryPtr& pe, const Ice::Current& current);
		virtual ::Ice::Int removeFan(::Ice::Int pageId, ::Ice::Int userId,
						const Ice::Current& = Ice::Current());
		::Ice::Int removeFanWork(::Ice::Int pageId, ::Ice::Int userId,
						const Ice::Current& current);
		virtual ::Ice::Int updateFanWithState(const PageEntryPtr& pe, const Ice::Current& = Ice::Current());
		::Ice::Int updateFanWithStateWork(const PageEntryPtr& pe, const Ice::Current& current);
		virtual ::Ice::Int updateFansWithState(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState,
						const Ice::Current& = Ice::Current());
		::Ice::Int updateFansWithStateWork(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState,
						const Ice::Current& current);

		//----for black list ---
		virtual ::Ice::Int addBlock(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp,
						const Ice::Current& = Ice::Current());
		::Ice::Int addBlockWork(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp,
						const Ice::Current& current);
		virtual ::Ice::Int removeBlock(::Ice::Int pageId, ::Ice::Int userId,
						const Ice::Current& = Ice::Current());
		::Ice::Int removeBlockWork(::Ice::Int pageId, ::Ice::Int userId,
						const Ice::Current& current);

private:

		bool valid_;
		IceUtil::RWRecMutex validmutex_;
};

class PageFansDatabaseHelper: public MyUtil::Singleton<PageFansDatabaseHelper> {
public:
        bool insertFan(const PageEntryPtr& pe);
        bool deleteFan(::Ice::Int pageid, ::Ice::Int userid);
		bool updateFanWithState(const PageEntryPtr& pe);
		bool updateFansWithState(::Ice::Int pageId, ::Ice::Int oriState, ::Ice::Int tarState);
        
		bool insertBlock(::Ice::Int pageId, ::Ice::Int userId, ::Ice::Int operatorId, ::Ice::Int timestamp);
        bool deleteBlock(::Ice::Int pageId, ::Ice::Int userId);
private:
        string getPageFansTable(int userId);
};

class AddFanTask: virtual public MyUtil::Task {
public:
        AddFanTask(const PageEntryPtr& pe,
			const int tasklevel,
                        const Ice::Current& current) :
                MyUtil::Task(tasklevel),
		_pe(pe), 
		_current(current) {
        }
        virtual void handle();
private:
        const PageEntryPtr _pe;
		const Ice::Current _current;
};

class RemoveFanTask: virtual public MyUtil::Task {
public:
        RemoveFanTask(const int pageId, const int userId, 
			const int tasklevel,
			const Ice::Current& current ) :
                MyUtil::Task(tasklevel), 
		_pageId(pageId), 
		_userId(userId), 
		_current(current) {
        }
        virtual void handle();
private:
        const int _pageId;
	const int _userId;
	const Ice::Current _current;
};

class UpdateFanWithStateTask: virtual public MyUtil::Task {
public:
        UpdateFanWithStateTask(const PageEntryPtr& pe, 
                        const int tasklevel,
			const Ice::Current& current
                        ) :
                MyUtil::Task(tasklevel), 
		_pe(pe), 
		_current(current) {
        }
        virtual void handle();
private:
        const PageEntryPtr _pe;
        const Ice::Current _current;
};

class UpdateFansWithStateTask: virtual public MyUtil::Task {
public:
        UpdateFansWithStateTask(const int pageId,
			const int oriState,
			const int tarState,
			const int tasklevel,    
                        const Ice::Current& current
                        ) :
                MyUtil::Task(tasklevel), 
		_pageId(pageId), 
		_oriState(oriState), 
		_tarState(tarState), 
		_current(current) {
        }
        virtual void handle();
private:
        const int _pageId;
	const int _oriState;
	const int _tarState;
	const Ice::Current _current;
};

class AddBlockTask: virtual public MyUtil::Task {
public:
        AddBlockTask(const int pageId,   
                        const int userId,
                        const int operatorId,
			const int timestamp,
			const int tasklevel,
                        const Ice::Current& current
                        ) :
                MyUtil::Task(tasklevel), 
		_pageId(pageId), 
		_userId(userId), 
		_operatorId(operatorId), 
		_timestamp(timestamp), 
		_current(current) {
        }
        virtual void handle();
private:
        const int _pageId;
        const int _userId;
	const int _operatorId;
	const int _timestamp;
	const Ice::Current _current;
};

class RemoveBlockTask: virtual public MyUtil::Task {
public:
        RemoveBlockTask(const int pageId,
                        const int userId,
                        const int tasklevel,
			const Ice::Current& current
                        ) :
		MyUtil::Task(tasklevel),
                _pageId(pageId), 
		_userId(userId),  
		_current(current) {
        }
        virtual void handle();
private:
        const int _pageId;
        const int _userId;
	const Ice::Current _current;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};


}
}

#endif
