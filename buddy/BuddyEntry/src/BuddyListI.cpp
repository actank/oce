#include "BuddyListI.h"

namespace xce {
namespace buddy {

using namespace std;

BuddyListI::BuddyListI(BuddyListPtr& blptr){
	_blptr = blptr;
}

BuddyListI::BuddyListI(BuddyListI& bl){
	if(_blptr){
		setIndex(_blptr->hindex); 
		setLittleHash(_blptr->hlittlehash);
		setBuddyRelation(_blptr->hbr);	
	}
}

BuddyListI::~BuddyListI() {
}

BuddyDesc BuddyListI::getDesc(uint32_t id, unsigned long long& hit, unsigned long long& total, IceUtil::Mutex& mutex){
	{
		IceUtil::Mutex::Lock lock(mutex);
		total++;
		if(!BuddyListHelper::instance().findHash(id,_blptr->hlittlehash,(unsigned char)_blptr->hindex)){
			hit++;
			return NoPath;
		}
	}	
	IceUtil::Mutex::Lock lock(*this);
	MyUtil::IntSeq::const_iterator it;

        {
                it = findSeq(id, _blptr->hbr->apply);
                if( it != _blptr->hbr->apply.end())
        	        return Apply;
        }
        {
                it = findSeq(id, _blptr->hbr->applied);
                if( it != _blptr->hbr->applied.end())
                        return Applied;
        }
        {
                it = findSeq(id, _blptr->hbr->friends);
                if( it != _blptr->hbr->friends.end())
                        return Friend;
        }
        {
                it = findSeq(id, _blptr->hbr->block);
                if( it != _blptr->hbr->block.end())
                        return Block;
        }
        {
                it = findSeq(id, _blptr->hbr->blocked);
                if( it != _blptr->hbr->blocked.end())
                        return Blocked;
        }
        {
                it = findSeq(id, _blptr->hbr->biblock);
                if( it != _blptr->hbr->biblock.end())
                        return BiBlock;
        }
        return NoPath;
}

bool BuddyListI::isFriend(uint32_t id){
	IceUtil::Mutex::Lock lock(*this);
        if(!BuddyListHelper::instance().findHash(id,_blptr->hlittlehash,(unsigned char)_blptr->hindex))
                return false;
        MyUtil::IntSeq::const_iterator it;
	{
                it = findSeq(id, _blptr->hbr->friends);
                if( it != _blptr->hbr->friends.end())
                        return true;
        }
	return false;
}

void BuddyListI::setBuddyRelation(BuddyRelationPtr& brp){
        IceUtil::Mutex::Lock lock(*this);
        _blptr->hbr->friends.reserve(brp->friends.size());
	_blptr->hbr->friends = brp->friends;

	_blptr->hbr->apply.reserve(brp->apply.size());
	_blptr->hbr->apply = brp->apply;

        _blptr->hbr->applied.reserve(brp->applied.size());
	_blptr->hbr->applied = brp->applied;

	_blptr->hbr->block.reserve(brp->block.size());
 	_blptr->hbr->block = brp->block;

	_blptr->hbr->blocked.reserve(brp->blocked.size());
	_blptr->hbr->blocked = brp->blocked;

	_blptr->hbr->biblock.reserve(brp->biblock.size());
	_blptr->hbr->biblock = brp->biblock;
}

MyUtil::IntSeq::const_iterator BuddyListI::findSeq( const int to,const MyUtil::IntSeq& seq){
        MCE_TRACE(" find "<< to <<" in BuddyListI");
        MyUtil::IntSeq::const_iterator it = seq.end();;
        if(!seq.empty()){
                it = lower_bound(seq.begin(),seq.end(), to);
                if(it != seq.end() && *it != to){
                        it = seq.end();
                }
        }
        return it;
}

}
}
