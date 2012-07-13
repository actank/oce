#ifndef FOLLOWLOGIC_ICE_
#define FOLLOWLOGIC_ICE_
 
#include <Util.ice>
 
module xce
{
module follow
{
// Pub --> Sub 关系
//         黑与被黑	申请与被申请   follow与followed
//   00    0   0     0     0      0		    0

const int Blocking = 0x20;			//0010 0000
const int Blocked = 0x10;		//0001 0000
const int Following = 0x08;		//0000 0010
const int Followed = 0x04;		//0000 0001
const int Applying = 0x02;		//0000 1000
const int Applied = 0x01;		//0000 0100


	class LogicManager
	{
		int addFollowing(int publisher, int subscriber, bool needApply);
		void blockFollowed(int publisher,int subscriber);
		void cancelFollowing(int publisher, int subscriber);
		void cancelBlocked(int publisher,int subscriber); 
		void acceptApply(int publisher,int subscriber); 
		void refuseApply(int publisher,int subscriber); 
		
		void removeBuddyNotify(int host,int guest);
		
		void addFollowingBoth(int host,int guest);
		void removeFollowingBoth(int host,int guest);
	};
};
};

#endif /*FOLLOWLOGIC_ICE_*/
