#ifndef _USERSTATEFILTER_ICE_
#define _USERSTATEFILTER_ICE_

#include <Util.ice>

module xce {
module userstatefilter {

	dictionary<int,bool> Int2BoolMap;
	interface UserStateFilterManager {
		MyUtil::ByteSeq getStateStarMinMax(int minId,int maxId);
		Int2BoolMap getStateStarBatch(MyUtil::IntSeq ids);
		MyUtil::IntSeq getStarIdsFrom(MyUtil::IntSeq ids);
		bool isStateStarTrue(int userId);
		MyUtil::ByteSeq getStateGuideMinMax(int minId,int maxId);
		Int2BoolMap getStateGuideBatch(MyUtil::IntSeq ids);
		MyUtil::IntSeq getGuideIdsFrom(MyUtil::IntSeq ids);
		bool isStateGuideTrue(int userId);
		void updateUserState(int userId,int state);
		void updateUserStateBatch(MyUtil::Int2IntMap statemap);
	};
};
};

#endif
