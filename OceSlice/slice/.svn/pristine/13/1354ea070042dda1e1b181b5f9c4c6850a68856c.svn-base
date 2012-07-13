#ifndef _BUDDYCOMMON_ICE_
#define _BUDDYCOMMON_ICE_

#include <Util.ice>

module xce {
module buddy {
	/**
 	 *	 Buddy关系描述
 	 **/
	enum BuddyDesc
	{
		NoPath,
		Self,
		Friend,
		Apply,
		Applied,
		Block,
		Blocked,
		BiBlock //BilateralBlock
	};
	
	/**
         *       Buddy操作状态描述
         **/
	enum BuddyState{
            	NULLSTATE,
            	ADDAPPLY,
            	ACCEPTAPPLY,
            	DENYAPPLY,
            	REMOVEFRIEND,
            	ADDBLOCK,
            	REMOVEBLOCK,
	    	CHECKERROR,
	    	ADDFRIEND //backdoor
        };

	/**
         *       Buddy操作返回值
         **/
	struct OpState
	{
		BuddyDesc first;
		BuddyDesc second;
	};
	
	/**
         *       申请信息
         **/
	struct ApplyInfo
	{
		MyUtil::Str2StrMap props;
	};
	
	/**
         *       Buddy关系
         **/
	struct Relationship
	{
		int from;
		int to;
	};
	
	/**
         *       Buddy操作记录
         **/
	struct StateRecorder
	{
        	int lastid;
        	BuddyState laststate;
	};
	
	/**
         *       Buddy所有关系组合
         **/
	class BuddyRelation {
		MyUtil::IntSeq friends;
		MyUtil::IntSeq apply; 
		MyUtil::IntSeq applied; 
		MyUtil::IntSeq block; 
		MyUtil::IntSeq blocked; 
		MyUtil::IntSeq biblock; 
	};
	
	/**
         *       Buddy相关操作与所有关系
         **/
	class BuddyRelationHolders {
		int hfrom;
        	BuddyRelation hbr;
       	        StateRecorder hsr;
	};
	
	/**
         *       BuddyRelation相关操作与所有关系
         **/

        class BuddyList {
                BuddyRelation hbr;
        	MyUtil::ByteSeq hlittlehash;
		byte hindex;
	};

	/**
         *       Buddy与该Buddy所有关系组合映射
         **/
	dictionary<int, BuddyRelation> Int2BuddyRelationMap;
	
	/**
         *       Buddy与该Buddy申请关系映射
         **/
	dictionary<int, Relationship> ApplyshipMap;
	
	/**
         *       Buddy与该Buddy申请信息映射
         **/
    	dictionary<int, ApplyInfo> ApplyInfoMap;
	
	/**
         *       Buddy与该Buddy操作返回映射
         **/
    	dictionary<int, OpState> OpStateMap;

};
};

#endif
