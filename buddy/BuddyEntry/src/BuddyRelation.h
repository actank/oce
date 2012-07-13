#ifndef __BUDDY_RELATION_H__
#define __BUDDY_RELATION_H__

#include <vector>
#include <set>
#include <inttypes.h>

#include "BuddyCommon.h"
#include "ObjectCacheI.h"
#include "QueryRunner.h"

namespace xce {

namespace buddy {

class BuddyRelationHoldersI : virtual public Ice::Object , virtual public IceUtil::Mutex {
public:
	BuddyRelationHoldersI(){}
//	BuddyRelationHoldersI(int, BuddyRelation,StateRecorder);
	BuddyRelationHoldersI(BuddyRelationHoldersPtr);
	virtual ~BuddyRelationHoldersI();

	BuddyRelationHoldersPtr getBuddyRelationHolders() {return _holdersptr;}	
	void addApply(const int to);
	void removeApply(const int to);

	void addApplied(const int to);
	void removeApplied(const int to);

        void addFriend(const int to);
	void removeFriend(const int to);

	void addBlock(const int to);
	void removeBlock(const int to);

	void addBlocked(const int to);
	void removeBlocked(const int to);
	
	void addBiBlock(const int to);
	void removeBiBlock(const int to);
	
	BuddyDesc getDesc(const int to);
	MyUtil::IntSeq getBuddyList(const BuddyDesc& desc);
//	BuddyRelationPtr getAll();

	int getCount(const int to, BuddyDesc& desc);

	StateRecorder getStateRecorder(){
		return _holdersptr->hsr;
	}
	void setStateRecorder(const StateRecorder& sr);
	bool checkStateRecorder(const StateRecorder& sr);
private:
	void addSeq(const int, const std::string, MyUtil::IntSeq&);
	void removeSeq(const int, const std::string, MyUtil::IntSeq&);
	MyUtil::IntSeq::const_iterator findSeq(const int, const MyUtil::IntSeq&);
	int getCount(const int to,const MyUtil::IntSeq&);

private:	
	BuddyRelationHoldersPtr _holdersptr; 	
};
typedef IceUtil::Handle<BuddyRelationHoldersI> BuddyRelationHoldersIPtr;

}
}


#endif//__BUDDY_RELATION_H__
