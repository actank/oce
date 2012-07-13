/*
 * SearchCacheData.h
 *
 *  Created on: 2009-4-20
 *      Author: ning.zhao@opi-corp.com
 */

#ifndef _SEARCHCACHEDATA_H_
#define _SEARCHCACHEDATA_H_

#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>
#include <ServiceI.h>

#include <SearchCache.h>
#include "BinString.h"
#include "SearchCacheDataSerialize.h"
#include "SearchMemCacheData.h"

namespace xce {
namespace searchcache {

struct Network{
	int id;
	int stage;
	std::string name;
};

struct ElementarySchool{
	int id;
	int year;
	string name;
};

struct JuniorSchool{
	int id;
	int year;
	string name;
};

struct HighSchool{
	int id;
	int year;
	string name;
    string class1;
    string class2;
    string class3;
};

struct College{
	int id;
	int year;
	string name;
	string department;
};

struct University{
	int id;
	int year;
	string name;
	string department;
	string dorm;
};

struct Workplace{
	int id;
	string name;
};

struct Region{
	int id;
	string city;
	string province;
};

class SearchCacheData: virtual public Ice::Object {
public:
	SearchCacheData();
  SearchCacheData& operator=(const SearchCacheData& other);
	SearchCacheResult getProperties();
  void setElementarySchoolProperties(mysqlpp::Row& row);
  void setJuniorSchoolProperties(mysqlpp::Row& row);
  void setHighSchoolProperties(mysqlpp::Row& row);
  void setCollegeProperties(mysqlpp::Row& row);
  void setUniversityProperties(mysqlpp::Row& row);
  void setWorkplaceProperties(mysqlpp::Row& row);
  void setRegionProperties(mysqlpp::Row& row);
  void setNetworkProperties(mysqlpp::Row& row);
  void setElementarySchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setJuniorSchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setHighSchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setCollegePropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setUniversityPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setWorkplacePropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setRegionPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
  void setNetworkPropertiesAccordingVector(std::vector<mysqlpp::Row>&);

  void setConfigProperties(mysqlpp::Row& row);
  void setViewCountProperties(mysqlpp::Row& row);
  void setBasicProperties(mysqlpp::Row& row);
  void setUserUrlProperties(mysqlpp::Row& row);
  void setInterestProperties(mysqlpp::Row& row);
  void setMusicProperties(mysqlpp::Row& row);
  void setMovieProperties(mysqlpp::Row& row);
  void setGameProperties(mysqlpp::Row& row);
  void setComicProperties(mysqlpp::Row& row);
  void setSportProperties(mysqlpp::Row& row);
  void setBookProperties(mysqlpp::Row& row);
  void setSocietyProperties(mysqlpp::Row& row);
  void setLastlogintimeProperties(mysqlpp::Row& row);
  void setUserStateProperties(mysqlpp::Row& row);
  void setUserStatusProperties(mysqlpp::Row& row);
  void setUserStageProperties(mysqlpp::Row& row);
  void setUserNameProperties(mysqlpp::Row& row);
  void setUserBornProperties(mysqlpp::Row& row);
  void setUserFondProperties(mysqlpp::Row& row);
  void setUserAuthProperties(mysqlpp::Row& row);
  std::string splitProfilePrivacy(std::string profile_privacy);
public:
    //基本信息
	int _id;
	std::string _name;
	std::string _gender;
  std::string _signature;
  int _star;
  int _status;
  int _stage;
  int _birthday_year;
  int _birthday_month;
  int _birthday_day;
  int _state;
  std::string _lastlogintime;
  std::string _tinyurl;
  std::string _headurl;
  std::string _astrology;
  int _browse_config;
  int _status_config;
  int _basic_config;
  int _auth;
  int _viewcount;
  std::string _home_city;
  std::string _home_province;
  std::string _interest;
  std::string _music;
  std::string _movie;
  std::string _game;
  std::string _comic;
  std::string _sport;
  std::string _book;
  std::string _society;
  std::string _profile_privacy;

  std::vector<Network> _networks;
  std::vector<Region> _regions;
  std::vector<Workplace> _workplaces;
  std::vector<University> _universities;
  std::vector<College> _colleges;
  std::vector<HighSchool> _highSchools;
  std::vector<JuniorSchool> _juniorSchools;
  std::vector<ElementarySchool> _elementarySchools;
  IceUtil::RWRecMutex _dataMutex;

  BinStringPtr Serialize() const;
  bool Unserialize(const char*buf, unsigned int lens);
};

typedef IceUtil::Handle<SearchCacheData> SearchCacheDataPtr;
typedef std::vector<SearchCacheDataPtr> SearchCacheDataSeq;

}
}

#endif
