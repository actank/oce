#ifndef TRIBE_TOPICS_ICE
#define TRIBE_TOPICS_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	const int CAddUseredTribe = 1;
	const int CRemoveUseredTribe = 0;
	
    class UseredTribeTopic
    {  
        void updateUseredTribe(MyUtil::IntSeq userIds);
        void updateMemberedTribe(MyUtil::IntSeq userIds, int op, int tribeId);
        void updateAdminedTribe(MyUtil::IntSeq userIds, int op, int tribeId);
    };
};
};
};
};
#endif
