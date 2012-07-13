#include "HighSchoolFilterAdapter.h"

namespace xce {
namespace highschoolfilter {
namespace adapter {

MyUtil::ByteSeq HighSchoolFilterAdapter::getStageHighSchoolMinMax(int minId,int maxId) {
	return getProxy(0)->getStageHighSchoolMinMax(minId, maxId);
}

xce::highschoolfilter::Int2BoolMap HighSchoolFilterAdapter::getStageHighSchoolBatch(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStageHighSchoolBatch(ids);
}

MyUtil::IntSeq HighSchoolFilterAdapter::getStageHighSchoolIdsFrom(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStageHighSchoolIdsFrom(ids);
}

void HighSchoolFilterAdapter::getAllStageHighSchool(std::bitset<HIGHSCHOOL_FILTER_SIZE>& s) {
	MyUtil::ByteSeq result;
	for (int i=0;i<210;i++) {
		MyUtil::ByteSeq temp = getProxy(0)->getStageHighSchoolMinMax(i*10000000, (i+1)*10000000);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	MyUtil::ByteSeq temp=getProxy(0)->getStageHighSchoolMinMax(2100000000, 2147483647);
	result.insert(result.end(), temp.begin(), temp.end());
	for (int i=0;i<HIGHSCHOOL_FILTER_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			s.set(i);
		}
	}
}

bool HighSchoolFilterAdapter::isStageHighSchoolTrue(int userId) {
	return getProxy(0)->isStageHighSchoolTrue(userId);
}

void HighSchoolFilterAdapter::updateStageHighSchool(int userId,int stage) {
	return getProxy(0)->updateStageHighSchool(userId, stage);
}

void HighSchoolFilterAdapter::updateStageHighSchoolBatch(const MyUtil::Int2IntMap& stagemap) {
	return getProxy(0)->updateStageHighSchoolBatch(stagemap);
}

}
}
}
