#ifndef IMPORTER_ICE
#define IMPORTER_ICE

module com
{
module xiaonei
{
module svc
{
module importer
{
	struct OwnerInfo
	{
		int userId;
		string email;
		string password;
	};
	struct BuddyInfoStatus
	{
		int code;
		string message;
	};
	struct BuddyInfo
	{
		string name;
		string email;
		BuddyInfoStatus status;
	};
	sequence<BuddyInfo> BuddyInfoSeq;
	struct BuddyUserInfo
	{
		string email;
		int userId;
		string userName;
		string tinyurl;
		int currentNetworkId;
		int currentNetworkStage;
		string currentNetworkName;
	};
	dictionary<string, BuddyUserInfo> BuddyUserInfoMap;
	
	struct BuddyListStatus
	{
		int code;
		string message;
	};
	struct BuddyList
	{
		BuddyListStatus status;
		OwnerInfo owner;
		BuddyInfoSeq buddies;
		BuddyUserInfoMap users;
	};

	interface ImporterManager
	{
		void sendRequest(OwnerInfo owner); // This is same as MSNRequest
		void sendMSNRequest(OwnerInfo owner);
		void sendGMailRequest(OwnerInfo owner);
		void sendCSVRequest(OwnerInfo owner);
		void sendNeteaseRequest(OwnerInfo owner);
		void send126Request(OwnerInfo owner);
		void sendYahooRequest(OwnerInfo owner);
		void sendSinaRequest(OwnerInfo owner);
		
		BuddyList getResponse(OwnerInfo owner);// This is same as MSNResponse
		BuddyList getMSNResponse(OwnerInfo owner);
		BuddyList getGMailResponse(OwnerInfo owner);
		BuddyList getCSVResponse(OwnerInfo owner);
		BuddyList getNeteaseResponse(OwnerInfo owner);
		BuddyList get126Response(OwnerInfo owner);
		BuddyList getYahooResponse(OwnerInfo owner);
		BuddyList getSinaResponse(OwnerInfo owner);
		
		bool verifyMSNAccount(string liveId,string livePassword);
    };

};
};
};
};

#endif

