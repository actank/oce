#include "show.h"
bool ShowSearchMemCacheData(const SearchMemCacheData& s)
{
  std::cout <<"id=" << s._id << std::endl; 
  std::cout << "name=" << s._name << std::endl; 
  std::cout << "gender=" << s._gender<< std::endl; 
  std::cout << "signature=" << s._signature << std::endl; 
  std::cout << "star=" << s._star << std::endl; 
  std::cout << "status=" << s._status << std::endl; 
  std::cout << "stage=" << s._stage<< std::endl; 
  std::cout << "birthday_year=" << s._birthday_year<< std::endl; 
  std::cout << "tinyurl=" << s._tinyurl << std::endl; 
  std::cout << "headurl=" << s._headurl << std::endl; 
  std::cout << "browse_config=" << s._browse_config << std::endl; 
  std::cout << "status_config=" << s._status_config << std::endl; 
  std::cout << "home_city=" << s._home_city << std::endl; 
  std::cout << "home_province=" << s._home_province << std::endl; 
  std::cout << "auth=" <<s._auth<<std::endl; 
  std::cout << "profile_privacy=" << s._profile_privacy<<std::endl; 
  std::cout << "network" << std::endl; 
  for (size_t i=0; i< s._networks.size(); ++i) { 
    std::cout << "tid=" << s._networks[i].id << std::endl; 
    std::cout << "tstage=" << s._networks[i].stage << std::endl; 
    std::cout << "tname=" << s._networks[i].name<< std::endl; 
  } 
  std::cout << "regions" << std::endl; 
  for (size_t i=0; i< s._regions.size(); ++i) { 
    std::cout << "tid=" << s._regions[i].id << std::endl; 
    std::cout << "tcity=" << s._regions[i].city<< std::endl; 
    std::cout << "tprovince=" << s._regions[i].province<< std::endl; 
  } 
  std::cout << "workplace" << std::endl; 
  for (size_t i=0; i< s._workplaces.size(); ++i) { 
    std::cout << "tid=" << s._workplaces[i].id << std::endl; 
    std::cout << "tname=" << s._workplaces[i].name<< std::endl; 
  } 
  std::cout << "university" << std::endl; 
  for (size_t i=0; i< s._universities.size(); ++i) { 
    std::cout << "tid=" << s._universities[i].id << std::endl; 
    std::cout << "tyear=" << s._universities[i].year << std::endl; 
    std::cout << "tname=" << s._universities[i].name<< std::endl; 
    std::cout << "tdepartment=" << s._universities[i].department << std::endl; 
    std::cout << "tdorm=" << s._universities[i].dorm<< std::endl; 
  } 
  std::cout << "college" << std::endl; 
  for (size_t i=0; i< s._colleges.size(); ++i) { 
    std::cout << "tid=" << s._colleges[i].id << std::endl; 
    std::cout << "tyear=" << s._colleges[i].year << std::endl; 
    std::cout << "tname=" << s._colleges[i].name<< std::endl; 
    std::cout << "tdepartment=" << s._colleges[i].department << std::endl; 
  } 
  std::cout << "highschool" << std::endl; 
  for (size_t i=0; i< s._highSchools.size(); ++i) { 
    std::cout << "tid=" << s._highSchools[i].id << std::endl; 
    std::cout << "tyear=" << s._highSchools[i].year << std::endl; 
    std::cout << "tname=" << s._highSchools[i].name<< std::endl; 
    std::cout << "tclass1=" << s._highSchools[i].class1<< std::endl; 
    std::cout << "tclass2=" << s._highSchools[i].class2<< std::endl; 
    std::cout << "tclass3=" << s._highSchools[i].class3<< std::endl; 
  } 
  std::cout << "juniorschool" << std::endl; 
  for (size_t i=0; i< s._juniorSchools.size(); ++i) { 
    std::cout << "tid=" << s._juniorSchools[i].id << std::endl; 
    std::cout << "tyear=" << s._juniorSchools[i].year << std::endl; 
    std::cout << "tname=" << s._juniorSchools[i].name<< std::endl; 
  } 
  std::cout << "elmentaryschool" << std::endl; 
  for (size_t i=0; i< s._elementarySchools.size(); ++i) { 
    std::cout << "tid=" << s._elementarySchools[i].id << std::endl; 
    std::cout << "tyear=" << s._elementarySchools[i].year << std::endl; 
    std::cout << "tname=" << s._elementarySchools[i].name<< std::endl; 
  } 
  return true;
}

