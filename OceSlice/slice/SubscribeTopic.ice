#ifndef SUBSCRIBE_TOPIC_ICE
#define SUBSCRIBE_TOPIC_ICE 


module mop
{
module hi
{
module svc
{
module im
{
	class UserTopic
	{
		void updateUser(int userId);
		void updateUserBasic(int userId);
//		void updateUserDesc(int userId);
//		void updateUserDailyExp(int userId);
//		void updateUserVisitCount(int userId);
 //       void updateUserPersonalInfo(int userId);
		void updateUserPassword(int userId,string pwd);
		void updateUserHeadUrl(int userId,string link);
        void updateUserNickName(int userId,string nickName);
	};
	
	class UserRelationTopic
	{
        void addBuddy(int hostId,int guestId);
        void setBlock(int hostId,int guestId);
        void removeBuddy(int hostId,int guestId);
        void changeBuddyGroupId(int hostId,int guestId,int groupId);
        void setBuddy(int hostId,int guestId);
        void addBuddyApply(int hostId,int guestId);
        void addRelationGroupSet(int userId,int groupId,string name);
        void updateRelationGroupSet(int userId,int groupId,string name);
        void removeRelationGroupSet(int userId,int groupId);

    };
    
};
};
};
};
#endif
