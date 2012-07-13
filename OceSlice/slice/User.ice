#ifndef _USER_ICE
#define _USER_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	//exception UserException extends MyUtil::MceException{};
	//exception NoSuchUserException extends UserException{};
	//exception InvalidUserException extends UserException{};

	class UserVisitCount
	{
		int getCount();
		void setCount(int count);
		void incCount(int step);

		int count;
	};

	class UserHomeView
	{
		MyUtil::Str2StrMap users;
		MyUtil::Str2StrMap userBasic;
		MyUtil::Str2StrMap userDesc;
		MyUtil::Int2IntMap friendDist;
		int onlineStatus;
		int viewCount;
	};

	class QueryCounter
	{
		void beginCountQueryUser();
		int endCountQueryUser();
		void incQueryUserCount();

		void beginCountQueryUserBasic();
		int endCountQueryUserBasic();
		void incQueryUserBasicCount();

		void beginCountQueryUserDesc();
		int endCountQueryUserDesc();
		void incQueryUserDescCount();

		void beginCountQueryUserHomeView();
		int endCountQueryUserHomeView();
		void incQueryUserHomeViewCount();

		void beginCountQueryUserIMView();
		int endCountQueryUserIMView();
		void incQueryUserIMViewCount();

		int queryUserCount;
		int queryUserBasicCount;
		int queryUserDescCount;
		int queryUserHomeViewCount;
		int queryUserIMViewCount;
	};
	
	const string CUID = "ID";
	const string CUPASSWORD = "PASSWORD";
	const string CUEMAIL = "EMAIL";
	const string CUNAME = "NAME";
	const string CUFRIENDCOUNT = "FRIENDCOUNT";
	const string CUNEWMESSAGECOUNT = "NEWMESSAGECOUNT";
	const string CUPOSTCOUNT = "POSTCOUNT";
	const string CUSTATUS = "STATUS";
	const string CULOGINCOUNT = "LOGINCOUNT";
	const string CUHEADURL = "HEADURL";
	//const string CUMESSAGEINCOUNT = "MESSAGE_IN_COUNT";
	const string CULEAVEWORDCOUNT = "LEAVEWORD_COUNT";
	//const string CUMESSAGEOUTCOUNT = "MESSAGE_OUT_COUNT";
	const string CUSELECTED = "SELECTED";
	const string CUVERIFYCODE = "VERIFY_CODE";
	const string CUAUTH = "AUTH";
	const string CUNEWEMAIL = "NEW_EMAIL";
	const string CUEMAILVERIFY = "EMAIL_VERIFY";
	const string CUTRIBECOUNT = "TRIBE_COUNT";
	const string CUGENDER = "GENDER";
	const string CUUNIVERSITY = "UNIVERSITY";
	const string CUEXP = "EXP";
	//const string CULASTIP = "LAST_IP";
	const string CULARGEURL = "LARGEURL";
	const string CUBLOGCONTROL = "BLOG_CONTROL";
	const string CUNOTIFY = "NOTIFY";
	const string CULEAVEWORDFLAG = "LEAVE_WORD_FLAG";
	const string CUENTRYCOMMENTFLAG = "ENTRY_COMMENT_FLAG";
	const string CUSTAR = "STAR";
	const string CUPHOTOCONTROL = "PHOTOCONTROL";
	const string CUGUESTREQUESTCOUNT = "GUESTREQUESTCOUNT";
	const string CUTRIBEREQUESTCOUNT = "TRIBE_REQUESTCOUNT";
	const string CUCOMMONRIGHTS = "COMMON_RIGHTS";
	const string CUPASSWORDMD5 = "PASSWORD_MD5";
	const string CUENTRY = "ENTRY";
	const string CUVERIFYREQUESTCOUNT = "VERIFYREQUESTCOUNT";
	const string CUSPACESIZE = "SPACESIZE";
	//const string CUCOOKIEID = "COOKIE_ID";
	const string CUSIGNATURE = "SIGNATURE";
	const string CUQMAIL = "QMAIL";
	const string CUSOURCE = "SOURCE";
	//const string CUOPENCONFIG = "OPEN_CONFIG";
	//const string CUCHATCONFIG = "CHAT_CONFIG";
	const string CUTEMPLATE = "TEMPLATE";
	const string CUCLASSCREATECOUNT = "CLASS_CREATE_COUNT";
	const string CUCLASSJOINCOUNT = "CLASS_JOIN_COUNT";
	const string CUCLASSINVITEDCOUNT = "CLASS_INVITED_COUNT";
	const string CUSTAGE = "STAGE";
