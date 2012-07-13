#ifndef __USERSTATE_FILTERI_H__
#define __USERSTATE_FILTERI_H__

#include <UserStateFilter.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <TaskManager.h>

namespace xce {
namespace userstatefilter {

const int USERSTATE_FILTER_SIZE = 2147483647;
const int TASK_LEVEL_INIT = 1;

class UserStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
        public:
                UserStateResultHandler(int& large_id, map<int, bool>& star_result, map<int, bool>& guide_result) :
                       _large_id(large_id), _star_result(star_result), _guide_result(guide_result) {
                }
        protected:
                virtual bool handle(mysqlpp::Row&) const;
        private:
		int& _large_id;
                map<int, bool>& _star_result;
                map<int, bool>& _guide_result;
};

class UserStateFilterManagerI: virtual public UserStateFilterManager,
        virtual public MyUtil::Singleton<UserStateFilterManagerI>/*, virtual public IceUtil::Thread */{
public:
	UserStateFilterManagerI();
        void initialize();

	virtual MyUtil::ByteSeq getStateStarMinMax(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual Int2BoolMap getStateStarBatch(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual MyUtil::IntSeq getStarIdsFrom(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual bool isStateStarTrue(::Ice::Int,
			const Ice::Current&);

	virtual MyUtil::ByteSeq getStateGuideMinMax(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual Int2BoolMap getStateGuideBatch(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual MyUtil::IntSeq getGuideIdsFrom(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual bool isStateGuideTrue(::Ice::Int,
			const Ice::Current&);

	virtual void updateUserState(::Ice::Int,::Ice::Int,
			const Ice::Current&);
        virtual void updateUserStateBatch(const ::MyUtil::Int2IntMap&,
			const Ice::Current&);
private:

	std::bitset<USERSTATE_FILTER_SIZE> _user_state_star;
	std::bitset<USERSTATE_FILTER_SIZE> _user_state_guide;
};

//
class InitTask : virtual public MyUtil::Task {
public:
        InitTask( std::bitset<USERSTATE_FILTER_SIZE>& user_state_star,
        	std::bitset<USERSTATE_FILTER_SIZE>& user_state_guide,
                Ice::Int level=TASK_LEVEL_INIT) :
                        MyUtil::Task(level), _user_state_star(user_state_star), _user_state_guide(user_state_guide){
        }
protected:
        virtual void handle();
private:
	std::bitset<USERSTATE_FILTER_SIZE>& _user_state_star;
	std::bitset<USERSTATE_FILTER_SIZE>& _user_state_guide;
};


}
}

#endif
