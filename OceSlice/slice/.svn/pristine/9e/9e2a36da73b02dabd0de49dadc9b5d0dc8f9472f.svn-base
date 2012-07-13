#ifndef _USER_CACHE_ICE
#define _USER_CACHE_ICE

#include <Util.ice>

module xce
{
module searchcache
{
	dictionary<string, MyUtil::Str2StrMapSeq> Str2Str2StrMapSeq;
	struct SearchCacheResult
	{
		MyUtil::Str2StrMap generels;
		Str2Str2StrMapSeq infos;
	};
	dictionary<int,SearchCacheResult> Int2SearchCacheResultMap;
    dictionary<int,MyUtil::ByteSeq> Int2ByteSeq;
    struct UpdateUnit
    {
        int id;
        string table;
        MyUtil::Str2StrMap fieldPairMap;
    };
    sequence<UpdateUnit> UpdateUnitSeq;
	interface SearchCacheManager
	{
		SearchCacheResult getSearchCache(int userId);
		Int2SearchCacheResultMap getSearchCacheList(MyUtil::IntSeq userIds);
		Int2ByteSeq getSearchCacheList2(MyUtil::IntSeq userIds);
		void load(MyUtil::IntSeq userIds);
        void update(UpdateUnitSeq updateUnits);
		bool isValid();
		void setValid(bool newState);
        bool puts(Int2ByteSeq updateMap);
        bool removes(MyUtil::LongSeq userIds);
	};
    enum DataType {NOUSE, USERID, USERNAME, SIGNATURE, STAR, SELECTED, STATUS, 
        TINYURL, HEADURL, BROWSECONFIG, STATUSCONFIG, HOMECITY,
        HOMEPROVINCE, NETWORKS, REGIONS, WORKPLACES, UNIVERSITIES, 
        COLLEGES, HIGHSCHOOLS, JUNIORSCHOOLS, ELEMENTARYSCHOOLS, GENDER,
        STAGE, BIRTHYEAR, BIRTHMONTH, BIRTHDAY, LASTLOGINTIME, 
        ASTROLOGY, INTEREST, MUSIC, MOVIE, GAME, COMIC, SPORT, BOOK, SOCIETY, STATE,
		BASICCONFIG, VIEWCOUNT, AUTH, PROFILEPRIVACY};
};
};
#endif
