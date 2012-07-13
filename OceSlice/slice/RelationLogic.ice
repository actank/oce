#ifndef RELATION_LOGIC_ICE
#define RELATION_LOGIC_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module relation
{
    interface RelationLogicManager
    {
		void addApply(int fromId, int toId);
		void acceptRequest(int toId, int fromId);
		void denyRequest(int toId, int fromId);
		void removeBuddy(int hostId, int guestId);
		
		void addBlock(int hostId, int guestId);
		void removeBlock(int hostId, int guestId);
    };
};
};
};
};
#endif

