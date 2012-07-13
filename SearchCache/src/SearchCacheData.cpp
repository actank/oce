/*
 * SearchCacheDataI.h
 *
 *  Created on: 2009-4-20
 *      Author: ning.zhao@opi-corp.com
 */

#include "SearchCacheData.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace xce::searchcache;
using namespace MyUtil;
using namespace mysqlpp;
using namespace IceUtil;


SearchCacheData::SearchCacheData() {
}

SearchCacheResult SearchCacheData::getProperties() {
	SearchCacheResult result;
	
	RWRecMutex::RLock lock(_dataMutex);
	result.generels[SEARCH_CACHE_ID] = boost::lexical_cast<string>(_id);
	result.generels[SEARCH_CACHE_STATUS] = boost::lexical_cast<string>(_status);
	result.generels[SEARCH_CACHE_NAME] = _name;
	result.generels[SEARCH_CACHE_SIGNATURE] = _signature;
	result.generels[SEARCH_CACHE_SELECTED] = boost::lexical_cast<string>(_selected);
	result.generels[SEARCH_CACHE_HEADURL] = _headurl;
	result.generels[SEARCH_CACHE_TINYURL] = _tinyurl;
	result.generels[SEARCH_CACHE_STAR] = boost::lexical_cast<string>(_star);
	result.generels[SEARCH_CACHE_HOME_CITY] = boost::lexical_cast<string>(_home_city);
	result.generels[SEARCH_CACHE_HOME_PROVINCE] = boost::lexical_cast<string>(_home_province);

	result.generels[SEARCH_CACHE_STATUS_CONFIG] = boost::lexical_cast<string>(_status_config);
	result.generels[SEARCH_CACHE_BROWSE_CONFIG] = boost::lexical_cast<string>(_browse_config);

	result.infos[SEARCH_CACHE_TYPE_NETWORK]=vector<map<string,string> >();
	result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL] = vector<map<string,string> >();
	result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL] = vector<map<string,string> >();
	result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL] = vector<map<string, string> >();
	result.infos[SEARCH_CACHE_TYPE_COLLEGE] = vector<map<string,string> >();
	result.infos[SEARCH_CACHE_TYPE_UNIVERSITY] = vector<map<string,string> >();
	result.infos[SEARCH_CACHE_TYPE_WORKPLACE] = vector<map<string,string> >();
	result.infos[SEARCH_CACHE_TYPE_REGION] = vector<map<string,string> >();
	
	for(vector<Network>::iterator it = _networks.begin(); it != _networks.end(); ++it) {
		Str2StrMap networkmap;
		networkmap[SEARCH_CACHE_NETWORK_ID] = boost::lexical_cast<string>(it->id);
		networkmap[SEARCH_CACHE_NETWORK_NAME] = it->name;
		networkmap[SEARCH_CACHE_NETWORK_STAGE] = boost::lexical_cast<string>(it->stage);
		result.infos[SEARCH_CACHE_TYPE_NETWORK].push_back(networkmap);
	}
	
	for(vector<ElementarySchool>::iterator it = _elementarySchools.begin(); it != _elementarySchools.end(); ++it){
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_ELEMENTARY_SCHOOL_ID] = boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME] = it->name;
		schoolmap[SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR] = boost::lexical_cast<string>(it->year);
		result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL].push_back(schoolmap);
	}
	
	for(vector<JuniorSchool>::iterator it = _juniorSchools.begin(); it != _juniorSchools.end(); ++it) {
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_JUNIOR_SCHOOL_ID] = boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_JUNIOR_SCHOOL_NAME] = it->name;
		schoolmap[SEARCH_CACHE_JUNIOR_SCHOOL_YEAR] = boost::lexical_cast<string>(it->year);
		result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL].push_back(schoolmap);
	}
	
	for(vector<HighSchool>::iterator it = _highSchools.begin(); it != _highSchools.end(); ++it) {
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_HIGH_SCHOOL_ID] = boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_HIGH_SCHOOL_NAME] = it->name;
		schoolmap[SEARCH_CACHE_HIGH_SCHOOL_YEAR] = boost::lexical_cast<string>(it->year);
		result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL].push_back(schoolmap);
	}
	
	for(vector<College>::iterator it = _colleges.begin(); it != _colleges.end(); ++it) {
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_COLLEGE_ID] = boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_COLLEGE_NAME] = it->name;
		schoolmap[SEARCH_CACHE_COLLEGE_DEPARTMENT] = it->department;
		schoolmap[SEARCH_CACHE_COLLEGE_YEAR] = it->year;
		result.infos[SEARCH_CACHE_TYPE_COLLEGE].push_back(schoolmap);
	}
	
	for(vector<University>::iterator it=_universities.begin(); it!=_universities.end(); ++it){
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_UNIVERSITY_ID] = boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_UNIVERSITY_NAME] = it->name;
		schoolmap[SEARCH_CACHE_UNIVERSITY_YEAR] = boost::lexical_cast<string>(it->year);
		schoolmap[SEARCH_CACHE_UNIVERSITY_DEPARTMENT] = it->department;
		result.infos[SEARCH_CACHE_TYPE_UNIVERSITY].push_back(schoolmap);
	}
	
	for(vector<Workplace>::iterator it=_workplaces.begin(); it!=_workplaces.end(); ++it){
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_WORKPLACE_ID]=boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_WORKPLACE_NAME]=it->name;
		result.infos[SEARCH_CACHE_TYPE_WORKPLACE].push_back(schoolmap);
	}
	
	for(vector<Region>::iterator it=_regions.begin();it!=_regions.end();++it){
		Str2StrMap schoolmap;
		schoolmap[SEARCH_CACHE_REGION_ID]=boost::lexical_cast<string>(it->id);
		schoolmap[SEARCH_CACHE_REGION_CITY]=it->city;
		schoolmap[SEARCH_CACHE_REGION_PROVINCE]=it->province;
		result.infos[SEARCH_CACHE_TYPE_REGION].push_back(schoolmap);
	}

	return result;
}

