#ifndef __HIGHSCHOOL_FILTERI_H__
#define __HIGHSCHOOL_FILTERI_H__

#include <HighSchoolFilter.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <TaskManager.h>

namespace xce {
namespace highschoolfilter {

const int HIGHSCHOOL_FILTER_SIZE = 2147483647;
const int TASK_LEVEL_INIT = 1;

class HighSchoolResultHandler : virtual public com::xiaonei::xce::ResultHandler {
        public:
                HighSchoolResultHandler(int& large_id, map<int, bool>& stage_highschool_result) :
                       _large_id(large_id), _stage_highschool_result(stage_highschool_result) {
                }
        protected:
                virtual bool handle(mysqlpp::Row&) const;
        private:
		int& _large_id;
                map<int, bool>& _stage_highschool_result;
};

class HighSchoolFilterManagerI: virtual public HighSchoolFilterManager,
        virtual public MyUtil::Singleton<HighSchoolFilterManagerI>/*, virtual public IceUtil::Thread */{
public:
	HighSchoolFilterManagerI();
        void initialize();

	virtual MyUtil::ByteSeq getStageHighSchoolMinMax(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual Int2BoolMap getStageHighSchoolBatch(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual MyUtil::IntSeq getStageHighSchoolIdsFrom(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual bool isStageHighSchoolTrue(::Ice::Int,
			const Ice::Current&);

	virtual void updateStageHighSchool(::Ice::Int,::Ice::Int,
			const Ice::Current&);
        virtual void updateStageHighSchoolBatch(const ::MyUtil::Int2IntMap&,
			const Ice::Current&);
private:

	std::bitset<HIGHSCHOOL_FILTER_SIZE> _user_stage_highschool;
};

//
class InitTask : virtual public MyUtil::Task {
public:
        InitTask( std::bitset<HIGHSCHOOL_FILTER_SIZE>& user_stage_highschool,
                Ice::Int level=TASK_LEVEL_INIT) :
                        MyUtil::Task(level), _user_stage_highschool(user_stage_highschool){
        }
protected:
        virtual void handle();
private:
	std::bitset<HIGHSCHOOL_FILTER_SIZE>& _user_stage_highschool;
};


}
}

#endif
