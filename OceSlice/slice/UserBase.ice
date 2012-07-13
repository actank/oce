#ifndef _USERBASE_ICE
#define _USERBASE_ICE

#include <Util.ice>

#include <ObjectCache.ice>
module xce
{
module userbase
{
	const string CUPID          = "ID";
	const string CUPSTATUS      = "STATUS";
	const string CUPACCOUNT     = "ACCOUNT";
	const string CUPDOMAIN      = "DOMAIN";
	const string CUPPASSWORDMD5 = "PASSWORD_MD5";
	const string CUPSAFESTATUS  = "SAFE_STATUS";
	
	class UserPassportData
	{
		int    uID;
		int    status;
		int    safeStatus;
		string account;
		string domain;
		string passwordmd5;
	};
	
	const string CUNID    = "ID";
	const string CUNNAME = "NAME";
	const string CUNNICKNAME = "NICKNAME";
	
	class UserNameData
	{
		int    uID;
		string name;
		string nickname;
	};
	
	const string CURID           = "ID";
	const string CURAUTH         = "AUTH";
	const string CURCOMMONRIGHTS = "COMMON_RIGHTS";
	
	class UserRightData
	{
		int    uID;
		int    auth;
		int    rights;
	};

	const string CUSID = "ID";
	const string CUSSTATE = "STATE";
	const string CUSSTAR = "STAR";
	const string CUSLEVEL = "LEVEL";

	const int CStateSELECTED = 0;
	const int CStateVIPMEMBER = 1;
	const int CStateMSNIMME = 2;
	const int CStateMOBILEBIND = 3;
	const int CStateEVERVIPMEMBER = 4;
	const int CStateGUIDEDONE = 5;
	const int CStateSOCIALGRAPHGOOD = 6;
	const int CStateKEEPUSE = 7;
	const int CStateCHARITY = 8;
	const int CStateCHARITYINVITED = 9;
	const int CStateVEST = 10;
	const int CStateDisneyVIP = 11;
	const int CStateAPPLYSPAMMER = 12;
	const int CStateGAMEMANAGER = 13;
	const int CStateHIDEADS = 14;
	const int CStateDOUBLEACCOUNT = 15;
	const int CStateSAMEPASSWORD = 16;
	const int CStateCOUPLESSPACE = 17;
	const int CStatePAGEMANAGER = 18;
  const int CStateEXPERIMENT = 19;
  const int CStateACTIVE = 20;
  const int CStateJUMPLOGIC = 21;
  const int CStateLUNARBIRTH = 22;
  const int CStateGIFTMEMBER = 23;
  const int CStateZOMBIE = 24;
  const int CStateMOBILEMEMBER = 25;
  const int CStateINWHITELIST = 26;
  const int CStateGROUPMEMBER = 27;
  const int CStateCHANNELSPOKER = 28;
  const int CStateV6 = 29;

	class UserStateData
	{
		int id;
		int state;
		int star;
		int level;
	};

	const string CULCID         = "ID";
	const string CULCLOGINCOUNT = "LOGINCOUNT";

	class UserLoginCountData
	{
		int    uID;
		int    logincount;
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
	const string CUBISLUNARBIRTH = "IS_LUNAR_BIRTH";
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
		int doingTime;
		int stage;
		string room;
	};

	class UserBasicDataN
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
		int doingTime;
		int stage;
		string room;
		bool isLunarBirth;
	};

	const string CUBOID = "ID";
	const string CUBOGENDER = "GENDER";
	const string CUBOBIRTHDAY = "BIRTH_DAY";
	const string CUBOBIRTHMONTH = "BIRTH_MONTH";
	const string CUBOBIRTHYEAR = "BIRTH_YEAR";
	const string CUBOHOMEPROVINCE = "HOME_PROVINCE";
	const string CUBOHOMECITY = "HOME_CITY";
	const string CUBOCITYCODE = "CITY_CODE";
	const string CUBOASTROLOGY = "ASTROLOGY";
	const string CUBOISLUNARBIRTH = "IS_LUNAR_BIRTH";

	class UserBornData
	{
		int id;
		string gender;
		int birthDay;
		int birthMonth;
		int birthYear;
		string homeProvince;
		string homeCity;
		string cityCode;
		string astrology;
	};
	
	class UserBornDataN
	{
		int id;
		string gender;
		int birthDay;
		int birthMonth;
		int birthYear;
		string homeProvince;
		string homeCity;
		string cityCode;
		string astrology;
		bool isLunarBirth;
	};

