#ifndef __BUDDY_COMMON_H__
#define __BUDDY_COMMON_H__
#include <vector>
#include <string>

namespace xce {
namespace buddy {
namespace common {

template<class Prx>
class BuddyCheckHelper {
public:
	BuddyCheckHelper(size_t n, const std::string& name, const std::vector<std::vector<Prx> > & prxs) {
		numCluster_sizeT_ = n;
		name_ = name;
		prxs_ = prxs;
	}
	
	MyUtil::IntSeq checkFriendCounts(const MyUtil::IntSeq& ids) {
		if (ids.empty()){
			return MyUtil::IntSeq();
		}
		int numCluster = (int)numCluster_sizeT_;
		if (numCluster < 0) {
			MCE_WARN("[" << name_ << "::checkFriendCounts] this should not be seen! Why cluster="
					<< numCluster_sizeT_ << " ???" );
			return MyUtil::IntSeq();
		}
		if (numCluster == 0 || numCluster == 1) {
			return compareReplica(0,ids);
		} else {
			vector< vector<int> > seperatedIds;
			seperatedIds.resize(numCluster);
			for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
				seperatedIds.at(*it % numCluster).push_back(*it);
			}
			MyUtil::IntSeq result;
			for (int posCluster = 0; posCluster < numCluster; ++posCluster) {
				MyUtil::IntSeq someResult = compareReplica(posCluster,seperatedIds.at(posCluster));
				if(!someResult.empty()){
					result.insert( result.end(),someResult.begin(), someResult.end() );
				}
			}
			return result;
		}
	}
private:
	MyUtil::IntSeq compareReplica(int mod, const MyUtil::IntSeq& ids) {
		if(ids.empty()){
			return MyUtil::IntSeq();
		}
		MyUtil::IntSeq result;

		MyUtil::Int2IntMap standerdResult;
		for(typename std::vector<Prx>::const_iterator itPrx = prxs_[mod].begin(); itPrx != prxs_[mod].end(); ++itPrx ){
			if(standerdResult.empty()){
				try{
					standerdResult = (*itPrx)->getFriendCounts(ids);
				} catch (...) {
					MCE_WARN( "[" << name_ << "::getFriendCounts] from Proxy(" << mod << ") handle unknown exception." );
				}	
			}else{
				MyUtil::Int2IntMap someResult;
				try{
					someResult = (*itPrx)->getFriendCounts(ids);
				} catch (...) { 
					MCE_WARN( "[" << name_ << "::getFriendCounts] from Proxy(" << mod << ") handle unknown exception." );
				}
				compareResult(standerdResult,someResult,result);
			}
		}
		return result;
	}
	void compareResult(const MyUtil::Int2IntMap& standerd,  
			const MyUtil::Int2IntMap& compare, MyUtil::IntSeq& result){
		if(standerd != compare){
			if(standerd.size() < compare.size()) {
				compareResult(compare, standerd, result);
			} else {
				for(MyUtil::Int2IntMap::const_iterator it = standerd.begin(), end = standerd.end(), ftend = compare.end(); it != end; ++it) {
					MyUtil::Int2IntMap::const_iterator ft = compare.find(it->first);
					if(ft == ftend || ft->second != it->second) {
						result.push_back(it->first);
					}
				}
			}
		}
	}
	std::vector<std::vector<Prx> > prxs_;
	size_t numCluster_sizeT_;
	std::string name_;
};
}
}
}
#endif
