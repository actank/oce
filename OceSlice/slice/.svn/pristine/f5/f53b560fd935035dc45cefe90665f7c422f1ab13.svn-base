#ifndef _USER_CONFIG_ICE
#define _USER_CONFIG_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module mop
{
module hi
{
module svc
{
module model
{

	exception UserException extends MyUtil::MceException{};
	
	const string CUCUSERID = "ID";                       
	const string CUCBASICCONFIG = "BASIC_CONFIG";         
	const string CUCPOKECONFIG= "POKE_CONFIG";          
	const string CUCREQUESTFRIENDCONFIG = "REQUEST_FRIEND_CONFIG";
	const string CUCPHOTOCONFIG = "PHOTO_CONFIG";         
	const string CUCMESSAGECONFIG = "MESSAGE_CONFIG";    
	const string CUCEMAILCONFIG = "EMAIL_CONFIG";
	const string CUCBROWSECONFIG = "BROWSE_CONFIG";
	const string CUCDISPLAYMENULIST = "DISPLAY_MENU_LIST";
	const string CUCMOREMENULIST = "MORE_MENU_LIST";
	const string CUCWANTSEECSS = "WANT_SEE_CSS";
	const string CUCSENDFEEDCONFIG = "SEND_FEED_CONFIG";
	const string CUCRECVFEEDCONFIG = "RECV_FEED_CONFIG";
	const string CUCPROFILEPRIVACY = "PROFILE_PRIVACY";
	const string CUCSTATUSCONFIG = "STATUS_CONFIG";	

	class UserConfigData
	{
		int userId;
		int basicConfig;
		int pokeConfig;
		int requestFriendConfig;
		int photoConfig;
		int messageConfig;
		long emailConfig;	
		int browseConfig;
		string displayMenuList;
		string moreMenuList;
		short wantSeeCss;
	    string sendFeedConfig;
	    string recvFeedConfig;
	    string profilePrivacy;
	    int statusConfig;
	};
	
	class UserConfigManager
	{
		void createUserConfig(MyUtil::Str2StrMap properties);
		MyUtil::Str2StrMap getUserConfig(int uid) throws UserException;
		void setUserConfig(int uid, MyUtil::Str2StrMap props);
		void reload(int userId);

		//供Preloader调用
		void setData(MyUtil::ObjectResult userData);

//		bool isValid();
		void setValid(bool newState);
	};

	class UserConfigReader
	{
		MyUtil::Str2StrMap getUserConfig(int uid);

		//供Writer和Preloader调用
		void setUserConfig(int uid, MyUtil::Str2StrMap properties);
		void setData(MyUtil::ObjectResult userData);

		bool isValid();
		void setValid(bool newState);
	};

	class UserConfigWriter
	{
		void createUserConfig(MyUtil::Str2StrMap properties);
		void setUserConfig(int uid, MyUtil::Str2StrMap props);
		void reload(int userId);

		bool isValid();
		void setValid(bool newState);
	};

};
};
};
};
#endif
    
    
    
    
    
    
    
    
    
    
    
