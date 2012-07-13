#ifndef _USER_API_CACHE_ICE
#define _USER_API_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module userapicache
{
	class UserApiCacheData
	{
		//user_passport
		int id;
		int status;
		//user_names
		string name;
		//user_state
		int state;
		//user_right
		int auth;
		//user_config
		int statusconfig;
		int basicconfig;
		string profileprivacy;
		//user_basic
		int stage;
		string gender;
		int univ;
		int birthyear;
		int birthmonth;
		int birthday;
		string homeprovince;
		string homecity;
		//user_url
		string mainurl;
		string headurl;
		string tinyurl;
	};

	class UserApiCacheData1
	{
		//user_passport
		int id;
		int status;
		//user_names
		string name;
		//user_state
		int state;
		int level;
		//user_right
		int auth;
		//user_config
		int statusconfig;
		int basicconfig;
		string profileprivacy;
		//user_basic
		int stage;
		string gender;
		int univ;
		int birthyear;
		int birthmonth;
		int birthday;
		string homeprovince;
		string homecity;
		//user_url
		string mainurl;
		string headurl;
		string tinyurl;
	};

	dictionary<int, UserApiCacheData> Int2UserApiCacheDataMap;
	dictionary<int, UserApiCacheData1> Int2UserApiCacheData1Map;
	dictionary<byte, string> Byte2StrMap;

	const byte TABLEBORN = 0;
	const byte TABLESTAGE = 1;
	const byte TABLESTATE = 2;
	const byte TABLENAMES = 3;
	const byte TABLERIGHT = 4;
	const byte TABLEPASSPORT = 5;
	const byte TABLEURL = 6;
	const byte TABLECONFIG = 7;

	const byte FIELDGENDER = 0;
	const byte FIELDBIRTHYEAR = 1;
	const byte FIELDBIRTHMONTH = 2;
	const byte FIELDBIRTHDAY = 3;
	const byte FIELDHOMEPROVINCE = 4;
	const byte FIELDHOMECITY = 5;
	const byte FIELDUNIV = 6;
	const byte FIELDSTAGE = 7;
	const byte FIELDLEVEL = 8;
	const byte FIELDSTATE = 9;
	const byte FIELDNAME = 10;
	const byte FIELDAUTH = 11;
	const byte FIELDSTATUS = 12;
	const byte FIELDTINYURL = 13;
	const byte FIELDHEADURL = 14;
	const byte FIELDMAINURL = 15;
	const byte FIELDPROFILEPRIVACY = 16;
	const byte FIELDBASICCONFIG = 17;
	const byte FIELDSTATUSCONFIG = 18;

	class UserApiCacheUpdateOldData
	{
		int userid;
		byte table;
		MyUtil::Str2StrMap data;
	};

	class UserApiCacheUpdateNewData
	{
		int userid;
		Byte2StrMap data;
	};

  sequence<UserApiCacheUpdateNewData> UserApiCacheUpdateNewDataSeq;

	interface UserApiCacheManager
	{
		UserApiCacheData getUserApiCache(int userId);	
		Int2UserApiCacheDataMap getUserApiCacheList(MyUtil::IntSeq userIds);
		Int2UserApiCacheData1Map getUserApiCache1List(MyUtil::IntSeq userIds);
		bool isValid();
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
		void update(UserApiCacheUpdateNewData data);
	};

        interface UserApiCacheReloader
        {
                void reload(MyUtil::IntSeq userIds);
                void update(UserApiCacheUpdateOldData data);
		bool isValid();
		void setValid(bool newState);
        };


	interface UserApiCacheLoader
	{
		void load(MyUtil::IntSeq userIds);
	};
};
};

#endif