	const string CUSGID = "ID";
	const string CUSGUNIV = "UNIV";
	const string CUSGSTAGE = "STAGE";
	
	class UserStageData
	{
		int id;
		int univ;
		int stage;
	};

	const string CUDID = "ID";
	const string CUDDOING = "DOING";
	const string CUDDOINGTIME = "DOINGTIME";
	
	class UserDoingData
	{
		int id;
		string doing;
		int doingTime;
	};
	class UserUrlData
	{
		int id;
		string largeUrl;
		string mainUrl;
		string headUrl;
		string tinyUrl;
	};
	
	class UserPassportManager
	{

		void createUserPassport(MyUtil::Str2StrMap props);
		
		UserPassportData getUserPassport(int uid);
		
		void setUserPassport(int uid, MyUtil::Str2StrMap props);	
		
		void reload(int userId);
		int getUserIdByEmail(string email);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserNameManager
	{

		void createUserName(MyUtil::Str2StrMap props);
		
		UserNameData getUserName(int uid);
		
		void setUserName(int uid, MyUtil::Str2StrMap props);	
		
		void reload(int userId);
		
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserRightManager
	{

		void createUserRight(MyUtil::Str2StrMap props);
		
		UserRightData getUserRight(int uid);
		
		void setUserRight(int uid, MyUtil::Str2StrMap props);	
		
		void reload(int userId);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserStateManager
	{

		void createUserState(MyUtil::Str2StrMap props);
		
		UserStateData getUserState(int uid); 
		
		void setUserState(int uid, MyUtil::Str2StrMap props);
                void setStateOn(int userId, int pos);
                void setStateOff(int userId, int pos);
		
		void reload(int userId);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserLoginCountManager
	{

		void createUserLoginCount(MyUtil::Str2StrMap props);
		
		int getUserLoginCount(int uid);
		
		void incUserLoginCount(int uid);	
		
		void reload(int userId);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};
	

	
	class UserBasicManager
	{
		UserBasicData getUserBasic(int userId);
		bool isValid();
		void setValidForce(bool newState);
	};
	
	class UserBornManager
	{
	
		void createUserBorn(MyUtil::Str2StrMap properties);

		void reloadUserBorn(int userId);
		
		UserBornData getUserBorn(int userId);
		
		void setUserBorn(int userId, MyUtil::Str2StrMap properties);
		
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);

		bool isValid();
	};
	
	class UserStageManager
	{
	
		void createUserStage(MyUtil::Str2StrMap properties);

		void reloadUserStage(int userId);
		
		UserStageData getUserStage(int userId);
		
		void setUserStage(int userId, MyUtil::Str2StrMap properties);

		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};
	
	class UserDoingManager
	{
	
		void createUserDoing(MyUtil::Str2StrMap properties);

		void reloadUserDoing(int userId);
		
		UserDoingData getUserDoing(int userId);
		
		void setUserDoing(int userId, MyUtil::Str2StrMap properties);

		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	class UserUrlManager
	{
		void createUserUrl(MyUtil::Str2StrMap properties);
		void reload(int userId);
		UserUrlData getUserUrl(int userId);
		void setUserUrl(int userId, MyUtil::Str2StrMap properties);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};


	class UserBaseViewData
	{
		UserPassportData upd;
		UserRightData    urd;
		UserNameData     und;
		UserLoginCountData ulcd;
		UserStateData usd;
		UserUrlData userurl;
		UserBasicData userbasic;
	};
	
	class UserBaseViewDataN
	{
		UserPassportData upd;
		UserRightData    urd;
		UserNameData     und;
		UserLoginCountData ulcd;
		UserStateData usd;
		UserUrlData userurl;
		UserBasicDataN userbasic;
	};

	class UserCommonViewData
	{
		UserPassportData userpassport;
                UserRightData    userright;
                UserNameData     username;
                UserLoginCountData userlogincount;
		UserStateData userstate;
                UserUrlData userurl;
		int stage;
		int univ;
		string gender;
	};

	
	class UserBaseViewManager
	{
		UserBaseViewData getUserBaseView(int uID);
		UserCommonViewData getUserCommonView(int uID);
		bool isValid();
		void setValidForce(bool newState);
	};


	class UserPassportReader
	{
		UserPassportData getUserPassport(int uid);
		
		void setUserPassport(int uid, MyUtil::Str2StrMap props);	
		
		int getUserIdByEmail(string email);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserNameReader
	{
		UserNameData getUserName(int uid);
		
		void setUserName(int uid, MyUtil::Str2StrMap props);	
		
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserRightReader
	{
		UserRightData getUserRight(int uid);
		
		void setUserRight(int uid, MyUtil::Str2StrMap props);	
		
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	
	class UserStateReader
	{
		UserStateData getUserState(int uid); 
		
		void setUserState(int uid, MyUtil::Str2StrMap props);
                void setStateOn(int userId, int pos);
                void setStateOff(int userId, int pos);
		
		void setData(MyUtil::ObjectResult userData);
		void setValid(bool newState);
		bool isValid();
	};

	
	class UserLoginCountReader
	{
		int getUserLoginCount(int uid);
		
		void incUserLoginCount(int uid);	
		
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};
	

	
	class UserBasicReader
	{
		UserBasicDataN getUserBasic(int userId);
		bool isValid();
		void setValidForce(bool newState);
	};
	
	class UserBornReader
	{
		UserBornDataN getUserBorn(int userId);
		
		void setUserBorn(int userId, MyUtil::Str2StrMap properties);
		
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);

		bool isValid();
	};
	
	class UserStageReader
	{
		UserStageData getUserStage(int userId);
		
		void setUserStage(int userId, MyUtil::Str2StrMap properties);

		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};
	
	class UserDoingReader
	{
		UserDoingData getUserDoing(int userId);
		
		void setUserDoing(int userId, MyUtil::Str2StrMap properties);

		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};

	class UserUrlReader
	{
		UserUrlData getUserUrl(int userId);
		void setUserUrl(int userId, MyUtil::Str2StrMap properties);
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		bool isValid();
	};
	class UserBaseViewReader
	{
		UserBaseViewDataN getUserBaseView(int uID);
		UserCommonViewData getUserCommonView(int uID);
		bool isValid();
		void setValidForce(bool newState);
	};

	class UserPassportWriter
	{

		void createUserPassport(MyUtil::Str2StrMap props);
		void setUserPassport(int uid, MyUtil::Str2StrMap props);	
		void reload(int userId);
		void setValid(bool newState);
	};

	
	class UserNameWriter
	{

		void createUserName(MyUtil::Str2StrMap props);
		
		void setUserName(int uid, MyUtil::Str2StrMap props);	
		
		void reload(int userId);
		
		void setValid(bool newState);
	};

	
	class UserRightWriter
	{

		void createUserRight(MyUtil::Str2StrMap props);
		
		void setUserRight(int uid, MyUtil::Str2StrMap props);	
		
		void reload(int userId);
		void setValid(bool newState);
	};

	
	class UserStateWriter
	{

		void createUserState(MyUtil::Str2StrMap props);
		
		void setUserState(int uid, MyUtil::Str2StrMap props);
                void setStateOn(int userId, int pos);
                void setStateOff(int userId, int pos);
		
		void reload(int userId);
		void setValid(bool newState);
	};

	
	class UserLoginCountWriter
	{

		void createUserLoginCount(MyUtil::Str2StrMap props);
		
		void incUserLoginCount(int uid);	
		
		void reload(int userId);
		void setValid(bool newState);
	};
	
	
	class UserBornWriter
	{
		void createUserBorn(MyUtil::Str2StrMap properties);

		void reloadUserBorn(int userId);
		
		void setUserBorn(int userId, MyUtil::Str2StrMap properties);
		
		void setValid(bool newState);

	};
	
	class UserStageWriter
	{
	
		void createUserStage(MyUtil::Str2StrMap properties);

		void reloadUserStage(int userId);
		
		void setUserStage(int userId, MyUtil::Str2StrMap properties);

		void setValid(bool newState);
	};
	
	class UserDoingWriter
	{
	
		void createUserDoing(MyUtil::Str2StrMap properties);

		void reloadUserDoing(int userId);
		
		void setUserDoing(int userId, MyUtil::Str2StrMap properties);

		void setValid(bool newState);
	};

	class UserUrlWriter
	{
		void createUserUrl(MyUtil::Str2StrMap properties);
		void reload(int userId);
		void setUserUrl(int userId, MyUtil::Str2StrMap properties);
		void setValid(bool newState);
	};


};  
};  
#endif