//	const string CUCURNETWORKID = "CUR_NETWORK_ID";
//	const string CUCURNETWORKNAME = "CUR_NETWORK_NAME";
	const string CUISOLDUSER = "IS_OLD_USER";
	class UserData
	{
		int id;
		string password;
		string email;
		string name;
		int friendCount;				// Moved to FriendDist
		int newMessageCount;	// Moved to CountCache
		int postCount;
		int status;
		int loginCount;
		string headUrl;
		//int messageInCount;
		int leavewordCount;		// ?? No use
		int selected;						// Will move to UserState
		string verifyCode;
		int auth;
		string newEmail;
		string emailVerify;
		int tribeCount;					// ?? No use
		string gender;
		int university;
		int exp;
		string largeUrl;
		int blogControl;
		int notify;
		int leavewordFlag;
		int entryCommentFlag;
		int star;
		int photoControl;
		int guestRequestCount;		// Moved to CountCache
		int tribeRequestCount;		// Moved to CountCache
		int commonRights;				// ?? Moved to UserConfig
		string passwordMd5;
		int entry;
		int verifyRequestCount;
		int spaceSize;
		string signature;
		string qmail;
		int source;
	//	int openConfig;
	//	int chatConfig;
		string template1;
		int classInvitedCount;
		int classCreateCount;
		int classJoinCount;
		int stage;
//		int curNetworkId;
//		string curNetworkName;
		int isOldUser;
	};
	
	const string CUBID = "ID";
	const string CUBUNIV = "UNIV";
	const string CUBSELECTED = "SELECTED";
	const string CUBNAME = "NAME";
	const string CUBSTATUS = "STATUS";
	const string CUBGENDER = "GENDER";
	const string CUBUNIVYEAR = "UNIV_YEAR";
	const string CUBDEPARTMENT = "DEPARTMENT";
	const string CUBDORM = "DORM";
	const string CUBBIRTHDAY = "BIRTH_DAY";
	const string CUBBIRTHMONTH = "BIRTH_MONTH";
	const string CUBBIRTHYEAR = "BIRTH_YEAR";
	const string CUBHOMEPROVINCE = "HOME_PROVINCE";
	const string CUBHOMECITY = "HOME_CITY";
	const string CUBCITYCODE = "CITY_CODE";
	const string CUBHIGHSCHOOL = "HIGH_SCHOOL";
	const string CUBHIGHSCHOOLCODE = "HIGH_SCHOOL_CODE";
	const string CUBASTROLOGY = "ASTROLOGY";
	const string CUBHEADURL = "HEADURL";
	const string CUBTINYURL = "TINYURL";
	const string CUBBASICCONTROL = "BASICCONTROL";
	const string CUBSTAR = "STAR";
	const string CUBDOING = "DOING";
	const string CUBDOINGTIME = "DOINGTIME";
	const string CUBSTAGE = "STAGE";
