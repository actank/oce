/*
 *
 * SearchMemCacheDataI.h
 *
 *  Created on: 2009-4-20
 *      Author: ning.zhao@opi-corp.com
 */

#include "SearchMemCacheData.h"
#include <boost/lexical_cast.hpp>
#include <ServiceI.h>
#include "LogWrapper.h"

using namespace std;
using namespace xce::searchcache;
using namespace MyUtil;
using namespace mysqlpp;
using namespace IceUtil;


SearchMemCacheData::SearchMemCacheData() {
  _id = 0;
  _star = 0;
  _status = 0;
  _stage = 0;
  _birthday_year = 0;
  _state = 0;
  _browse_config = 0;
  _status_config = 0;
  _auth = 0;
};

SearchCacheResult SearchMemCacheData::getProperties() {
	SearchCacheResult result;
	
	RWRecMutex::RLock lock(_dataMutex);
	result.generels[SEARCH_CACHE_ID] = boost::lexical_cast<string>(_id);
	result.generels[SEARCH_CACHE_STATUS] = boost::lexical_cast<string>(_status);
	result.generels[SEARCH_CACHE_NAME] = _name;
	result.generels[SEARCH_CACHE_SIGNATURE] = _signature;
	result.generels[SEARCH_CACHE_GENDER] = _gender;
	result.generels[SEARCH_CACHE_BIRTHDAY_YEAR] = boost::lexical_cast<string>(_birthday_year);
	result.generels[SEARCH_CACHE_HEADURL] = _headurl;
	result.generels[SEARCH_CACHE_TINYURL] = _tinyurl;
	result.generels[SEARCH_CACHE_STAR] = boost::lexical_cast<string>(_star);
	result.generels[SEARCH_CACHE_STATE] = boost::lexical_cast<string>(_state);
	result.generels[SEARCH_CACHE_SELECTED] = boost::lexical_cast<string>(0);
	result.generels[SEARCH_CACHE_STAGE] = boost::lexical_cast<string>(_stage);
	result.generels[SEARCH_CACHE_HOME_CITY] = boost::lexical_cast<string>(_home_city);
	result.generels[SEARCH_CACHE_HOME_PROVINCE] = boost::lexical_cast<string>(_home_province);

	result.generels[SEARCH_CACHE_STATUS_CONFIG] = boost::lexical_cast<string>(_status_config);
	result.generels[SEARCH_CACHE_BROWSE_CONFIG] = boost::lexical_cast<string>(_browse_config);
  result.generels[SEARCH_CACHE_AUTH] = boost::lexical_cast<string>(_auth);
  result.generels[SEARCH_CACHE_PROFILE_PRIVACY] = boost::lexical_cast<string>(_profile_privacy);
//	result.generels[SEARCH_CACHE_BASIC_CONFIG] = boost::lexical_cast<string>(_basic_config);
//	result.generels[SEARCH_CACHE_VIEW_COUNT] = boost::lexical_cast<string>(_viewcount);

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

BinStringPtr SearchMemCacheData::Serialize() const {
    try {
        BinStringPtr binString(new BinString());
        //SearchCacheData需要序列化的总的字段数
        int totalField = 0;
        binString->add(totalField);

        //序列化userid
        SearchCacheDataSerialize::SerializeToBinString(binString, USERID, _id, totalField);

        //序列化用户名
        SearchCacheDataSerialize::SerializeToBinString(binString, USERNAME, _name, totalField);

        //序列化性别
        SearchCacheDataSerialize::SerializeToBinString(binString, GENDER, _gender, totalField);

        //序列化signature
        SearchCacheDataSerialize::SerializeToBinString(binString, SIGNATURE, _signature, totalField);

        //序列化star
        SearchCacheDataSerialize::SerializeToBinString(binString, STAR, _star, totalField);

        //序列化status
        SearchCacheDataSerialize::SerializeToBinString(binString, STATUS, _status, totalField);

        //序列化state
        SearchCacheDataSerialize::SerializeToBinString(binString, STATE, _state, totalField);

        //序列化stage
        SearchCacheDataSerialize::SerializeToBinString(binString, STAGE, _stage, totalField);

        //序列化birthday_year
        SearchCacheDataSerialize::SerializeToBinString(binString, BIRTHYEAR, _birthday_year, totalField);

        //序列化birthday_month
        //SearchCacheDataSerialize::SerializeToBinString(binString, BIRTHMONTH, _birthday_month, totalField);

        //序列化birthday_day
        //SearchCacheDataSerialize::SerializeToBinString(binString, BIRTHDAY, _birthday_day, totalField);

        //序列化tinyurl
        SearchCacheDataSerialize::SerializeToBinString(binString, TINYURL, _tinyurl, totalField);

        //序列化headurl
        SearchCacheDataSerialize::SerializeToBinString(binString, HEADURL, _headurl, totalField);

        //序列化astrology
        //SearchCacheDataSerialize::SerializeToBinString(binString, ASTROLOGY, _astrology, totalField);

        //序列化browse_config
        SearchCacheDataSerialize::SerializeToBinString(binString, BROWSECONFIG, _browse_config, totalField);

        //序列化_status_config
        SearchCacheDataSerialize::SerializeToBinString(binString, STATUSCONFIG, _status_config, totalField);
        
        //序列化auth
        SearchCacheDataSerialize::SerializeToBinString(binString, AUTH, _auth, totalField);
  
        //序列化profile_privacy
        SearchCacheDataSerialize::SerializeToBinString(binString, PROFILEPRIVACY, _profile_privacy, totalField);




        //序列化basic_config
        //SearchCacheDataSerialize::SerializeToBinString(binString, BASICCONFIG, _basic_config, totalField);

        //序列化viewcount
        //SearchCacheDataSerialize::SerializeToBinString(binString, VIEWCOUNT, _viewcount, totalField);

        //序列化home_city
        SearchCacheDataSerialize::SerializeToBinString(binString, HOMECITY, _home_city, totalField);

        //序列化home_province
        SearchCacheDataSerialize::SerializeToBinString(binString, HOMEPROVINCE, _home_province, totalField);

        int total_size = 0;
        //序列化network vector
        if (_networks.size() > 0) {
            int vec_size = 0;
            //获得neworks所占用的总的字节数
            total_size = sizeof((int)_networks.size());
            BinStringPtr bstr(new BinString());
            for(vector<Network>::const_iterator it = _networks.begin();
                    it != _networks.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof((*it).stage);
                total_size += sizeof(int) + (*it).name.length();
                bstr->add((*it).id);
                bstr->add((*it).stage);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
            }

            SearchCacheDataSerialize::SerializeToBinString(binString, NETWORKS, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化region vector
        if (_regions.size() > 0) {
            int vec_size = 0;
            //获得regions所占用的总的字节数
            total_size = sizeof((int)_regions.size());
            BinStringPtr bstr(new BinString());
            for(vector<Region>::const_iterator it = _regions.begin();
                    it != _regions.end(); ++it) {
                if ((*it).city.empty() && (*it).province.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof(int) + (*it).city.length();
                total_size += sizeof(int) + (*it).province.length();
                bstr->add((*it).id);
                bstr->add((int)(*it).city.length());
                bstr->add((*it).city);
                bstr->add((int)(*it).province.length());
                bstr->add((*it).province);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, REGIONS, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化workplace vector
        if (_workplaces.size() > 0) {
            int vec_size = 0;
            //获得workplaces所占用的总的字节数
            total_size = sizeof((int)_workplaces.size());
            BinStringPtr bstr(new BinString());
            for(vector<Workplace>::const_iterator it = _workplaces.begin();
                    it != _workplaces.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof(int) + (*it).name.length();
                bstr->add((*it).id);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, WORKPLACES, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化university vector
        if (_universities.size() > 0) {
            int vec_size = 0;
            //获得universities所占用的总的字节数
            total_size = sizeof((int)_universities.size());
            BinStringPtr bstr(new BinString());
            for(vector<University>::const_iterator it = _universities.begin();
                    it != _universities.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof((*it).year);
                total_size += sizeof(int) + (*it).name.length();
                total_size += sizeof(int) + (*it).department.length();
                total_size += sizeof(int) + (*it).dorm.length();
                bstr->add((*it).id);
                bstr->add((*it).year);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
                bstr->add((int)(*it).department.length());
                bstr->add((*it).department);
                bstr->add((int)(*it).dorm.length());
                bstr->add((*it).dorm);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, UNIVERSITIES, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化college vector
        if (_colleges.size() > 0 ) {
            int vec_size = 0;
            //获得colleges所占用的总的字节数
            total_size = sizeof((int)_colleges.size());
            BinStringPtr bstr(new BinString());
            for(vector<College>::const_iterator it = _colleges.begin();
                    it != _colleges.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof((*it).year);
                total_size += sizeof(int) + (*it).name.length();
                total_size += sizeof(int) + (*it).department.length();
                bstr->add((*it).id);
                bstr->add((*it).year);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
                bstr->add((int)(*it).department.length());
                bstr->add((*it).department);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, COLLEGES, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化high school vector
        if (_highSchools.size() > 0) {
            int vec_size = 0;
            //获得highSchool所占用的总的字节数
            total_size = sizeof((int)_highSchools.size());
            BinStringPtr bstr(new BinString());
            for(vector<HighSchool>::const_iterator it = _highSchools.begin();
                    it != _highSchools.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof((*it).year);
                total_size += sizeof(int) + (*it).name.length();
                total_size += sizeof(int) + (*it).class1.length();
                total_size += sizeof(int) + (*it).class2.length();
                total_size += sizeof(int) + (*it).class3.length();
                bstr->add((*it).id);
                bstr->add((*it).year);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
                bstr->add((int)(*it).class1.length());
                bstr->add((*it).class1);
                bstr->add((int)(*it).class2.length());
                bstr->add((*it).class2);
                bstr->add((int)(*it).class3.length());
                bstr->add((*it).class3);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, HIGHSCHOOLS, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化junior school vector
        if (_juniorSchools.size() > 0) {
            int vec_size = 0;
            //获得juniorSchool所占用的总的字节数
            total_size = sizeof((int)_juniorSchools.size());
            BinStringPtr bstr(new BinString());
            for(vector<JuniorSchool>::const_iterator it = _juniorSchools.begin();
                    it != _juniorSchools.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof((*it).year);
                total_size += sizeof(int) + (*it).name.length();
                bstr->add((*it).id);
                bstr->add((*it).year);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, JUNIORSCHOOLS, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //序列化elementary school vector
        if (_elementarySchools.size() > 0) {
            int vec_size = 0;
            //获得elementarySchool所占用的总的字节数
            total_size = sizeof((int)_elementarySchools.size());
            BinStringPtr bstr(new BinString());
            for(vector<ElementarySchool>::const_iterator it = _elementarySchools.begin();
                    it != _elementarySchools.end(); ++it) {
                if ((*it).name.empty()) continue;
                vec_size++;
                total_size += sizeof((*it).id);
                total_size += sizeof((*it).year);
                total_size += sizeof(int) + (*it).name.length();
                bstr->add((*it).id);
                bstr->add((*it).year);
                bstr->add((int)(*it).name.length());
                bstr->add((*it).name);
            }
            SearchCacheDataSerialize::SerializeToBinString(binString, ELEMENTARYSCHOOLS, total_size, vec_size, 
                    *bstr.get(), totalField); 
        }

        //添加总有效字节数
        binString->add(binString->getBufferLen());
        //回写总字段数
        char*p;
        int len;
        binString->getBuf(&p, &len);
        if (len > 0) {
            int *pBuf = (int*)p;
            *pBuf = totalField;
        }
        return binString;
    } catch (...) {
        BinStringPtr binString((BinString*)0);
        MCE_WARN("serialize id =" << _id << " failed! " );
        return binString;
    }
}

bool SearchMemCacheData::Unserialize(const char *buf, unsigned int lens) {
    const char* p = buf;
    int total = 0; 

    if (buf == NULL || lens <= 0) return false;

    //获得总的有效字节数
    total  = *(int*)(p+lens-sizeof(int));

    if (lens-sizeof(int) != total) {
        MCE_WARN("total lens is error! total=" << total << "lens = " << lens);
        return false;
    }

    if (total <= 0) return false;

    int count = 0;
    int vector_size = 0;

    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, count))
        return false;

    for(int i=0; i<count; ++i) {
        //获得标签
        char lable = 0;
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, lable))
            return false;

        //获得长度
        int lens = 0;
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, lens))
            return false;

        if (lens < 0)
            continue;

        if (lens > total)
            return false;

        switch(lable) {
            case USERID:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _id))
                    return false;
                break;
            case USERNAME: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _name))
                    return false;
                break;
            case GENDER:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _gender))
                    return false;
                break;
            case SIGNATURE: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _signature))
                    return false;
                break;
            case STAR: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _star))
                    return false;
                break;
            case STATUS:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _status))
                    return false;
                break;
            case STATE:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _state))
                    return false;
                break;
            case STAGE:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _stage))
                    return false;
                break;
            case BIRTHYEAR:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _birthday_year))
                    return false;
                break;
            /*
            case BIRTHMONTH:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _birthday_month))
                    return false;
                break;
            case BIRTHDAY:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _birthday_day))
                    return false;
                break;
             */
            case TINYURL: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _tinyurl))
                    return false;
                break;
            case HEADURL: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _headurl))
                    return false;
                break;
            /*
            case ASTROLOGY: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _astrology))
                    return false;
                break;
             */
            case BROWSECONFIG: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _browse_config))
                    return false;
                break;
            case STATUSCONFIG: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _status_config))
                    return false;
                break;
            case AUTH: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _auth))
                  return false;
                break;
            case PROFILEPRIVACY: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _profile_privacy))
                  return false;
                break;

            /*
            case BASICCONFIG: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _basic_config))
                    return false;
                break;
            case VIEWCOUNT: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _viewcount))
                    return false;
                break;
            */
            case HOMECITY: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _home_city))
                    return false;
                break;
            case HOMEPROVINCE: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _home_province))
                    return false;
                break;
            case NETWORKS: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    Network network;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, network.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, network.stage))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, network.name))
                        return false;
                    _networks.push_back(network);
                }
                break;
            case REGIONS:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    Region region;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, region.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, region.city))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, region.province))
                        return false;
                    _regions.push_back(region);
                }
                break;
            case WORKPLACES: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    Workplace workplace;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, workplace.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, workplace.name))
                        return false;
                    _workplaces.push_back(workplace);
                }
                break;
            case UNIVERSITIES: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    University university;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.year))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.name))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.department))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.dorm))
                        return false;
                    _universities.push_back(university);
                }
                break;
            case COLLEGES: 
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    College college;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.year))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.name))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.department))
                        return false;
                    _colleges.push_back(college);
                }
                break;
            case HIGHSCHOOLS:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    HighSchool highSchool;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.year))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.name))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.class1))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.class2))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.class3))
                        return false;
                    _highSchools.push_back(highSchool);
                }
                break;
            case JUNIORSCHOOLS:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    JuniorSchool juniorSchool;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, juniorSchool.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, juniorSchool.year))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, juniorSchool.name))
                        return false;
                    _juniorSchools.push_back(juniorSchool);
                }
                break;
            case ELEMENTARYSCHOOLS:
                if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
                    return false;
                for(int i = 0; i<vector_size; ++i) {
                    ElementarySchool elementarySchool;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, elementarySchool.id))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, elementarySchool.year))
                        return false;
                    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, elementarySchool.name))
                        return false;
                    _elementarySchools.push_back(elementarySchool);
                }
                break;
            default: //跳过这个指段
                p += lens;
        }
    }
    return true;
}

