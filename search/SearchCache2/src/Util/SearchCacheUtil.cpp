
#include "SearchCacheUtil.h"
#include "SearchCacheData.h"
#include "UtilCxx/src/LogWrapper.h"

namespace xce {
  namespace searchcache{

    bool SearchCacheUtil::ShowSearchCacheData(SearchCacheDataPtr& obj) {
      try{
        MCE_INFO("[TRACE] " << "uid"<< " : " << obj->_id);
        MCE_INFO("[TRACE] "<< "status"<< " : " << obj->_status);
        MCE_INFO("[TRACE] "<< "name"<< " : " << obj->_name );
        MCE_INFO("[TRACE] "<< "signature"<< " : " << obj->_signature);
        MCE_INFO("[TRACE] "<< "headurl"<< " : " << obj->_headurl );
        MCE_INFO("[TRACE] "<< "tinyurl"<< " : " << obj->_tinyurl);
        MCE_INFO("[TRACE] "<< "star"<< " : " << obj->_star);
        MCE_INFO("[TRACE] "<< "home_city"<< " : " << obj->_home_city);
        MCE_INFO("[TRACE] "<< "home_province"<< " : " << obj->_home_province );
        MCE_INFO("[TRACE] "<< "status_config"<< " : " << obj->_status_config );
        MCE_INFO("[TRACE] "<< "browse_config"<< " : " << obj->_browse_config);
        MCE_INFO("[TRACE] "<< "birthday_year" << " : " << obj->_birthday_year);
        MCE_INFO("[TRACE] "<< "birthday_month" << " : " << obj->_birthday_month);
        MCE_INFO("[TRACE] "<< "birthday_day" << " : " << obj->_birthday_day);
        MCE_INFO("[TRACE] "<< "state" << " : " << obj->_state);
        MCE_INFO("[TRACE] "<< "stage" << " : " << obj->_stage);
        MCE_INFO("[TRACE] "<< "lastlogintime" << " : " << obj->_lastlogintime);
        MCE_INFO("[TRACE] "<< "astrology" << " : " << obj->_astrology);
        MCE_INFO("[TRACE] "<< "basic_config" << " : " << obj->_basic_config);
        MCE_INFO("[TRACE] "<< "gender" << " : " << obj->_gender);
        MCE_INFO("[TRACE] "<< "interest" << " : " << obj->_interest);
        MCE_INFO("[TRACE] "<< "movie" << " : " << obj->_movie);
        MCE_INFO("[TRACE] "<< "sport" << " : " << obj->_sport);
        MCE_INFO("[TRACE] "<< "music" << " : " << obj->_music);
        MCE_INFO("[TRACE] "<< "game" << " : " << obj->_game);
        MCE_INFO("[TRACE] "<< "comic" << " : " << obj->_comic);
        MCE_INFO("[TRACE] "<< "book" << " : " << obj->_book);
        MCE_INFO("[TRACE] "<< "society" << " : " << obj->_society);
        MCE_INFO("[TRACE] "<< "viewcount" << " : " << obj->_viewcount);
        
        MCE_INFO("[TRACE] "<< "profile_privacy" << " : " << obj->_profile_privacy);
        MCE_INFO("[TRACE] "<< "auth" << " : " << obj->_auth);



        MCE_INFO("[TRACE] "<< "Network" );
        for (size_t i = 0; i<obj->_networks.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_networks[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_networks[i].stage);
          MCE_INFO("[TRACE] "<< "\t" << obj->_networks[i].name);
        }

        MCE_INFO("[TRACE] "<< "Elementary School" );

        for (size_t i=0; i<obj->_elementarySchools.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_elementarySchools[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_elementarySchools[i].year);
          MCE_INFO("[TRACE] "<< "\t" << obj->_elementarySchools[i].name);
        }

        MCE_INFO("[TRACE] "<< "Junior School" );
        for (size_t i = 0; i<obj->_juniorSchools.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_juniorSchools[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_juniorSchools[i].year);
          MCE_INFO("[TRACE] "<< "\t" << obj->_juniorSchools[i].name);
        }

        MCE_INFO("[TRACE] "<<"High School" );

        for (size_t i = 0; i<obj->_highSchools.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_highSchools[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_highSchools[i].year);
          MCE_INFO("[TRACE] "<< "\t" << obj->_highSchools[i].name);
          MCE_INFO("[TRACE] "<< "\t" << obj->_highSchools[i].class1);
          MCE_INFO("[TRACE] "<< "\t" << obj->_highSchools[i].class2);
          MCE_INFO("[TRACE] "<< "\t" << obj->_highSchools[i].class3);
        }


        MCE_INFO("[TRACE] "<<"Colege" );
        for (size_t i = 0; i<obj->_colleges.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_colleges[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_colleges[i].year);
          MCE_INFO("[TRACE] "<< "\t" << obj->_colleges[i].department);
        }


        MCE_INFO("[TRACE] "<<"University" );
        for (size_t i = 0; i<obj->_universities.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_universities[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_universities[i].year);
          MCE_INFO("[TRACE] "<< "\t" << obj->_universities[i].name);
          MCE_INFO("[TRACE] "<< "\t" << obj->_universities[i].department);
          MCE_INFO("[TRACE] "<< "\t" << obj->_universities[i].dorm);
        }

        MCE_INFO("[TRACE] "<<"Workplace" );
        for (size_t i = 0; i<obj->_workplaces.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_workplaces[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_workplaces[i].name);
        }

        MCE_INFO("[TRACE] "<<"Region" );
        for (size_t i = 0; i<obj->_regions.size(); ++i) {
          MCE_INFO("[TRACE] "<< "\t" << obj->_regions[i].id);
          MCE_INFO("[TRACE] "<< "\t" << obj->_regions[i].city);
          MCE_INFO("[TRACE] "<< "\t" << obj->_regions[i].province);
        }

      } catch (const Ice::Exception& e) {
        MCE_INFO("[TRACE] "<<e );
        return false;
      } catch (const char* msg) {
        MCE_INFO("[TRACE] "<<msg );
        return false;
      }
      return true;


    }
  }
}

