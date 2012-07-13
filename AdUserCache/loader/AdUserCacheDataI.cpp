#include "AdUserCacheDataI.h"
#include "AdUserCacheLoaderI.h"

using namespace ::mysqlpp;
using namespace ::xce::ad;
using namespace MyUtil;
using namespace ::com::xiaonei::xce;
using namespace xce::feed;
/*id = 0;
stage = -1;//
gender = 2;//默认无性别
age = 25;//默认
area = "";
school = 0;//默认0
grade = 0;//默认0
*/

void UserProfileWrapper::GenerateIdentity(){
  MCE_DEBUG("UserProfileWrapper::GenerIdentity : ");
  int cur_year = MyUtil::Date(time(NULL)).year();
  if(30 == stage_){
    identity_ = Worker;
    stage_ = 4;
    MCE_DEBUG("stage == 30   Worker  UserProfileWrapper::GenerIdentity : work_city_id_ << " << work_city_id_ << " stage_ = " << stage_);
    return;
  }
  if(work_city_id_.length() && "0" != work_city_id_){
    identity_ = Worker;
    stage_ = 4;
    MCE_DEBUG("Worker  UserProfileWrapper::GenerIdentity : work_city_id_ << " << work_city_id_ << " stage_ = " << stage_);
    return;
  }
  MCE_DEBUG("univ_id_ == " << univ_id_  << " junior_school_id_ = " << junior_school_id_ << "  high_school_id_ == " << high_school_id_);
  if(-1 != univ_id_ && univ_id_){
    MCE_DEBUG("Univ univ_enrollyear_ = " << univ_enrollyear_);
    int grade = cur_year - univ_enrollyear_;
    bool grade_ok = false;
    //如果毕业了，默认是白领
    if("大学生" == type_of_course_ || "本科生" == type_of_course_){
      if(grade >= 1 && grade <= 5){
        grade += 40;
        grade_ok = true;
      }
      else{
        grade = 0;
      }
    }
    else if ("博士" == type_of_course_){
      if(grade >= 1 && grade <= 4){
        grade += 60;
        grade_ok = true;
      }
      else{
        grade = 0;
      }
    }
    else if("硕士" == type_of_course_){
      if(grade >= 1 && grade <= 4){
        grade += 50;
        grade_ok = true;
      }
      else{
        grade = 0;
      }
    }
    if(grade_ok){
      identity_ = Univ_Stu;
      stage_ = 2;
      grade_ = grade;
      MCE_DEBUG("Univ_stu  UserProfileWrapper::GenerIdentity : grade = " << grade_ << " stage_ = " << stage_);
      return;
    }
  }
  if(-1 != high_school_id_ && high_school_id_){
    int grade = cur_year - high_school_enrollyear_;
    MCE_DEBUG("HighSchool  high_school_enrollyear_ = " << high_school_enrollyear_ << " grade = " << grade << " cur_year = " << cur_year);
    if(grade >= 1 && grade <= 4){
      identity_ = HighSchool_Stu;
      stage_ = 1;
      grade_ = grade + 30;
      MCE_DEBUG("HighSchool_stu  UserProfileWrapper::GenerIdentity : grade = " << grade_ << " stage_ = " << stage_);
      return;
    }
  }
  if(-1 != junior_school_id_ && junior_school_id_){
    int grade = cur_year - junior_school_enrollyear_;
    MCE_DEBUG("Junior junior_school_enrollyear_ = " << junior_school_enrollyear_ << " grade = " << grade<< " cur_year = " << cur_year);
    if(grade >= 1 && grade <= 4){
      identity_ = Junior_Stu;
      stage_ = 1;
      grade_ = grade + 20;
      MCE_DEBUG("Junior_stu  UserProfileWrapper::GenerIdentity : grade = " << grade_ << " stage_ = " << stage_);
      return;
    }

  }

  identity_ = NONE;
  stage_ = 4;
  grade_ = 0;
  MCE_DEBUG("NONE__Worker  UserProfileWrapper::GenerIdentity : grade = " << grade_ << " stage_ = " << stage_);
  return;
}

UserProfileWrapper::UserProfileWrapper() {
  work_city_id_ = "";
  id_ = 0;
  univ_id_ = -1;
  junior_school_id_ = -1;
  high_school_id_ = -1;
  grade_ = 0;
  identity_ = NONE;

  gender_ = "";
  birthday_ = 0;
  birthmonth_ = 0;
  birthyear_ = 0;
  stage_ = 4;

  type_of_course_ = "";
  univ_enrollyear_ = 0;
  id_ = 0;
  home_city_ = "";
  junior_school_enrollyear_ = 0;
  high_school_enrollyear_ = 0;
  grade_ = 0;
}

int UserProfileWrapper::gender(){
	int gender = rand() % 2;//默认随机性别
	if("男生" == gender_){
	  gender = 1;
	}
	else if("女生" == gender_){
	  gender = 0;
	}
	MCE_DEBUG("UserProfileWrapper::GetAdCodeGender --> sex = " << gender_ << "  gender = " << gender );
	return gender;
}

