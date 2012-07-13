#ifndef _BUDDYLIST_H_
#define _BUDDYLIST_H_

#include <inttypes.h>
#include <iostream>
#include <Ice/Ice.h>
#include "BuddyCommon.h"
#include "BuddyRelation.h"

namespace xce {
namespace buddy {

static const uint32_t prime_array[] =
{
        29,//0-10
	71,//10-30
	131,//30-50
	431,//50-200
	761,//200-350
	1103,//350-500
};
//7833827个人抽样统计
        //0-0 105944 0.013523913663143186
        //1-1 138046 0.017621783069756328
        //2-3 138894 0.01773003156694678
        //4-10 293284 0.03743815123821346
        //11-30 656969 0.08386309781923956
        //31-50 647329 0.08263253707287639
        //51-100 1642336 0.20964670269077937
        //101-150 1436856 0.18341686636684726
        //151-200 1005119 0.12830497788628725
        //201-250 632712 0.08076665466316782
        //251-300 388552 0.04959925717021833
        //301-350 240083 0.03064696220633925
        //351-400 152009 0.019404181379037346
        //401-500 165307 0.02110169142106406
        //501-700 119024 0.015193595671694052
        //701-1000 65422 0.008351218376407853
        //1001-2000 5315 6.784678803859213E-4
        //2001-65536 66 8.425000960577761E-6
class BuddyListHelper:virtual public MyUtil::Singleton<BuddyListHelper>{
public:
	unsigned char getPrimeNum(const uint32_t& num){
        	if(num>50){
                	if(num<200)
                        	return 3;
                	else if (num < 350)
                        	return 4;
                	else if (num < 500)
                        	return 5;
                	else
                        	return 0xFF;
        	}else{
                	if(num > 30)
                        	return 2;
                	else if (num > 10)
                        	return 1;
                	else
                        	return 0;
        	}
	}

	void setBit(const uint32_t& id , MyUtil::ByteSeq& littlehash, const unsigned char& index){
		uint32_t offset = id % prime_array[index];
        	switch( offset % 8){
                	case 0:{
                        	littlehash.at(offset/8) |=  0x01;
                	}break;
                	case 1:{
				littlehash.at(offset/8) |=  0x02;
			}break;
			case 2:{
				littlehash.at(offset/8) |=  0x04;
			}break;
			case 3:{
				littlehash.at(offset/8) |=  0x08;
			}break;
			case 4:{
				littlehash.at(offset/8) |=  0x10;
			}break;
			case 5:{
				littlehash.at(offset/8) |=  0x20;
			}break;
			case 6:{
				littlehash.at(offset/8) |=  0x40;
			}break;
			case 7:{
				littlehash.at(offset/8) |=  0x80;
			}break;
			default:
			break;
		}
	}
	void initHash(const BuddyRelationPtr& brptr, MyUtil::ByteSeq& littlehash, unsigned char& index ){
		if( !brptr){
			MCE_WARN("[initLittleHash] BuddyList is NULL object");
			return;
		}
		index = getPrimeNum(brptr->friends.size());
		if(0xFF == index){
			return;
		}
		littlehash.reserve((int)(prime_array[index]/8+1));
		littlehash.resize((int)(prime_array[index]/8+1));	

		for ( MyUtil::IntSeq::const_iterator friendsIt = brptr->friends.begin();
				friendsIt!= brptr->friends.end(); ++friendsIt ){
			setBit(*friendsIt,littlehash,index);
		}
		for ( MyUtil::IntSeq::const_iterator applyIt = brptr->apply.begin();
				applyIt!= brptr->apply.end(); ++applyIt ){
			setBit(*applyIt,littlehash,index);
		}
		for ( MyUtil::IntSeq::const_iterator appliedIt = brptr->applied.begin();
				appliedIt!= brptr->applied.end(); ++appliedIt ){
			setBit(*appliedIt,littlehash,index);
		}
		for ( MyUtil::IntSeq::const_iterator blockIt = brptr->block.begin();
				blockIt!= brptr->block.end(); ++blockIt ){
			setBit(*blockIt,littlehash,index);
		}
		for ( MyUtil::IntSeq::const_iterator blockedIt = brptr->blocked.begin();
				blockedIt!= brptr->blocked.end(); ++blockedIt ){
			setBit(*blockedIt,littlehash,index);
		}
		for ( MyUtil::IntSeq::const_iterator biblockIt = brptr->biblock.begin();
                        biblockIt!= brptr->biblock.end(); ++biblockIt ){
			setBit(*biblockIt,littlehash,index);
		}
	}

