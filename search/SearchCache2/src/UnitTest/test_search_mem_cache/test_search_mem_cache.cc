/** 
 * @file test_search_mem_cache.cc
 * @brief 用于测试SearchMemCache的更新逻辑
 * @author zhigang.wu
 * @date 2010-04-15
 */


#include "test_search_mem_cache.h"
#include <iostream>
#include <string>
//#include <gtest/gtest.h>
#include "SearchCache.h"
#include "OceCxxAdapter/src/SearchMemCacheAdapter.h"
#include "search/SearchCache2/src/Util/SearchMemCacheData.h"
#include "search/SearchCache2/src/Util/IDbCache.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"

using std::cout;
using std::endl;
using std::string;

using namespace xce::searchcache;

void Usage();
void BinStr2ByteSeqMap(const long userId, const BinString* pBinString, Int2ByteSeq& id2Map);
bool ShowSearchCacheResult(Int2SearchCacheResultMap& map);
void ShowSearchCacheData(const SearchCacheData& s);
bool ShowSearchCacheResult(Int2ByteSeq& map);
bool GetUserFromDBCache(const int id, SearchCacheData& data);

int main(int argc, char* argv[]) {
  MyUtil::init_logger("Mce", "test_search_mem_cache.log", "DEBUG");
  if (1 == argc || 0 == strcmp(argv[1], "--help")) {
    Usage();
  }/* else if (0 == strcmp(argv[1], "unit_test")){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  }*/
}

void Usage() {
  cout<<"Usage: <unit_test | show_user_info_in_mem_cache user_id_list>" << endl;
}

///*****TEST(TestSearchMemCache, TestUpdateAndRemove) {
///*****  //1. 从TT中查寻相应数据
///*****  int user_id = 58742;
///*****
///*****  MyUtil::IntSeq user_ids;
///*****  user_ids.push_back(user_id);
///*****
///*****  SearchCacheData data;
///*****  GetUserFromDBCache(user_id, data);
///*****
///*****  //2. 修改相应字段
///*****  data._name = "武志刚20100416";
///*****  cout << "after data.name:" << endl;
///*****  ShowSearchCacheData(data);
///*****  //3. 序列化后写入相应的MemCache
///*****  Int2ByteSeq id_2_byte_seq;
///*****  BinStringPtr bin_string_ptr = data.Serialize();
///*****
///*****  BinStr2ByteSeqMap(user_id, bin_string_ptr.get(), id_2_byte_seq);
///*****  cout << "before SearchMemCacheAdapter::instance().doUpdateMemCacheData:" << endl;
///*****  ShowSearchCacheResult(id_2_byte_seq);
///*****
///*****  SearchMemCacheAdapter::instance().doUpdateMemCacheData(id_2_byte_seq);
///*****
///*****
///*****  Int2ByteSeq results2 = SearchMemCacheAdapter::instance().getSearchCacheMap2(user_ids);
///*****  cout << "After SearchMemCacheAdapter::instance().getSearchCacheMap2:" << endl;
///*****  ShowSearchCacheResult(results2);
///*****  //4. 读MemCache
///*****  Int2SearchCacheResultMap results = SearchMemCacheAdapter::instance().getSearchCacheMap(user_ids);
///*****  cout << "After SearchMemCacheAdapter::instance().getSearchCacheMap:" << endl;
///*****  ShowSearchCacheResult(results);
///*****  EXPECT_EQ(1, results.size());
///*****  SearchCacheResult& result = (results.begin())->second;
///*****  EXPECT_STREQ("武志刚20100416", result.generels[SEARCH_CACHE_NAME].c_str());
///*****  //5. 在MemCache中删除该用户信息
///*****  MyUtil::LongSeq user_ids2;
///*****  user_ids2.push_back(58742);
///*****  SearchMemCacheAdapter::instance().doRemoveMemCacheData(user_ids2);
///*****  results = SearchMemCacheAdapter::instance().getSearchCacheMap(user_ids);
///*****
///*****  EXPECT_EQ(0, results.size());
///*****}

bool GetUserFromDBCache(const int id, SearchCacheData& data) {
    boost::shared_ptr<IDbCache> db_cache; 
    if (0 == id % 4) {
      db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://s17:1981");
    } else if (1 == id % 4 ) {
      db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://s18:1981");
    } else if (2 == id % 4 ) {
      db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://s19:1981");
    } else if (3 == id % 4 ) {
      db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://s20:1981");
    }
    if (db_cache->Initialize() == false) {
        std::cout<<"Db initialize failed"<<std::endl;
        return false;
    }

    if (!db_cache->Query(id, &data)) {
        std::cout << "error userid = " << id << std::endl;
        return false;
    } 

    cout << "user info in tt is:" << endl;
    ShowSearchCacheData(data);
    db_cache->Close();

    return true;
}

void BinStr2ByteSeqMap(const long userId, const BinString* pBinString, Int2ByteSeq& id2Map) {
  char * pBuffer=NULL;
  int len=0;
  if (NULL == pBinString || 0 >= userId) {
    return;
  }

  pBinString->getBuf(&pBuffer, &len);

  if (NULL == pBuffer || 0 == len) {
    return;
  }

  id2Map[userId].assign(pBuffer, pBuffer+len);
}

