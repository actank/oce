#ifndef HIGHSCHOOLFILTER_H
#define HIGHSCHOOLFILTER_H

#include <HighSchoolFilter.h>
#include <AdapterI.h>
#include <bitset>

namespace xce {
namespace highschoolfilter {
namespace adapter {

const int HIGHSCHOOL_FILTER_SIZE = 2147483647;

class HighSchoolFilterAdapter :
	public MyUtil::ReplicatedClusterAdapterI<HighSchoolFilterManagerPrx>,
		public MyUtil::Singleton<HighSchoolFilterAdapter> {
public:
	HighSchoolFilterAdapter() : MyUtil::ReplicatedClusterAdapterI<HighSchoolFilterManagerPrx>("ControllerHighSchoolFilter", 120, 3000) {}
	MyUtil::ByteSeq getStageHighSchoolMinMax(int minId,int maxId);
        xce::highschoolfilter::Int2BoolMap getStageHighSchoolBatch(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getStageHighSchoolIdsFrom(const MyUtil::IntSeq& ids);
        void getAllStageHighSchool(std::bitset<HIGHSCHOOL_FILTER_SIZE>& s);
        bool isStageHighSchoolTrue(int userId);
        void updateStageHighSchool(int userId,int stage);
        void updateStageHighSchoolBatch(const MyUtil::Int2IntMap& stagemap);
};

}
}
}

#endif