	bool findHash(const uint32_t& id, const MyUtil::ByteSeq& littlehash, const unsigned char& index){
		if(littlehash.empty() || 0xFF == index){
			return false;
		}
		uint32_t offset = id % prime_array[index];
		switch(offset % 8){
			case 0:{
				if( littlehash.at(offset/8) & 0x01)
					return true;
			}break;
			case 1:{
				if( littlehash.at(offset/8) & 0x02 )
					return true;
			}break;
			case 2:{
				if( littlehash.at(offset/8) & 0x04 )
					return true;
			}break;
			case 3:{
				if( littlehash.at(offset/8) & 0x08 )
					return true;
			}break;
			case 4:{
				if( littlehash.at(offset/8) & 0x10 )
                                return true;
			}break;
			case 5:{
				if( littlehash.at(offset/8) & 0x20 )
					return true;
			}break;
			case 6:{
				if( littlehash.at(offset/8) & 0x40 )
					return true;
			}break;
			case 7:{
				if( littlehash.at(offset/8) & 0x80 )
					return true;
			}break;
			default:
			break;
		}
		return false;
	}

private:
	friend class MyUtil::Singleton<BuddyListHelper>;
	BuddyListHelper(){
                ;
        }
};
//BuddyList没有声明到slice, 未找到slice里char*合适声明方法
//定义vector的话，浪费3个字节空间，_cash本身也不会很大，得不偿失
class BuddyListI : virtual public Ice::Object,virtual public IceUtil::Mutex {
public:
	BuddyListI(){
		;
	}

	BuddyListI(BuddyListPtr& blptr);
	BuddyListI(BuddyListI& blptr);	

	virtual ~BuddyListI(); 

	BuddyDesc getDesc(uint32_t id,unsigned long long& hit,unsigned long long& total,IceUtil::Mutex& mutex);
	bool isFriend(uint32_t id);

	//BuddyRelationPtr& getBuddyRelation() {return hbr;}
	//vector<unsigned char> getLittleHash() {return hlittlehash;}
	//unsigned char getIndex() {return hindex;}

private:
	//7833827个人抽样统计
	//0-0 105944 0.013523913663143186
	//1-1 138046 0.017621783069756328
	//2-3 138894 0.01773003156694678
	//4-10 293284 0.03743815123821346
	//11-30 656969 0.08386309781923956
	//31-50 647329 0.08263253707287639
	//51-100 1642336 0.20964670269077937
	//101-150 1436856 0.18341686636684726
	//151-200 1005119 0.12830497788628725
	//201-250 632712 0.08076665466316782
	//251-300 388552 0.04959925717021833
	//301-350 240083 0.03064696220633925
	//351-400 152009 0.019404181379037346
	//401-500 165307 0.02110169142106406
	//501-700 119024 0.015193595671694052
	//701-1000 65422 0.008351218376407853
	//1001-2000 5315 6.784678803859213E-4
	//2001-65536 66 8.425000960577761E-6


	void setBuddyRelation(BuddyRelationPtr& brp);
	
	MyUtil::IntSeq::const_iterator findSeq( const int to,const MyUtil::IntSeq& seq);

	void setLittleHash(MyUtil::ByteSeq littlehash){
		IceUtil::Mutex::Lock lock(*this);
		_blptr->hlittlehash.reserve(littlehash.size());
		_blptr->hlittlehash = littlehash;
	}
        void setIndex(unsigned char index) { 
		_blptr->hindex = index;
	}
private:
	BuddyListPtr _blptr;
};

typedef IceUtil::Handle<BuddyListI> BuddyListIPtr;

}
}

#endif