bool ShowSearchCacheData(const SearchCacheData& data) {
    const std::string SEARCH_CACHE_TYPE_NETWORK = "network";
    const std::string SEARCH_CACHE_TYPE_JUNIOR_SCHOOL = "junior_school";
    const std::string SEARCH_CACHE_TYPE_HIGH_SCHOOL = "high_school";
    const std::string SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL = "elementary_school";
    const std::string SEARCH_CACHE_TYPE_COLLEGE = "college";
    const std::string SEARCH_CACHE_TYPE_UNIVERSITY = "university";
    const std::string SEARCH_CACHE_TYPE_WORKPLACE = "workplace";
    const std::string SEARCH_CACHE_TYPE_REGION = "region";

    const std::string SEARCH_CACHE_ID = "user_basic.id";
    const std::string SEARCH_CACHE_NAME = "name";
    const std::string SEARCH_CACHE_SIGNATURE = "signature";
    const std::string SEARCH_CACHE_STAR = "star";
    const std::string SEARCH_CACHE_STATE= "state";
    const std::string SEARCH_CACHE_SELECTED = "selected";
    const std::string SEARCH_CACHE_GENDER = "gender";
    const std::string SEARCH_CACHE_BIRTHDAY_YEAR = "birth_year";
    const std::string SEARCH_CACHE_STATUS = "status";
    const std::string SEARCH_CACHE_TINYURL = "tinyurl";
    const std::string SEARCH_CACHE_HEADURL = "headurl";
    const std::string SEARCH_CACHE_BROWSE_CONFIG = "browse_config";
    const std::string SEARCH_CACHE_STATUS_CONFIG = "status_config";
    const std::string SEARCH_CACHE_BASIC_CONFIG  = "basic_config";
    const std::string SEARCH_CACHE_VIEW_COUNT  = "viewcount";
    const std::string SEARCH_CACHE_HOME_CITY = "home_city";
    const std::string SEARCH_CACHE_HOME_PROVINCE = "home_province";
    const std::string SEARCH_CACHE_NETWORK_ID = "network_id";
    const std::string SEARCH_CACHE_NETWORK_NAME = "network_name";
    const std::string SEARCH_CACHE_NETWORK_STAGE = "network_history.stage";
    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_ID = "elementary_school_info.elementary_school_id";
    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR = "elementary_school_info.elementary_school_year";
    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME = "elementary_school_info.elementary_school_name";
    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_ID = "junior_high_school_info.junior_high_school_id";
    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_NAME = "junior_high_school_info.junior_high_school_name";
    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_YEAR = "junior_high_school_info.junior_high_school_year";
    const std::string SEARCH_CACHE_HIGH_SCHOOL_YEAR = "enroll_year";
    const std::string SEARCH_CACHE_HIGH_SCHOOL_NAME = "high_school_name";
    const std::string SEARCH_CACHE_HIGH_SCHOOL_ID = "high_school_id";
    const std::string SEARCH_CACHE_COLLEGE_ID = "college_info.college_id";
    const std::string SEARCH_CACHE_COLLEGE_NAME ="college_info.college_name";
    const std::string SEARCH_CACHE_COLLEGE_DEPARTMENT = "college_info.department";
    const std::string SEARCH_CACHE_COLLEGE_YEAR = "college_info.enroll_year";
    const std::string SEARCH_CACHE_UNIVERSITY_ID = "university_id";
    const std::string SEARCH_CACHE_UNIVERSITY_NAME = "university_name";
    const std::string SEARCH_CACHE_UNIVERSITY_YEAR = "enroll_year";
    const std::string SEARCH_CACHE_UNIVERSITY_DEPARTMENT = "department";
    const std::string SEARCH_CACHE_WORKPLACE_ID = "workplace_id";
    const std::string SEARCH_CACHE_WORKPLACE_NAME = "workplace_name";
    const std::string SEARCH_CACHE_REGION_ID = "region_id";
    const std::string SEARCH_CACHE_REGION_PROVINCE = "province_name";
    const std::string SEARCH_CACHE_REGION_CITY = "city_name";

    try{
        cout<<" " << SEARCH_CACHE_ID   << " : " << data._id << endl;
        cout<<" "<< SEARCH_CACHE_STATUS << " : " << data._status << endl;
        cout<<" "<< SEARCH_CACHE_NAME << " : " << data._name  << endl;
        cout<<" "<< SEARCH_CACHE_SIGNATURE << " : " << data._signature << endl;
        cout<<" "<< SEARCH_CACHE_HEADURL << " : " << data._headurl  << endl;
        cout<<" "<< SEARCH_CACHE_TINYURL << " : " << data._tinyurl << endl;
        cout<<" "<< SEARCH_CACHE_STAR << " : " << data._star << endl;
        cout<<" "<< SEARCH_CACHE_HOME_CITY   << " : " << data._home_city << endl;
        cout<<" "<< SEARCH_CACHE_HOME_PROVINCE << " : " << data._home_province  << endl;
        cout<<" "<< SEARCH_CACHE_STATUS_CONFIG << " : " << data._status_config  << endl;
        cout<<" "<< SEARCH_CACHE_BROWSE_CONFIG << " : " << data._browse_config << endl;
        cout<<" "<< "profile_privacy" << " : " << data._profile_privacy << endl;
        cout<<" "<< "birthday_year" << " : " << data._birthday_year << endl;
        cout<<" "<< "birthday_month" << " : " << data._birthday_month << endl;
        cout<<" "<< "birthday_day" << " : " << data._birthday_day << endl;
        cout<<" "<< "state" << " : " << data._state << endl;
        cout<<" "<< "stage" << " : " << data._stage << endl;
        cout<<" "<< "lastlogintime" << " : " << data._lastlogintime << endl;
        cout<<" "<< "astrology" << " : " << data._astrology << endl;
        cout<<" "<< "basic_config" << " : " << data._basic_config << endl;
        cout<<" "<< "gender" << " : " << data._gender << endl;
        cout<<" "<< "interest" << " : " << data._interest << endl;
        cout<<" "<< "movie" << " : " << data._movie << endl;
        cout<<" "<< "sport" << " : " << data._sport << endl;
        cout<<" "<< "music" << " : " << data._music << endl;
        cout<<" "<< "game" << " : " << data._game << endl;
        cout<<" "<< "comic" << " : " << data._comic << endl;
        cout<<" "<< "book" << " : " << data._book << endl;
        cout<<" "<< "society" << " : " << data._society << endl;
        cout<<" "<< "viewcount" << " : " << data._viewcount << endl;


        cout<<" "<< "Network"  << endl;
        for (size_t i = 0; i<data._networks.size(); ++i) {
            cout<<" network.id :"<< "\t" << data._networks[i].id << endl;
            cout<<" network.stage:"<< "\t" << data._networks[i].stage << endl;
            cout<<" network.name:"<< "\t" << data._networks[i].name << endl;
        }

        cout<<" "<< "Elementary School"  << endl;

        for (size_t i=0; i<data._elementarySchools.size(); ++i) {
            cout<<" "<< "\t" << data._elementarySchools[i].id << endl;
            cout<<" "<< "\t" << data._elementarySchools[i].year << endl;
            cout<<" "<< "\t" << data._elementarySchools[i].name << endl;
        }
     
        cout<<" "<< "Junior School"  << endl;
        for (size_t i = 0; i<data._juniorSchools.size(); ++i) {
            cout<<" "<< "\t" << data._juniorSchools[i].id << endl;
            cout<<" "<< "\t" << data._juniorSchools[i].year << endl;
            cout<<" "<< "\t" << data._juniorSchools[i].name << endl;
        }

        cout<<" "<<"High School"  << endl;
        
        for (size_t i = 0; i<data._highSchools.size(); ++i) {
            cout<<" "<< "\t" << data._highSchools[i].id << endl;
            cout<<" "<< "\t" << data._highSchools[i].year << endl;
            cout<<" "<< "\t" << data._highSchools[i].name << endl;
            cout<<" "<< "\t" << data._highSchools[i].class1 << endl;
            cout<<" "<< "\t" << data._highSchools[i].class2 << endl;
            cout<<" "<< "\t" << data._highSchools[i].class3 << endl;
        }


        cout<<" "<<"Colege"  << endl;
               for (size_t i = 0; i<data._colleges.size(); ++i) {
            cout<<" "<< "\t" << data._colleges[i].id << endl;
            cout<<" "<< "\t" << data._colleges[i].name << endl;
            cout<<" "<< "\t" << data._colleges[i].year << endl;
            cout<<" "<< "\t" << data._colleges[i].department << endl;
        }


        cout<<" "<<"University"  << endl;
              for (size_t i = 0; i<data._universities.size(); ++i) {
            cout<<" "<< "\t" << data._universities[i].id << endl;
            cout<<" "<< "\t" << data._universities[i].year << endl;
            cout<<" "<< "\t" << data._universities[i].name << endl;
            cout<<" "<< "\t" << data._universities[i].department << endl;
            cout<<" "<< "\t" << data._universities[i].dorm << endl;
        }

        cout<<" "<<"Workplace"  << endl;
             for (size_t i = 0; i<data._workplaces.size(); ++i) {
            cout<<" "<< "\t" << data._workplaces[i].id << endl;
            cout<<" "<< "\t" << data._workplaces[i].name << endl;
        }

        cout<<" "<<"Region"  << endl;
        for (size_t i = 0; i<data._regions.size(); ++i) {
            cout<<" "<< "\t" << data._regions[i].id << endl;
            cout<<" "<< "\t" << data._regions[i].city << endl;
            cout<<" "<< "\t" << data._regions[i].province << endl;
        }

    } catch (const Ice::Exception& e) {
        cout<<" "<<e  << endl;
    } catch (const char* msg) {
        cout<<" "<<msg  << endl;
    }

    return true;
}

