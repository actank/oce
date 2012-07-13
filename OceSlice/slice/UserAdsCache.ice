#ifndef _USER_ADS_CACHE_ICE
#define _USER_ADS_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module useradscache
{
	enum SchoolType
        {
                Elementary, Junior, High, College, University
        };

	class SchoolInfo
	{
		int id;
		string name;
		SchoolType type;
		int enrollyear;
		string department;
		string dorm;
	};

	sequence<SchoolInfo> SchoolInfoSeq;

	class WorkplaceInfo
	{
		string workplacename;
		int jobtitleid;
		int positionid;
	};

	sequence<WorkplaceInfo> WorkplaceInfoSeq;

	class NetworkInfo
	{
		int networkid;
		int stage;
		int status;
	};

	sequence<NetworkInfo> NetworkInfoSeq;

	class UserAdsCacheData
	{
		int id;
		int stage;
		string sex;
		int birthyear;
		int birthmonth;
		int birthday;
		string homeprovince;
		string homecity;
		string seatprovince;
		string seatcity;
		SchoolInfoSeq schools;
		WorkplaceInfoSeq workplaces;
		NetworkInfoSeq networks;
	};

	dictionary<int, UserAdsCacheData> Int2UserAdsCacheDataMap;

	class UserAdsCacheUrlData
	{
		int id;
		int stage;
		string sex;
		int birthyear;
		int birthmonth;
		int birthday;
		string homeprovince;
		string homecity;
		string seatprovince;
		string seatcity;
		string tinyurl;
		SchoolInfoSeq schools;
		WorkplaceInfoSeq workplaces;
		NetworkInfoSeq networks;
	};

	dictionary<int, UserAdsCacheUrlData> Int2UserAdsCacheUrlDataMap;

	interface UserAdsCacheManager
	{
		UserAdsCacheData getUserAdsCache(int userId);	
		Int2UserAdsCacheDataMap getUserAdsCacheList(MyUtil::IntSeq userIds);

		UserAdsCacheUrlData getUserAdsCacheUrl(int userId);	
		Int2UserAdsCacheUrlDataMap getUserAdsCacheUrlList(MyUtil::IntSeq userIds);
		bool isValid();
		void setValid(bool newState);
		void setData(MyUtil::ObjectResult userData);
	};

	interface UserAdsCacheLoader
	{
		void load(MyUtil::IntSeq userIds);
	};
};
};

#endif
