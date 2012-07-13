#ifndef __WINDOWS_LIVE_IMPORTER_ICE__
#define __WINDOWS_LIVE_IMPORTER_ICE__

#include <WindowsLiveSchema.ice>

module xce
{
module importer
{

	enum ImportState
	{
		PENDING,
		AUTHING,
		AUTHFAILED,
		AUTHED,
		RETRIVING,
		RETRIVEFAILED,
		RETRIVED,
	};
	class WindowsLiveContacts
	{
		com::live::schema::TLiveContacts contacts;
		ImportState state;
	};

	interface WindowsLiveImporterManager
	{
		void requestByPassword(int xnId, string email, string passwd);
		void requestByDelegationTokenAndLocationID(int xnId, string email, string token, string location);

		["ami", "amd"] WindowsLiveContacts contactsByPassword(int xnId, string email, string passwd);
		["ami", "amd"] WindowsLiveContacts contactsByDelegationTokenAndLocationID(int xnId, string email, string token, string location);

		WindowsLiveContacts response(int xnId, string email);
		
		bool verifyAccount(string email, string passwd);
	};
};
};
#endif
