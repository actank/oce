#ifndef __PHOTOSTATE_FILTERI_H__
#define __PHOTOSTATE_FILTERI_H__

#include <PhotoStateFilter.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <TaskManager.h>

namespace xce {
namespace photostatefilter {

const int PHOTOSTATE_FILTER_SIZE = 2147483647;
const int TASK_LEVEL_INIT = 1;
//const std::string MEN_DEFAULT_PHOTO="0/0/men_main.gif";
//const std::string WOMEN_DEFAULT_PHOTO="0/0/women_main.gif";

//
class PhotoStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
        public:
                PhotoStateResultHandler(int& large_id, map<int, bool>& result) :
                       _large_id(large_id), _result(result) {
                }
        protected:
                virtual bool handle(mysqlpp::Row&) const;
        private:
		int& _large_id;
                map<int, bool>& _result;
};
//

class PhotoStateFilterManagerI: virtual public PhotoStateFilterManager,
        virtual public MyUtil::Singleton<PhotoStateFilterManagerI>/*, virtual public IceUtil::Thread */{
public:
	PhotoStateFilterManagerI();
        void initialize();

	virtual MyUtil::ByteSeq getPhotoStateMinMax(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual Int2BoolMap getPhotoStateBatch(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual MyUtil::IntSeq getPhotoIdsFrom(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual bool isPhotoStateTrue(::Ice::Int,
			const Ice::Current&);
	virtual void updatePhotoState(::Ice::Int,const std::string&,
			const Ice::Current&);
        virtual void updatePhotoStateBatch(const ::MyUtil::Int2StrMap&,
			const Ice::Current&);
private:

	std::bitset<PHOTOSTATE_FILTER_SIZE> _photo_state;

//	virtual void run();
//	xce::atomicstl::BlockingQueue<std::pair<int, int> > queue_;
};

class InitTask : virtual public MyUtil::Task {
public:
        InitTask( std::bitset<PHOTOSTATE_FILTER_SIZE>& photo_state,
                Ice::Int level=TASK_LEVEL_INIT) :
                        MyUtil::Task(level), _photo_state(photo_state){
        }
protected:
        virtual void handle();
private:
	std::bitset<PHOTOSTATE_FILTER_SIZE>& _photo_state;
};


}
}

#endif
