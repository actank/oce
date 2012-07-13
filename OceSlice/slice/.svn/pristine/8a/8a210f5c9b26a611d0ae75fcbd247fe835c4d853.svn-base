#ifndef _USER_CACHE_ICE
#define _USER_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module usercache
{
	const string CUserCacheID = "id";
	const string CUserCacheSTATUS = "status";
	const string CUserCacheNAME = "name";
	const string CUserCacheUNIV = "univ";
	const string CUserCacheHEADURL = "headurl";
	const string CUserCacheTINYURL = "tinyurl";
	const string CUserCacheSTATE = "state";
	const string CUserCacheSTAR = "star";
	const string CUserCacheLEVEL = "level";
	const string CUserCacheDOING = "doing";
	const string CUserCacheSTAGE = "stage";
	const string CUserCacheONLINE = "online";
	const string CUserCacheGENDER = "gender";

	sequence<MyUtil::StrSeq> StrSeqSeq;


	class UserCacheData3
	{
		int id;
		int status;
		int univ;
		int state;
		int level;
		string name;
		string tinyUrl;
		byte gender;
		byte online; 
	};
	sequence<UserCacheData3> UserCacheData3Seq;
	dictionary<int, UserCacheData3> UserCacheData3Map;

	const byte FIELDSTATUS = 0;
	const byte FIELDNAME = 1;
	const byte FIELDUNIV = 2;
	const byte FIELDTINYURL = 3;
	const byte FIELDSTATE = 4;
	const byte FIELDLEVEL = 5;
	const byte FIELDGENDER = 6;
	const byte FIELDONLINE = 7;
	

	const byte TABLEPASSPORT = 0;
	const byte TABLEBORN = 1;
	const byte TABLESTATE = 2;
	const byte TABLENAMES = 3;
	const byte TABLEURL = 4;
	const byte TABLESTAGE = 5;

	class UserCacheUpdateOldData
	{
		int userid;
		byte table;
		MyUtil::Str2StrMap data;
	};

	dictionary<byte, string> Byte2StrMap;
	class UserCacheUpdateNewData
	{
		int userid;
		Byte2StrMap data;
	};
	
	interface UserCacheManager
	{
		MyUtil::Str2StrMap getUserCache(int userId);
		MyUtil::Str2StrMapSeq getUserCacheList(MyUtil::IntSeq userIds);
		UserCacheData3Seq getUserCacheList3(MyUtil::IntSeq userIds); // new for return object.
		UserCacheData3Map getUserCacheMap3(MyUtil::IntSeq userIds);
		void load(MyUtil::IntSeq userIds);	// 将要停用的旧接口
		void setData(MyUtil::ObjectResult userData); // 将给id去数据加载,改为直接给出所有数据
		bool isValid();
		void setValid(bool newState);
		void update(UserCacheUpdateNewData data);
		
		void setOnline(MyUtil::Int2IntMap data);

		void reloadSetOnline(MyUtil::Int2IntMap data);
	};
        

        //-----------------------for online---------------------
      
        class OnlineType
        {
           int userId;
           int online;
        };
        sequence<OnlineType>  OnlineTypeSeq;

        //-----------------------for online----------------------

	interface UserCacheReloader
	{
		void reload(MyUtil::IntSeq userIds);
		void rebuildOnline(MyUtil::IntSeq userIds);
		void insert(int userid);
		void update(UserCacheUpdateOldData data);
		bool isValid();
		void setValid(bool newState);
	};

       	interface DistUserCacheReloader
	{
		void reload(MyUtil::IntSeq userIds);
		void insert(int userid);
		void update(UserCacheUpdateOldData data);
		bool isValid();
		void setValid(bool newState);
        void setOnline(int id, int online);
        void setOnlineBatch(OnlineTypeSeq data);
	};

	interface UserCacheLoader 
	{
		void load(MyUtil::IntSeq userIds);
	};


};
};
#endif