bool ShowSearchCacheResult(const map<long, xce::searchcache::dboperation::SearchCacheMemDataPtr>& map)
{
  std::cout<<"in Func ShowSearchCacheResult, map.size() is "<<map.size()<<std::endl;
    for(std::map<long, xce::searchcache::dboperation::SearchCacheMemDataPtr>::const_iterator iter=map.begin(); iter!=map.end(); ++iter)
    {
        cout << SEARCH_CACHE_ID   << " : " << iter->first << endl;
        cout << SEARCH_CACHE_STATUS << " : " << (iter->second)->status() << endl;
        cout << SEARCH_CACHE_NAME << " : " << (iter->second)->name() << endl;
        cout << SEARCH_CACHE_HEADURL << " : " << (iter->second)->headurl() << endl;
        cout << SEARCH_CACHE_TINYURL << " : " << (iter->second)->tinyurl() << endl;
        cout << SEARCH_CACHE_STATUS_CONFIG << " : " << (iter->second)->statusconfig() << endl;
        cout << SEARCH_CACHE_BROWSE_CONFIG << " : " << (iter->second)->browseconfig() << endl;
        cout << SEARCH_CACHE_PROFILE_PRIVACY << " : " << (iter->second)->profileprivacy() << endl;
   
        cout << "Network" << endl;
        for (size_t i = 0; i<(iter->second)->networks_size(); ++i) {
            cout << "\t" << (iter->second)->networks(i).id()<< endl;
            cout << "\t" << (iter->second)->networks(i).name() << endl;
            cout << "\t" <<  (iter->second)->networks(i).stage()<< endl;
        }    

        cout << "Elementary School" << endl;
        for (size_t i = 0; i<(iter->second)->elementaryschools_size(); ++i) {
            cout << "\t" << (iter->second)->elementaryschools(i).id() << endl;
            cout << "\t" <<  (iter->second)->elementaryschools(i).year()<< endl;
            cout << "\t" << (iter->second)->elementaryschools(i).name() << endl;
        }    

        cout << "Junior School" << endl;
        for (size_t i = 0; i<(iter->second)->juniorschools_size(); ++i) {
            cout << "\t" << (iter->second)->juniorschools(i).id() << endl;
            cout << "\t" << (iter->second)->juniorschools(i).year() << endl;
            cout << "\t" << (iter->second)->juniorschools(i).name() << endl;
        }    

        cout << "High School" << endl;
        for (size_t i = 0; i<(iter->second)->highschools_size(); ++i) {
            cout << "\t" << (iter->second)->highschools(i).id() << endl;
            cout << "\t" << (iter->second)->highschools(i).name() << endl;
            cout << "\t" << (iter->second)->highschools(i).class1() << endl;
            cout << "\t" << (iter->second)->highschools(i).class2() << endl;
            cout << "\t" << (iter->second)->highschools(i).class3() << endl;
        }

        cout << "Colege" << endl;
        for (size_t i = 0; i<(iter->second)->collegeschools_size(); ++i) {
            cout << "\t" << (iter->second)->collegeschools(i).id() << endl;
            cout << "\t" << (iter->second)->collegeschools(i).name() << endl;
            cout << "\t" << (iter->second)->collegeschools(i).year() << endl;
            cout << "\t" << (iter->second)->collegeschools(i).department() << endl;
        }

        cout << "University" << endl;
        for (size_t i = 0; i<(iter->second)->universityschools_size(); ++i) {
            cout << "\t" << (iter->second)->universityschools(i).id() << endl;
            cout << "\t" << (iter->second)->universityschools(i).year() << endl;
            cout << "\t" << (iter->second)->universityschools(i).name() << endl;
            cout << "\t" << (iter->second)->universityschools(i).dorm() << endl;
        }

        cout << "Workplace" << endl;
        for (size_t i = 0; i<(iter->second)->workplaces_size(); ++i) {
            cout << "\t" << (iter->second)->workplaces(i).id() << endl;
            cout << "\t" << (iter->second)->workplaces(i).name() << endl;
        }

        cout << "Region" << endl;
        for (size_t i = 0; i<(iter->second)->regions_size(); ++i) {
            cout << "\t" << (iter->second)->regions(i).id() << endl;
            cout << "\t" << (iter->second)->regions(i).city() << endl;
            cout << "\t" <<  (iter->second)->regions(i).province()<< endl;
        }
    }

    return true;

}


