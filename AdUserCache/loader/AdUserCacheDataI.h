#ifndef ADUSERCACHEDATAI_H_
#define ADUSERCACHEDATAI_H_

#include <QueryRunner.h>
#include "Date.h"
#include <boost/lexical_cast.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"

namespace xce {
namespace ad {

enum UserIdentity{
  Univ_Stu = 0, // 2
  Junior_Stu = 1,// 1
  HighSchool_Stu = 2, // 1
//  College_stu,
  Worker = 3, // 4
  NONE = 4,
};

class UserProfileWrapper : public Ice::Object {
public:
  UserProfileWrapper();
  void SetUserId(int userid){
    id_ = userid;
  }
  void SetUserBornProperties(const string& gender, int birthyear, int birthmonth, int birthday, const string& home_city){
    gender_ = gender;
    birthyear_ = birthyear;
    birthmonth_ = birthmonth;
    birthday_ = birthday;
    home_city_ = home_city;
  }
  void SetUserStageProperties(int stage){
    stage_ = stage;
  }
  void SetUniversityProperties(Ice::Long univ_id, const string& type_of_course, int enrollyear){
    univ_id_ = univ_id;
    type_of_course_ = type_of_course;
    univ_enrollyear_ = enrollyear;
  }
  void SetWorkPlaceProperties(const string& work_city_id){
    work_city_id_ = work_city_id;
  }
  void SetHighSchoolProperties(Ice::Long high_school_id, int high_school_enrollyear){
    high_school_id_ = high_school_id;
    high_school_enrollyear_ = high_school_enrollyear;
  }
  void SetJuniorProperties(Ice::Long junior_school_id, int junior_school_enrollyear){
    junior_school_id_ = junior_school_id;
    junior_school_enrollyear_ = junior_school_enrollyear;
  }
  UserProfile SetAllProperties(int id, int stage, int gender, int age, const string& current_area, const string& home_area, Ice::Long school, int grade);
  UserProfile GetUserProfile();
  void GenerateIdentity();
	int gender();
	int age();
	string current_area();
	string home_area();
	Ice::Long school();
private:
	UserProfile userprofile_;
	string work_city_id_;
	string gender_;
	int birthday_;
	int birthmonth_;
	int birthyear_;
	int stage_;
	string type_of_course_;
	int id_;
	string home_city_;
	UserIdentity identity_;
	Ice::Long univ_id_;
	Ice::Long junior_school_id_;
	Ice::Long high_school_id_;
	int junior_school_enrollyear_;
	int high_school_enrollyear_;
	int univ_enrollyear_;
	int grade_;
};

typedef ::IceUtil::Handle<UserProfileWrapper> UserProfileWrapperPtr;
typedef ::std::vector<UserProfileWrapperPtr> UserProfileWrapperSeq;
typedef ::std::map<int, UserProfileWrapperPtr> UserProfileWrapperMap;

}
}
#endif
