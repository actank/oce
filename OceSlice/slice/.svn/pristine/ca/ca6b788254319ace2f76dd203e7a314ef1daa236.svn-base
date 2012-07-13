#ifndef USERED_TRIBE_ICE
#define USERED_TRIBE_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	const int CAdminedTribe = 1;
	const int CMemberedTribe = 0;

	class TribeTinyView
	{
		int id;
		string name;
		string headurl;
		int usercount;
	};
	sequence<TribeTinyView> TribeTinyViewSeq;
	
	class UseredTribeSet 
    {
    	void add(int tribeId);
    	void remove(int tribeId);
    	
    	int size();
    	MyUtil::IntSeq getAll(int begin, int limit);
        
        MyUtil::IntSeq tribeIds;
    };
    
    class UseredTribeManager
    {
       	int getAdminedTribeCount(int userId);
       	int getMemberedTribeCount(int userId);       	
       	
       	MyUtil::IntSeq getAdminedTribes(int userId, int offset, int limit);
		MyUtil::IntSeq getMemberedTribes(int userId, int offset, int limit);
		
		TribeTinyViewSeq getAdminedTribeTinyViews(int userId, int offset, int limit);
		TribeTinyViewSeq getMemberedTribeTinyViews(int userId, int offset, int limit);
		
		void addMemberedTribe(int userId, int tribeId);
		void removeMemberedTribe(int userId, int tribeId);
		
		void addAdminedTribe(int userId, int tribeId);
		void removeAdminedTribe(int userId, int tribeId);
		
		void reloadTribeTinyView(int tribeId);
	};

};
};
};
};
#endif
