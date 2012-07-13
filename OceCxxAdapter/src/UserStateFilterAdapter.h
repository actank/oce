#ifndef USERSTATEFILTER_H
#define USERSTATEFILTER_H

#include <UserStateFilter.h>
#include <AdapterI.h>
#include <bitset>

namespace xce {
namespace userstatefilter {
namespace adapter {

const int USERSTATE_FILTER_SIZE = 2147483647;

class UserStateFilterAdapter :
	public MyUtil::ReplicatedClusterAdapterI<UserStateFilterManagerPrx>,
		public MyUtil::Singleton<UserStateFilterAdapter> {
public:
	UserStateFilterAdapter() : MyUtil::ReplicatedClusterAdapterI<UserStateFilterManagerPrx>("ControllerUserStateFilter", 120, 3000) {}
	MyUtil::ByteSeq getStateStarMinMax(int minId,int maxId);
        xce::userstatefilter::Int2BoolMap getStateStarBatch(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getStarIdsFrom(const MyUtil::IntSeq& ids);
        void getAllStateStar(std::bitset<USERSTATE_FILTER_SIZE>& s);
        bool isStateStarTrue(int userId);
	MyUtil::ByteSeq getStateGuideMinMax(int minId,int maxId);
        xce::userstatefilter::Int2BoolMap getStateGuideBatch(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getGuideIdsFrom(const MyUtil::IntSeq& ids);
        void getAllStateGuide(std::bitset<USERSTATE_FILTER_SIZE>& s);
        bool isStateGuideTrue(int userId);
        void updateUserState(int userId,int state);
        void updateUserStateBatch(const MyUtil::Int2IntMap& statemap);
};

}
}
}

#endif
