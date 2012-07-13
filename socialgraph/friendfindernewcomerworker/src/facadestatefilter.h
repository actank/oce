#ifndef __FACADESTATE_FILTERI_H__
#define __FACADESTATE_FILTERI_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ServiceI.h>
#include <QueryRunner.h>

namespace xce {
namespace socialgraph {
namespace friendfindernewcomer {

const int FACADESTATE_FILTER_SIZE = 2147483647;

class FacadeStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
        public:
                FacadeStateResultHandler(int& large_id, map<int, bool>& star_result ) :
                       _large_id(large_id), _star_result(star_result) {
                }
        protected:
                virtual bool handle(mysqlpp::Row&) const;
        private:
		int& _large_id;
                map<int, bool>& _star_result;
};

class FacadeStateFilter: 
        public MyUtil::Singleton<FacadeStateFilter>/*, public IceUtil::Thread */{
public:
	bool isFacadeStateTrue(int);

	void set(int);
	void reset(int);
private:
	std::bitset<FACADESTATE_FILTER_SIZE> _facade_state;
public:
	FacadeStateFilter();
	void initialize();
        bool isValid();
private:
	bool _inited_flag;
	IceUtil::RWRecMutex _inited_flag_mutex;

};

class FacadeStateFilterInitThread: public IceUtil::Thread {
public:
	virtual void run(){
		FacadeStateFilter::instance().initialize();
	};
};
typedef IceUtil::Handle<FacadeStateFilterInitThread> FacadeStateFilterInitThreadPtr;

}
}
}

#endif