void SearchCacheData::setElementarySchoolProperties(Row& row, const char* fieldElementarySchoolId,
		const char* fieldElementarySchoolYear, const char* fieldElementarySchoolName) {
	ElementarySchool school;
	school.id = (int) row[fieldElementarySchoolId];
	school.year = (int) row[fieldElementarySchoolYear];
#ifndef NEWARCH
	school.name = row[fieldElementarySchoolName].get_string();
#else
	school.name = row[fieldElementarySchoolName].c_str();
#endif
	RWRecMutex::WLock lock(_dataMutex);
	_elementarySchools.push_back(school);
}

void SearchCacheData::setJuniorSchoolProperties(Row& row, const char* fieldJuniorSchoolId,
		const char* fieldJuniorSchoolYear, const char* fieldJuniorSchoolName) {
	JuniorSchool school;
	school.id = (int) row[fieldJuniorSchoolId];
	school.year = (int) row[fieldJuniorSchoolYear];
#ifndef NEWARCH
	school.name = row[fieldJuniorSchoolName].get_string();
#else
	school.name = row[fieldJuniorSchoolName].c_str();
#endif
	RWRecMutex::WLock lock(_dataMutex);
	_juniorSchools.push_back(school);
}

void SearchCacheData::setHighSchoolProperties(Row& row, const char* fieldHighSchoolId,
		const char* fieldHighSchoolYear, const char* fieldHighSchoolName) {
	HighSchool school;
	school.id = (int) row[fieldHighSchoolId];
	school.year = (int) row[fieldHighSchoolYear];
#ifndef NEWARCH
	school.name = row[fieldHighSchoolName].get_string();
#else
	school.name = row[fieldHighSchoolName].c_str();
#endif
	RWRecMutex::WLock lock(_dataMutex);
	_highSchools.push_back(school);
}

void SearchCacheData::setCollegeProperties(Row& row, const char* fieldCollegeId, const char* fieldCollegeName,
		const char* fieldCollegeDepartment, const char* fieldCollegeYear) {
	College school;
	school.id = (int) row[fieldCollegeId];
	school.year = (int) row[fieldCollegeYear];
#ifndef NEWARCH
	school.name = row[fieldCollegeName].get_string();
	school.department = row[fieldCollegeDepartment].get_string();
#else
	school.name = row[fieldCollegeName].c_str();
	school.department = row[fieldCollegeDepartment].c_str();
#endif
	RWRecMutex::WLock lock(_dataMutex);
	_colleges.push_back(school);
}