void ShowSearchCacheData(const SearchCacheData& s) {
  cout <<"id=" << s._id << endl; 
  cout << "name=" << s._name << endl; 
  cout << "gender=" << s._gender<< endl; 
  cout << "signature=" << s._signature << endl; 
  cout << "view_count=" << s._viewcount<< endl; 
  cout << "star=" << s._star << endl; 
  cout << "status=" << s._status << endl; 
  cout << "stage=" << s._stage<< endl; 
  cout << "birthday_year=" << s._birthday_year<< endl; 
  cout << "birthday_month=" << s._birthday_month<< endl; 
  cout << "birthday_day=" << s._birthday_day<< endl; 
  cout << "tinyurl=" << s._tinyurl << endl; 
  cout << "headurl=" << s._headurl << endl; 
  cout << "astrology=" << s._astrology << endl; 
  cout << "browse_config=" << s._browse_config << endl; 
  cout << "status_config=" << s._status_config << endl; 
  cout << "home_city=" << s._home_city << endl; 
  cout << "home_province=" << s._home_province << endl; 
  cout << "intrest=" << s._interest<< endl; 
  cout << "music=" << s._music<< endl; 
  cout << "movie=" << s._movie<< endl; 
  cout << "game=" << s._game<< endl; 
  cout << "comic=" << s._comic<< endl; 
  cout << "sport=" << s._sport<< endl; 
  cout << "book=" << s._book<< endl; 
  cout << "society=" << s._society<< endl; 
  cout << "lastlogintime=" << s._lastlogintime << endl; 
  cout << "network" << endl; 
  for (unsigned int i=0; i< s._networks.size(); ++i) { 
    cout << "tid=" << s._networks[i].id << endl; 
    cout << "tstage=" << s._networks[i].stage << endl; 
    cout << "tname=" << s._networks[i].name<< endl; 
  } 
  cout << "regions" << endl; 
  for (unsigned i=0; i< s._regions.size(); ++i) { 
    cout << "tid=" << s._regions[i].id << endl; 
    cout << "tcity=" << s._regions[i].city<< endl; 
    cout << "tprovince=" << s._regions[i].province<< endl; 
  } 
  cout << "workplace" << endl; 
  for (unsigned i=0; i< s._workplaces.size(); ++i) { 
    cout << "tid=" << s._workplaces[i].id << endl; 
    cout << "tname=" << s._workplaces[i].name<< endl; 
  } 
  cout << "university" << endl; 
  for (unsigned i=0; i< s._universities.size(); ++i) { 
    cout << "tid=" << s._universities[i].id << endl; 
    cout << "tyear=" << s._universities[i].year << endl; 
    cout << "tname=" << s._universities[i].name<< endl; 
    cout << "tdepartment=" << s._universities[i].department << endl; 
    cout << "tdorm=" << s._universities[i].dorm<< endl; 
  } 
  cout << "college" << endl; 
  for (unsigned i=0; i< s._colleges.size(); ++i) { 
    cout << "tid=" << s._colleges[i].id << endl; 
    cout << "tyear=" << s._colleges[i].year << endl; 
    cout << "tname=" << s._colleges[i].name<< endl; 
    cout << "tdepartment=" << s._colleges[i].department << endl; 
  } 
  cout << "highschool" << endl; 
  for (unsigned i=0; i< s._highSchools.size(); ++i) { 
    cout << "tid=" << s._highSchools[i].id << endl; 
    cout << "tyear=" << s._highSchools[i].year << endl; 
    cout << "tname=" << s._highSchools[i].name<< endl; 
    cout << "tclass1=" << s._highSchools[i].class1<< endl; 
    cout << "tclass2=" << s._highSchools[i].class2<< endl; 
    cout << "tclass3=" << s._highSchools[i].class3<< endl; 
  } 
  cout << "juniorschool" << endl; 
  for (unsigned i=0; i< s._juniorSchools.size(); ++i) { 
    cout << "tid=" << s._juniorSchools[i].id << endl; 
    cout << "tyear=" << s._juniorSchools[i].year << endl; 
    cout << "tname=" << s._juniorSchools[i].name<< endl; 
  } 
  cout << "elmentaryschool" << endl; 
  for (unsigned i=0; i< s._elementarySchools.size(); ++i) { 
    cout << "tid=" << s._elementarySchools[i].id << endl; 
    cout << "tyear=" << s._elementarySchools[i].year << endl; 
    cout << "tname=" << s._elementarySchools[i].name<< endl; 
  }
}

