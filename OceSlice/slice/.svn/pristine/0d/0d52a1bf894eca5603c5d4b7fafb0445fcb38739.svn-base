#ifndef _HIGHSCHOOLFILTER_ICE_
#define _HIGHSCHOOLFILTER_ICE_

#include <Util.ice>

module xce {
module highschoolfilter {

	dictionary<int,bool> Int2BoolMap;
	interface HighSchoolFilterManager {
		MyUtil::ByteSeq getStageHighSchoolMinMax(int minId,int maxId);
		Int2BoolMap getStageHighSchoolBatch(MyUtil::IntSeq ids);
		MyUtil::IntSeq getStageHighSchoolIdsFrom(MyUtil::IntSeq ids);
		bool isStageHighSchoolTrue(int userId);
		void updateStageHighSchool(int userId,int stage);
		void updateStageHighSchoolBatch(MyUtil::Int2IntMap stagemap);
	};
};
};

#endif