void SearchCacheData::setUniversityProperties(Row& row, const char* fieldUniversityId, const char* fieldUniversityYear,
		const char* fieldUniversityName, const char* fieldUniversityDepartment) {
		University school;
		school.id = (int) row[fieldUniversityId];
		school.year = (int) row[fieldUniversityYear];
#ifndef NEWARCH
		school.name = row[fieldUniversityName].get_string();
		school.department = row[fieldUniversityDepartment].get_string();
#else
		school.name = row[fieldUniversityName].c_str();
		school.department = row[fieldUniversityDepartment].c_str();
#endif
	RWRecMutex::WLock lock(_dataMutex);
	_universities.push_back(school);
}

void SearchCacheData::setWorkplaceProperties(Row& row, const char* fieldWorkplaceId, const char* fieldWorkplaceName) {
		Workplace workplace;
		workplace.id = (int) row[fieldWorkplaceId];
#ifndef NEWARCH
		workplace.name = row[fieldWorkplaceName].get_string();
#else
		workplace.name = row[fieldWorkplaceName].c_str();
#endif
		RWRecMutex::WLock lock(_dataMutex);
		_workplaces.push_back(workplace);
}

void SearchCacheData::setRegionProperties(Row& row, const char* fieldRegionId, const char* fieldCity, const char* fieldProvince) {
		Region region;
		region.id = (int) row[fieldRegionId];
#ifndef NEWARCH
		region.city = row[fieldCity].get_string();
		region.province = row[fieldProvince].get_string();
#else
		region.city = row[fieldCity].c_str();
		region.province = row[fieldProvince].c_str();
#endif
		RWRecMutex::WLock lock(_dataMutex);
		_regions.push_back(region);
}

void SearchCacheData::setNetworkProperties(Row& row, const char* fieldNetworkId,
		const char* fieldNetworkName, const char* fieldNetworkStage) {
	Network network;
	network.id = (int) row[fieldNetworkId];
	network.stage = (int) row[fieldNetworkStage];
#ifndef NEWARCH
	network.name = row[fieldNetworkName].get_string();
#else
	network.name = row[fieldNetworkName].c_str();
#endif
	RWRecMutex::WLock lock(_dataMutex);
	_networks.push_back(network);
}

void SearchCacheData::setConfigProperties(Row& row, const char* fieldBrowseConfig,
		const char* fieldStatusConfig) {
	RWRecMutex::WLock lock(_dataMutex);
	_browse_config = (int) row[fieldBrowseConfig];
	_status_config = (int) row[fieldStatusConfig];
}

void SearchCacheData::setUsersProperties(Row& row, const char* fieldSignature){
	RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
	_signature = row[fieldSignature].get_string();
#else
	_signature = row[fieldSignature].c_str();
#endif
}

void SearchCacheData::setUserUrlProperties(Row& row,  
		const char* fieldTinyUrl, const char* fieldHeadUrl) {
	RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
	_headurl = row[fieldHeadUrl].get_string();
	_tinyurl = row[fieldTinyUrl].get_string();
#else
	_headurl = row[fieldHeadUrl].c_str();
	_tinyurl = row[fieldTinyUrl].c_str();
#endif
}

void SearchCacheData::setBasicProperties(Row& row, const char* fieldName, 
		const char* fieldStar, const char* fieldSelected, 
		const char* fieldStatus, const char* fieldHomeCity,
		const char* fieldHomeProvince) {
	RWRecMutex::WLock lock(_dataMutex);
	_selected = (int) row[fieldSelected];
	_status = (int) row[fieldStatus];
	_star = (int) row[fieldStar];
#ifndef NEWARCH
	_name = row[fieldName].get_string();
	_home_city= row[fieldHomeCity].get_string();
	_home_province=row[fieldHomeProvince].get_string();
#else
	_name = row[fieldName].c_str();
	_home_city=row[fieldHomeCity].c_str();
	_home_province=row[fieldHomeProvince].c_str();
#endif
}

