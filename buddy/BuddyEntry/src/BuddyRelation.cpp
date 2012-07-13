#include "BuddyRelation.h"
#include "ServiceI.h" // This is only for using LOG.
#include "QueryRunner.h"


using namespace std;
using namespace xce::buddy;
using namespace com::xiaonei::xce;

namespace xce {
namespace buddy {
/*
BuddyRelationHoldersI::BuddyRelationHoldersI(uint32_t from, BuddyRelation br, StateRecorder sr){
	hfrom = from;
	hbr = br;
	hsr = sr;
}*/

BuddyRelationHoldersI::BuddyRelationHoldersI(BuddyRelationHoldersPtr brhptr):
	_holdersptr(brhptr){
}

BuddyRelationHoldersI::~BuddyRelationHoldersI() {
}


void BuddyRelationHoldersI::addSeq( const int to, const string desc, MyUtil::IntSeq& seq ) {
	MCE_TRACE(desc << to);
        IceUtil::Mutex::Lock lock(*this);
        MyUtil::IntSeq::iterator it = lower_bound(seq.begin(),seq.end(), to);
        if (it != seq.end() && *it == to){
                MCE_TRACE("Already add " << *it);
        } else {
                MCE_TRACE("insert " << *it);
                seq.insert(it,to);
        }
}

void BuddyRelationHoldersI::removeSeq( const int to, const string desc, MyUtil::IntSeq& seq ) {
        MCE_TRACE(desc << to);
	if(seq.empty()){
		return;
	}
        IceUtil::Mutex::Lock lock(*this);
        MyUtil::IntSeq::iterator it = lower_bound(seq.begin(),seq.end(), to);
	while (it != seq.end()){
		if(*it == to)
		{
			MCE_TRACE("erase " << *it);
			it = seq.erase(it);
			continue;
		}
		if(*it > to)
			break;
		it++;
	}
}

MyUtil::IntSeq::const_iterator BuddyRelationHoldersI::findSeq( const int to,const MyUtil::IntSeq& seq){
	MCE_TRACE(" find "<< to <<" in BuddyRelationHoldersI");
	MyUtil::IntSeq::const_iterator it = seq.end();;
	if(!seq.empty()){
		it = lower_bound(seq.begin(),seq.end(), to);
		if(it != seq.end() && *it != to){
			it = seq.end();
		}
	}
	return it;	
}

void BuddyRelationHoldersI::addApply(const int to) {
	addSeq(to,"BuddyRelation::addApply ",_holdersptr->hbr->apply);
}

void BuddyRelationHoldersI::removeApply(const int to) {
        removeSeq(to,"BuddyRelation::removeApply ",_holdersptr->hbr->apply);
}

void BuddyRelationHoldersI::addApplied(const int to) {
        addSeq(to,"BuddyRelation::addApplied ",_holdersptr->hbr->applied);
}

void BuddyRelationHoldersI::removeApplied(const int to) {
        removeSeq(to,"BuddyRelation::removeApplied ",_holdersptr->hbr->applied);
}

void BuddyRelationHoldersI::addFriend(const int to) {
        addSeq(to,"BuddyRelation::addFriend ",_holdersptr->hbr->friends );
}

void BuddyRelationHoldersI::removeFriend(const int to) {
        removeSeq(to,"BuddyRelation::removeFriend ",_holdersptr->hbr->friends );
}

void BuddyRelationHoldersI::addBlock(const int to) {
        addSeq(to,"BuddyRelation::addBlock ",_holdersptr->hbr->block );
}

void BuddyRelationHoldersI::removeBlock(const int to) {
        removeSeq(to,"BuddyRelation::removeBlock ",_holdersptr->hbr->block );
}

void BuddyRelationHoldersI::addBlocked(const int to) {
        addSeq(to,"BuddyRelation::addBlocked ",_holdersptr->hbr->blocked );
}

void BuddyRelationHoldersI::removeBlocked(const int to) {
        removeSeq(to,"BuddyRelation::removeBlocked ", _holdersptr->hbr->blocked );
}

void BuddyRelationHoldersI::addBiBlock(const int to) {
        addSeq(to,"BuddyRelation::addBiblock ", _holdersptr->hbr->biblock );
}

void BuddyRelationHoldersI::removeBiBlock(const int to) {
	removeSeq(to,"BuddyRelation::removeBiblock ", _holdersptr->hbr->biblock );
}

BuddyDesc BuddyRelationHoldersI::getDesc(const int to){
	IceUtil::Mutex::Lock lock(*this);

	MyUtil::IntSeq::const_iterator it;

	{
		it = findSeq(to, _holdersptr->hbr->apply);
		if( it != _holdersptr->hbr->apply.end())
			return Apply;
	}
	{
		it = findSeq(to, _holdersptr->hbr->applied);
                if( it != _holdersptr->hbr->applied.end())
                        return Applied;
	}	
	{
		it = findSeq(to, _holdersptr->hbr->friends);
                if( it != _holdersptr->hbr->friends.end())
                        return Friend;
	}
	{
		it = findSeq(to,_holdersptr->hbr->block);
                if( it != _holdersptr->hbr->block.end())
                        return Block;
	}
	{
		it = findSeq(to,_holdersptr->hbr->blocked);
                if( it != _holdersptr->hbr->blocked.end())
                        return Blocked;
	}
	{
		it = findSeq(to,_holdersptr->hbr->biblock);
                if( it != _holdersptr->hbr->biblock.end())
                        return BiBlock;
	}
	return NoPath;
}

MyUtil::IntSeq BuddyRelationHoldersI::getBuddyList(const BuddyDesc& desc){
	MyUtil::IntSeq seq;
	switch(desc){
		case Apply:
			{
				return _holdersptr->hbr->apply;
			}
		case Applied:
			{
				return _holdersptr->hbr->applied;
			}
		case Friend:
			{
				return _holdersptr->hbr->friends;
			}
		case Block:
			{
				return _holdersptr->hbr->block;
			}
		case Blocked:
			{
				return _holdersptr->hbr->blocked;
			}
		case BiBlock:
			{
				return _holdersptr->hbr->biblock;
			}
		default:
			MCE_WARN("BuddyRelationHoldersI::getBuddyList: get the list doesn't exist! ");
	}

	return seq;	
}
/*
BuddyRelationPtr BuddyRelationHoldersI::getAll(){
	return _brptr;
}
*/
void BuddyRelationHoldersI::setStateRecorder(const StateRecorder& sr){
	_holdersptr->hsr=sr;
}

bool BuddyRelationHoldersI::checkStateRecorder(const StateRecorder& sr){
	if( (_holdersptr->hsr.lastid == 0) ||
            (_holdersptr->hsr.lastid == sr.lastid && _holdersptr->hsr.laststate == sr.laststate)){
                return true;
        }
	return false;
}

int BuddyRelationHoldersI::getCount(const int to,const MyUtil::IntSeq& seq){
	int i = 0;
        MyUtil::IntSeq::const_iterator it = findSeq(to,seq);
        while ( it != seq.end()){
                if(*it == to){
                	i++;
                	it++;
                	continue;
                }
                if(*it > to)
                        break;
                it++;
        }
	return i;
}

int BuddyRelationHoldersI::getCount(const int to, BuddyDesc& desc){
	int i = 0, j[8] = {0};
	if(NoPath == desc){	
		j[Apply]=getCount(to,_holdersptr->hbr->apply);		
		j[Applied]=getCount(to,_holdersptr->hbr->applied);
		j[Friend]=getCount(to,_holdersptr->hbr->friends);
		j[Block]=getCount(to,_holdersptr->hbr->block);
		j[Blocked]=getCount(to,_holdersptr->hbr->blocked);
		j[BiBlock]=getCount(to,_holdersptr->hbr->biblock);
		for( int k= Friend; k<=BiBlock; k++ ){
			i+=j[k];
			if( 0 != j[k])
				desc = (BuddyDesc)k;
		}
		
	}
	else{
		switch(desc)
		{
			case Apply:
				i=getCount(to,_holdersptr->hbr->apply);
				break;
			case Applied:
				i=getCount(to,_holdersptr->hbr->applied);
				break;
			case Friend:
				i=getCount(to,_holdersptr->hbr->friends);
				break;
			case Block:
				i=getCount(to,_holdersptr->hbr->block);
				break;
			case Blocked:
				i=getCount(to,_holdersptr->hbr->blocked);
				break;
			case BiBlock:
				i=getCount(to,_holdersptr->hbr->biblock);
				break;
			default:
				MCE_WARN("BuddyRelationHoldersI::getCount : get the list count doesn't exist! ");
				
		}
	}
	return i;
}

}
}