bool ShowSearchCacheResult(Int2SearchCacheResultMap& map)
{
  std::cout<<"in Func showSearchCacheResult, map.size() is "<<map.size()<<std::endl;
    for(Int2SearchCacheResultMap::iterator itS=map.begin(); itS!=map.end(); ++itS)
    {
        SearchCacheResult& result = (*itS).second;
        cout << SEARCH_CACHE_ID   << " : " << result.generels[SEARCH_CACHE_ID] << endl;
        cout << SEARCH_CACHE_STATUS << " : " << result.generels[SEARCH_CACHE_STATUS] << endl;
        //cout << SEARCH_CACHE_ISVIP<< " : " << result.generels[SEARCH_CACHE_ISVIP] << endl;
        cout << SEARCH_CACHE_NAME << " : " << result.generels[SEARCH_CACHE_NAME] << endl;
        cout << SEARCH_CACHE_SIGNATURE << " : " << result.generels[SEARCH_CACHE_SIGNATURE] << endl;
        cout << SEARCH_CACHE_HEADURL << " : " << result.generels[SEARCH_CACHE_HEADURL] << endl;
        cout << SEARCH_CACHE_TINYURL << " : " << result.generels[SEARCH_CACHE_TINYURL] << endl;
        cout << SEARCH_CACHE_STAR << " : " << result.generels[SEARCH_CACHE_STAR] << endl;
        cout << SEARCH_CACHE_HOME_CITY   << " : " << result.generels[SEARCH_CACHE_HOME_CITY] << endl;
        cout << SEARCH_CACHE_HOME_PROVINCE << " : " << result.generels[SEARCH_CACHE_HOME_PROVINCE] << endl;
        cout << SEARCH_CACHE_STATUS_CONFIG << " : " << result.generels[SEARCH_CACHE_STATUS_CONFIG] << endl;
        cout << SEARCH_CACHE_BROWSE_CONFIG << " : " << result.generels[SEARCH_CACHE_BROWSE_CONFIG] << endl;

        cout << "Network" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_NETWORK].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_STAGE] << endl;
        }    

        cout << "Elementary School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR] << endl;
        }    

        cout << "Junior School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_YEAR] << endl;
        }    

        cout << "High School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_YEAR] << endl;
        }

        cout << "Colege" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_COLLEGE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_DEPARTMENT] << endl;
        }

        cout << "University" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_UNIVERSITY].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_DEPARTMENT] << endl;
        }

        cout << "Workplace" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_WORKPLACE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_NAME] << endl;
        }

        cout << "Region" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_REGION].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_PROVINCE] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_CITY] << endl;
        }
    }

    return true;

}

bool ShowSearchCacheResult(Int2ByteSeq& map)
{
  std::cout<<"in Func showSearchCacheResult, map.size() is "<<map.size()<<std::endl;
    for(Int2ByteSeq::iterator itS=map.begin(); itS!=map.end(); ++itS)
    {
      SearchMemCacheData data;
      data.Unserialize((const char*)(*itS).second.data(), (*itS).second.size());


        SearchCacheResult result = data.getProperties();
        cout << SEARCH_CACHE_ID   << " : " << result.generels[SEARCH_CACHE_ID] << endl;
        cout << SEARCH_CACHE_STATUS << " : " << result.generels[SEARCH_CACHE_STATUS] << endl;
        //cout << SEARCH_CACHE_ISVIP<< " : " << result.generels[SEARCH_CACHE_ISVIP] << endl;
        cout << SEARCH_CACHE_NAME << " : " << result.generels[SEARCH_CACHE_NAME] << endl;
        cout << SEARCH_CACHE_SIGNATURE << " : " << result.generels[SEARCH_CACHE_SIGNATURE] << endl;
        cout << SEARCH_CACHE_HEADURL << " : " << result.generels[SEARCH_CACHE_HEADURL] << endl;
        cout << SEARCH_CACHE_TINYURL << " : " << result.generels[SEARCH_CACHE_TINYURL] << endl;
        cout << SEARCH_CACHE_STAR << " : " << result.generels[SEARCH_CACHE_STAR] << endl;
        cout << SEARCH_CACHE_HOME_CITY   << " : " << result.generels[SEARCH_CACHE_HOME_CITY] << endl;
        cout << SEARCH_CACHE_HOME_PROVINCE << " : " << result.generels[SEARCH_CACHE_HOME_PROVINCE] << endl;
        cout << SEARCH_CACHE_STATUS_CONFIG << " : " << result.generels[SEARCH_CACHE_STATUS_CONFIG] << endl;
        cout << SEARCH_CACHE_BROWSE_CONFIG << " : " << result.generels[SEARCH_CACHE_BROWSE_CONFIG] << endl;

        cout << "Network" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_NETWORK].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_STAGE] << endl;
        }    

        cout << "Elementary School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR] << endl;
        }    

        cout << "Junior School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_YEAR] << endl;
        }    

        cout << "High School" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_YEAR] << endl;
        }

        cout << "Colege" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_COLLEGE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_DEPARTMENT] << endl;
        }

        cout << "University" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_UNIVERSITY].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_DEPARTMENT] << endl;
        }

        cout << "Workplace" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_WORKPLACE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_NAME] << endl;
        }

        cout << "Region" << endl;
        for (int i = 0; i<result.infos[SEARCH_CACHE_TYPE_REGION].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_PROVINCE] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_CITY] << endl;
        }
    }

    return true;

}