int UserProfileWrapper::age(){
  int age = 25;//默认
  int cur_year = MyUtil::Date(time(NULL)).year();
  age = cur_year - birthyear_ + 1;
  if(age > 100 || age < 15){
    age = 20 + (rand() % 10);
  }
  MCE_DEBUG("birthyear = " << birthyear_ << " cur_year = " << cur_year << " age = " << age);
  return age;
}

Ice::Long UserProfileWrapper::school(){
  MCE_DEBUG("UserProfileWrapper::GetAdCodeSchool : ");
  string school_id_str = "0000000000";
  string head = "00";
  Ice::Long school_id_long = 0;

  if(Worker == identity_ || NONE == identity_){
    return school_id_long;
  }
  else if(Univ_Stu == identity_){
    school_id_str = boost::lexical_cast<string>(univ_id_);
    head = "40";
  }
  else if(Junior_Stu == identity_){
    school_id_str = boost::lexical_cast<string>(junior_school_id_);
    head = "20";
  }
  else if(HighSchool_Stu == identity_){
    school_id_str = boost::lexical_cast<string>(high_school_id_);
    head = "30";
  }
  MCE_DEBUG("  school_id_str = " << school_id_str);
	int length = 10 - school_id_str.length();
	if(length >= 0){
	  school_id_str = head + string(length, '0') + school_id_str;
	  try{
	    school_id_long = boost::lexical_cast<Ice::Long>(school_id_str);
	  }
	  catch (exception& e){
	    MCE_WARN("boost::lexical_cast exception  school_id_str = " << school_id_str);
	  }
	}
	return school_id_long;
}
string UserProfileWrapper::home_area(){
  MCE_DEBUG("UserProfileWrapper::home_area : ");
  string area = "0086";
  if("" != home_city_){
    MCE_DEBUG("GetAreacodeFromUserCityCache(" << home_city_ << ")");
    area += AdUserCacheLoaderI::instance().GetAreacodeFromUserCityCache(home_city_);
  }
  if("0086" == area){
   area = "0086000000000000";
  }
  return area;
}
string UserProfileWrapper::current_area(){
  MCE_DEBUG("UserProfileWrapper::GetAdCodeArea : ");
  string area = "0086";
  if(Worker == identity_){
    MCE_DEBUG("Get Area BY work_place_info's  city_id = " << work_city_id_);
    int length = 12 - work_city_id_.length();
    area +=  work_city_id_ + string(length, '0');
  }
  else if(Univ_Stu == identity_&& univ_id_ && univ_id_ != -1){
      MCE_DEBUG("GetAreacodeFromUserAreaCache(" << univ_id_ << ")");
      area += AdUserCacheLoaderI::instance().GetAreacodeFromUserAreaCache(univ_id_);
  }
  else if(Junior_Stu == identity_ && junior_school_id_ && junior_school_id_ != -1){
    MCE_DEBUG("GetAreacodeFromUserJuniorCache(" << junior_school_id_ << ")");
    area += AdUserCacheLoaderI::instance().GetAreacodeFromUserJuniorCache(junior_school_id_);
  }
  else if(HighSchool_Stu == identity_ && high_school_id_ && high_school_id_){
    MCE_DEBUG("GetAreacodeFromUserHighSchoolCache(" << high_school_id_ << ")");
    area += AdUserCacheLoaderI::instance().GetAreacodeFromUserHighSchoolCache(high_school_id_);
  }
  else if(NONE == identity_ && "" != home_city_){
    MCE_DEBUG("GetAreacodeFromUserCityCache(" << home_city_ << ")");
    area += AdUserCacheLoaderI::instance().GetAreacodeFromUserCityCache(home_city_);
  }
  if("0086" == area){
    area = "0086000000000000";
  }
  return area;
}

UserProfile UserProfileWrapper::SetAllProperties(int id, int stage, int gender, int age, const string& current_area, const string& home_area, Ice::Long school, int grade){
  userprofile_.set_current_area(current_area);
  userprofile_.set_home_area(home_area);
  userprofile_.set_id(id);
  userprofile_.set_stage(stage);
  userprofile_.set_gender(gender);
  userprofile_.set_age(age);
  userprofile_.set_school(school);
  userprofile_.set_grade(grade);
  MCE_DEBUG("UserProfileWrapper::SetAllProperties --> id = " << id << " stage = " << stage << " gender = " << gender << " age = " << age << " current_area = " << current_area << " home_area = " << home_area << " school = " << school << " grade = " << grade);
  return userprofile_;
}

UserProfile UserProfileWrapper::GetUserProfile(){
  MCE_DEBUG("UserProfileWrapper::GetUserProfile : ");
  GenerateIdentity();
  MCE_INFO("UserProfileWrapper::GetUserProfile --> identity = " << identity_);
  userprofile_.set_grade(grade_);
  userprofile_.set_id(id_);
  userprofile_.set_stage(stage_);
  userprofile_.set_gender(gender());
  userprofile_.set_age(age());
  userprofile_.set_school(school());
  userprofile_.set_home_area(home_area());
  userprofile_.set_current_area(current_area());
  return userprofile_;
}


