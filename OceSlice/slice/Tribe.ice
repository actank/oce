#ifndef TRIBE_ICE
#define TRIBE_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	exception TribeException extends MyUtil::MceException{};
    exception NoSuchTribeException extends TribeException{};
    exception InvalidTribeException extends TribeException{};
    
    class TribeManager
    {
    	MyUtil::Str2StrMap createTribe(MyUtil::Str2StrMap properties) throws TribeException;
	    void removeTribe(int tribeId);
	    void reloadTribe(int tribeId);

        MyUtil::Str2StrMap getTribe(int tribeId) throws TribeException;
        void setTribe(int tribeId, MyUtil::Str2StrMap properties);
    
		// Member
		void reloadTribeMember(int tribeId);
    	void addMember(int tribeId, int userId, MyUtil::Str2StrMap mem);
		void removeMember(int tribeId, int userId);
			
		int getMemberCount(int tribeId, MyUtil::Str2StrMap filter) throws TribeException;
		MyUtil::Str2StrMap getMember(int tribeId, int userId) throws TribeException;        
		MyUtil::Str2StrMapSeq getMembers(int tribeId, MyUtil::IntSeq memberIds) throws TribeException;
		MyUtil::Str2StrMapSeq getMatchedMembers(int tribeId, MyUtil::Str2StrMap filter, int offset, int limit) throws TribeException;

		void setMember(int tribeId, int userId, MyUtil::Str2StrMap properties);
	
		// Admin
		void reloadTribeAdmin(int tribeId);
		void addAdmin(int tribeId, int userId, MyUtil::Str2StrMap mem);
		void removeAdmin(int tribeId, int userId);
		
		int getAdminCount(int tribeId, MyUtil::Str2StrMap filter) throws TribeException;
		MyUtil::Str2StrMap getAdmin(int tribeId, int userId) throws TribeException;        
		MyUtil::Str2StrMapSeq getAdmins(int tribeId, MyUtil::IntSeq memberIds) throws TribeException;
		MyUtil::Str2StrMapSeq getMatchedAdmins(int tribeId, MyUtil::Str2StrMap filter, int offset, int limit) throws TribeException;

		void setAdmin(int tribeId, int userId, MyUtil::Str2StrMap properties);
		
		// Block 
		void reloadTribeBlock(int tribeId);
		void addBlock(int tribeId, int userId, MyUtil::Str2StrMap mem);
		void removeBlock(int tribeId, int userId);
		
		int getBlockCount(int tribeId, MyUtil::Str2StrMap filter) throws TribeException;
		MyUtil::Str2StrMap getBlock(int tribeId, int userId) throws TribeException;        
		MyUtil::Str2StrMapSeq getBlocks(int tribeId, MyUtil::IntSeq memberIds) throws TribeException;
		MyUtil::Str2StrMapSeq getMatchedBlocks(int tribeId, MyUtil::Str2StrMap filter, int offset, int limit) throws TribeException;

		void setBlock(int tribeId, int userId, MyUtil::Str2StrMap properties);
	};
};
};
};
};
#endif