//	const string CUBCURNETWORKID = "CUR_NETWORK_ID";
//	const string CUBCURNETWORKNAME = "CUR_NETWORK_NAME";
	const string CUBJOINYEAR = "JOIN_YEAR";
	const string CUBROOM = "ROOM";
	
	class UserBasicData
	{
		int id;
		int univ;
		int selected;
		string name;
		int status;
		string gender;
		int univYear;
		string department;
		string dorm;
		int birthDay;
		int birthMonth;
		int birthYear;
		string homeProvince;
		string homeCity;
		string cityCode;
		string highSchool;
		string highSchoolCode;
		string astrology;
		string headUrl;
		string tinyUrl;
		int basicControl;
		int star;
		string doing;
		string doingTime;
		int stage;
//		int curNetworkId;
//		string curNetworkName;
		int joinYear;
		string room;
	};
	
	const string CUDID = "ID";
	
	const string CUDREGISTERTIME = "REGISTERTIME";
	const string CUDACTIVATETIME = "ACTIVATETIME";
	const string CUDLASTLOGINTIME = "LASTLOGINTIME";
	const string CUDLASTLOGOUTTIME = "LASTLOGOUTTIME";
	
	const string CUDMSN = "MSN";
	const string CUDHOMEPAGE = "HOMEPAGE";
	const string CUDMOBILEPHONE = "MOBILEPHONE";
	const string CUDFIXPHONE = "FIXPHONE";
	const string CUDQQ = "QQ";
	
	const string CUDFAV = "FAV";
	const string CUDFAVFLAG = "FAV_FLAG";
	
	const string CUDLARGEURL = "LARGEURL";
	const string CUDMAINURL = "MAINURL";
	const string CUDHEADURL = "HEADURL";
	const string CUDTINYURL = "TINYURL";
		
	const string CUDCONTACTCONTROL = "CONTACTCONTROL";
	const string CUDDESCR = "DESCR";
	const string CUDDESCBAN = "DESCBAN";
	const string CUDLASTEMAILNOTIFY = "LASTEMAILNOTIFY";
	const string CUDTEMPLATE = "TEMPLATE";
	const string CUDLASTMODIFYTIME = "LAST_MODIFY_TIME";
	
	class UserDescData
	{
		int id;
		string registerTime;
		string lastLoginTime;
		string lastLogoutTime;
		string msn;
		string homepage;
		string mobilePhone;
		string fixPhone;
		int contactControl;
		int qq;
		string mainUrl;
		string descr;
		int descBan;
		string activateTime;
		int favFlag;
		string fav;
		string lastEmailNotify;
		//string lastEmailNotify1;
		int template1;
		string lastModifyTime;	
	};
	
	/*const string CUCUSERID = "ID";                       
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
	
	class UserConfig
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
	};
	*/
	const string CEIUSERID = "USERID";
	const string CEIELEMENTARYSCHOOLID = "ELEMENTARY_SCHOOL_ID";
	const string CEIELEMENTARYSCHOOLNAME = "ELEMENTARY_SCHOOL_NAME";
	const string CEIELEMENTARYSCHOOLYEAR = "ELEMENTARY_SCHOOL_YEAR";
	const string CEIJUNIORHIGHSCHOOLID = "JUNIOR_HIGH_SCHOOL_ID";
	const string CEIJUNIORHIGHSCHOOLNAME = "JUNIOR_HIGH_SCHOOL_NAME";
	const string CEIJUNIORHIGHSCHOOLYEAR = "JUNIOR_HIGH_SCHOOL_YEAR";
	
	class ExtraInfo
	{
		int userId;
		int elementarySchoolId;
		string elementarySchoolName;
		int elementarySchoolYear;
		int juniorHighSchoolId;
		string juniorHighSchoolName;
		int juniorHighSchoolYear;

	};

	const string CVAUSERID = "USERID";
	const string CVANICKNAME = "NICKNAME";
	
	class ValueAddedData
	{
		int userId;
		string nickname;
	};
	sequence<ValueAddedData> ValueAddedDataSeq;
	
	class UserManager
	{
		void createUser(MyUtil::Str2StrMap properties);
		void createUserBasic(MyUtil::Str2StrMap properties);
		void createUserDesc(MyUtil::Str2StrMap properties);
		void createUserTime(MyUtil::Str2StrMap properties);
		void createUserContact(MyUtil::Str2StrMap properties);
		void createUserFav(MyUtil::Str2StrMap properties);
		void createUserUrl(MyUtil::Str2StrMap properties);
		//void createUserDailyExp(MyUtil::Str2StrMap properties);
		void createUserVisitCount(MyUtil::Str2StrMap properties);
		//void createUserPersonalInfo(MyUtil::Str2StrMap properties);
		//void createUserConfig(MyUtil::Str2StrMap properties);
		void createExtraInfo(MyUtil::Str2StrMap properties);		
		
		MyUtil::Int2IntMap getLastMotifyTime(MyUtil::IntSeq ids);
		
		void addUser(int userId, MyUtil::Str2StrMap u, MyUtil::Str2StrMap ub, MyUtil::Str2StrMap ud);
		void removeUser(int userId);
		void reloadUser(int userId);
		void reloadUserBasic(int userId);
		void reloadUserDesc(int userId);
		void reloadUserTime(int userId);
		void reloadUserContact(int userId);
		void reloadUserFav(int userId);
		void reloadUserUrl(int userId);
		//void reloadUserDailyExp(int userId);
		void reloadUserVisitCount(int userId);
		//void reloadUserPersonalInfo(int userId);

		//MyUtil::Str2StrMap getUserConfig(int uid) throws UserException;
		//void setUserConfig(int uid, MyUtil::Str2StrMap props);	

//		ExtraInfo getExtraInfo(int uid) throws UserException;
		void setExtraInfo(int uid, MyUtil::Str2StrMap props);		

	//	int getUserIdByEmail(string email) throws UserException;
		void setUserIdByEmail(string email, int id);

		//MyUtil::Str2StrMap getUser(int userId) throws UserException;
		void setUser(int userId, MyUtil::Str2StrMap properties);

//		MyUtil::Str2StrMap getUserBasic(int userId) throws UserException;
		void setUserBasic(int userId, MyUtil::Str2StrMap properties);

//		MyUtil::Str2StrMap getUserDesc(int userId) throws UserException;
		void setUserDesc(int userId, MyUtil::Str2StrMap properties);

		//MyUtil::Str2StrMap getUserDailyExp(int userId) throws UserException;
		//void setUserDailyExp(int userId, MyUtil::Str2StrMap properties);

		//MyUtil::Str2StrMap getUserPersonalInfo(int userId) throws UserException;
		//void setUserPersonalInfo(int userId,MyUtil::Str2StrMap properties);

		// onlinestate
		//void setUserIMOnlineStat(int userId, int stat);
		void setUserOnlineStat(int userId, int stat);
		//void setUserIMOnlineStats(MyUtil::IntSeq userIds, int stat);
		void setUserOnlineStats(MyUtil::IntSeq userIds, int stat);

		// some View
//		MyUtil::Str2StrMap getUserIMView(int userId) throws UserException;
//		MyUtil::Str2StrMap getUserHomeView(int userId) throws UserException;		
//		UserHomeView getUserHomeViewB(int userId) throws UserException;		
//		UserHomeView getUserHomeViewA(int userId) throws UserException;		

		// counter:
		void incFriendCount(int userId);
		void decFriendCount(int userId);
		void incGuestRequestCount(int userId);
		void decGuestRequestCount(int userId);

		void incClassCreateCount(int userId);
		void decClassCreateCount(int userId);
		void incClassJoinCount(int userId);
		void decClassJoinCount(int userId);
		void incClassInvitedCount(int userId);
		void decClassInvitedCount(int userId);

		void incPostCount(int userId);
		void decPostCount(int userId);
		void incExp(int userId, int value);
		void decExp(int userId, int value);
		void incLeaveWordCount(int userId);
		void decLeaveWordCount(int userId);
		void incTribeCount(int userId);
		void decTribeCount(int userId);
		void incTribeRequestCount(int userId);
		void decTribeRequestCount(int userId);
		void incNewMessageCount(int userId);
		void decNewMessageCount(int userId);
		void incMessageInCount(int userId);
		void decMessageInCount(int userId);
		void incMessageOutCount(int userId);
		void decMessageOutCount(int userId);

		void incLoginCount(int userId);

//		int getUserVisitCount(int userId) throws UserException;
		void setUserVisitCount(int userId, int count);

		void incUserVisitCount(int userId);
		void incUserVisitCountBatch(MyUtil::IntSeq userIds, int count);


		// These two interface is not the same with above.

		void updateHeadUrl(int userId, string headUrl, string mainUrl, string tinyUrl, string largeUrl);
		bool hasUser(string email);

		MyUtil::Str2StrMap getUserRightInfo(int userId);

		ValueAddedData getValueAddedData(int userId);
		ValueAddedDataSeq getValueAddedDataSeq(MyUtil::IntSeq ids);
		//bool isValueAddedNicknameExist(string nickname);
		void setValueAddedData(int userId, MyUtil::Str2StrMap props);
		void addValueAddedData(MyUtil::Str2StrMap props);
		void reloadValueAddedData(int userId);

//		int getIdByValueAddedName(string name) throws UserException;
		void setValueAddedNameById(string name, int id);		
	};


};  
};  
};  
};  
#endif