bool ShowSearchCacheResult(const Int2ByteSeq& map)
{
  std::cout<<"in Func ShowSearchCacheResult, map.size() is "<<map.size()<<std::endl;
    for(Int2ByteSeq::const_iterator itS=map.begin(); itS!=map.end(); ++itS)
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
        cout << SEARCH_CACHE_AUTH << " : " << result.generels[SEARCH_CACHE_AUTH] << endl;
        cout << SEARCH_CACHE_PROFILE_PRIVACY << " : " << result.generels[SEARCH_CACHE_PROFILE_PRIVACY] << endl;
   


        cout << "Network" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_NETWORK].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_NETWORK][i][SEARCH_CACHE_NETWORK_STAGE] << endl;
        }    

        cout << "Elementary School" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL][i][SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR] << endl;
        }    

        cout << "Junior School" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_JUNIOR_SCHOOL][i][SEARCH_CACHE_JUNIOR_SCHOOL_YEAR] << endl;
        }    

        cout << "High School" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_HIGH_SCHOOL][i][SEARCH_CACHE_HIGH_SCHOOL_YEAR] << endl;
        }

        cout << "Colege" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_COLLEGE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_COLLEGE][i][SEARCH_CACHE_COLLEGE_DEPARTMENT] << endl;
        }

        cout << "University" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_UNIVERSITY].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_NAME] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_YEAR] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_UNIVERSITY][i][SEARCH_CACHE_UNIVERSITY_DEPARTMENT] << endl;
        }

        cout << "Workplace" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_WORKPLACE].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_WORKPLACE][i][SEARCH_CACHE_WORKPLACE_NAME] << endl;
        }

        cout << "Region" << endl;
        for (size_t i = 0; i<result.infos[SEARCH_CACHE_TYPE_REGION].size(); ++i) {
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_ID] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_PROVINCE] << endl;
            cout << "\t" << result.infos[SEARCH_CACHE_TYPE_REGION][i][SEARCH_CACHE_REGION_CITY] << endl;
        }
    }

    return true;

}
