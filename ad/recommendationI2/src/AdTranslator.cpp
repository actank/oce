#include "AdTranslator.h"

#include <bitset>
#include <boost/lexical_cast.hpp>

#include "AdStruct.h"

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace MyUtil;
using namespace boost;
using namespace com::xiaonei::xce;

UserBase UserBase::MakeUser(UserProfile & profile) {
  CookieEmptyCheck(profile);

  UserBase user;
  user.set_age(profile.age());
  if (profile.stage() == 2) { // 大学
    user.set_area(profile.current_area());
  } else {
    user.set_area(profile.ip_area());
  }
  user.set_gender(profile.gender());
  user.set_stage(profile.stage());
  user.set_grade(profile.grade());
  MCE_DEBUG("UserBase::MakeUser --> user age = " << user.age()
      << ", stage = " << user.stage() << ", grade = " << user.grade()
      << ", area = " << user.area() << ", gender = " << user.gender());
  return user;
}

void UserBase::CookieEmptyCheck(UserProfile& upf) {
  if (!upf.id()) {
    upf.set_id(888);
  }
  if(upf.age() > 100 || upf.age() < 15){
    upf.set_age(20 + Translator::instance().server_index() * 5);
  }
  if (upf.ip_area().empty() || "0000000000000000" == upf.ip_area()) {
    upf.set_ip_area("0086110000000000"); // 北京
  }
  if (!upf.stage()) {
    upf.set_stage(4);
  }
  if (!upf.ip()) {
    upf.set_ip(975044608); // 北京的一个随机IP
  }
  if (upf.current_area().empty()) {
    upf.set_current_area("0086110000000000"); // 北京
  }

  ///////////////////////////
//  upf.set_ip_area(upf.home_area()); // For test.
  ///////////////////////////
}

/***********************************************************************************************************************/
uint64_t Translator::GetIndex(const UserBase& obj) {
  uint64_t ans = 0;
  ans |= CommonUtil::instance().CodeGender(obj.gender());
  ans |= CommonUtil::instance().CodeStage(obj.stage());
  ans |= CommonUtil::instance().CodeGrade(obj.grade(), obj.stage());
  ans |= CommonUtil::instance().CodeAge(obj.age());
  ans |= AreaUtil::instance().CodeArea(obj.area());
  return ans;
}

uint64_t Translator::GetIndex(const BidUnit& obj) {
  uint64_t ans = 0;
  uint64_t tmp = 0;
  tmp = CommonUtil::instance().CodeGender(obj.gender());
  ans |= tmp;
  tmp = CommonUtil::instance().CodeStage(obj.stage());
  ans |= tmp;
  tmp = CommonUtil::instance().CodeGrade(obj.grade(), obj.stage());
  ans |= tmp;
  tmp = CommonUtil::instance().CodeAge(obj.age());
  ans |= tmp;
  tmp = AreaUtil::instance().CodeArea(obj.area());
  ans |= tmp;
  tmp = SchoolUtil::instance().CodeSchool(obj.school());
  ans |= tmp;
  return ans;
}
uint64_t Translator::GetIndex(const UserProfile& obj) {
  uint64_t ans = 0;
  ans |= CommonUtil::instance().CodeGender(obj.gender());
  ans |= CommonUtil::instance().CodeStage(obj.stage());
  ans |= CommonUtil::instance().CodeGrade(obj.grade(), obj.stage());
  ans |= CommonUtil::instance().CodeAge(obj.age());
  if (2 == obj.stage()) { // 大学
    ans |= AreaUtil::instance().CodeArea(obj.current_area());
  } else if (4 == obj.stage()) { //白领
    ans |= AreaUtil::instance().CodeArea(obj.ip_area());
  }
  return ans;
}
bool Translator::Match(const uint64_t& user_key, const uint64_t& bid_unit_key) {
  if ((user_key & bid_unit_key) == user_key) {
    return true;
  } else {
    return false;
  }
}

/***********************************************************************************************************************/
const int CommonUtil::GRADE_MAP_SIZE;
const int CommonUtil::GRADE_MAP[GRADE_MAP_SIZE] = { -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //25
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4,
    -1, -1, -1, -1, -1, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, 8, 8, 8, 8, 8, 8,
    8, 8, 8 };

vector<uint64_t> CommonUtil::Split(const string& str, boost::char_separator<char> separator) {
  vector < uint64_t > ans;
  Ice::Long integer = 0;
//  boost::char_separator<char> sep("-");
//  boost::char_separator<char> sep(separator);
  boost::tokenizer < boost::char_separator<char> > tok(str, separator);
  for (boost::tokenizer<boost::char_separator<char> >::iterator iter =
      tok.begin(); iter != tok.end(); ++iter) {
    try {
      integer = boost::lexical_cast<uint64_t>(*iter);
      ans.push_back(integer);
    } catch (...) {
      MCE_WARN("Translator::Split --> boost cast " << (*iter)
          << " to Ice::Long error");
    }
  }
  return ans;
}

uint64_t CommonUtil::CodeAge(int age) {
  if (age < 0) {
    age = 0;
  } else if (age > 99) {
    age = 99;
  }
  if (age % 10 != Translator::instance().server_index()) {
    return 0;
  }
  int pos = age / 10;
  uint64_t ans = 1;
  ans <<= pos;
  return ans << AGE_POS;
}
uint64_t CommonUtil::CodeAge(const string& age) {
  uint64_t ans = ONLY_AGE_SET;
  if (!age.empty()) {
    ans = 0;
    int begin = 0, end = 0, integer = 0;
    vector<int> ints;
    //    if (!age.empty()) {
    tokenizer<> tok(age);
    for (tokenizer<>::iterator iter = tok.begin(); iter != tok.end(); ++iter) {
      try {
        integer = boost::lexical_cast<int>(*iter);

        ints.push_back(integer);
      } catch (...) {
        MCE_WARN("Translator::CodeAge --> boost cast " << (*iter)
            << " to int error");
      }
    }
    //    }
    begin = ints.at(0);
    end = ints.at(1);
    for (int i = begin; i <= end; ++i) {
      ans |= CodeAge(i);
    }
  }
  return ans;
}
uint64_t CommonUtil::CodeGender(int gender) {
  uint64_t ans = 3;
  if (gender == 1) {
    ans = 2;
  } else if (gender == 0) {
    ans = 1;
  } else if (gender == 2) {
    ans = 3;
  } else {
    MCE_INFO("Translator::CodeGender --> error int_gender = " << gender);
  }
  return ans << GENDER_POS;
}

uint64_t CommonUtil::CodeStage(int stage) {
  uint64_t ans = 4;
  if (stage == 1 || stage == 2 || stage == 4 || stage == 6)
    ans = stage;
  else {
    MCE_INFO("Translator::CodeStage --> error int_stage = " << stage);
  }
  return ans << STAGE_POS;
}

uint64_t CommonUtil::CodeGrade(int grade, int stage) {
  uint64_t ans = 0;
  if (stage == 4 || stage == 6) {
    return ans;
  }
  if (grade >= 41 && grade < 70) {
    ans = 1;
    int offset = GRADE_MAP[grade];
    if (offset != -1) {
      ans <<= GRADE_MAP[grade];
    } else {
      MCE_INFO("Translator::CodeGrade --> error offset : grade =  " << grade);
      ans = 0;
    }
  } else {
    MCE_INFO("Translator::CodeGrade --> error int_grade = " << grade);
  }
  return ans << GRADE_POS;
}

uint64_t CommonUtil::CodeGrade(const string& grade, int stage) {
  uint64_t ans = ONLY_GRADE_SET;
  if (!grade.empty()) {
    int begin = 0, end = 0; //integer = 41,
    ans = 0;
    tokenizer<> tok(grade);
    for (tokenizer<>::iterator iter = tok.begin(); iter != tok.end();) {

      try {
        begin = lexical_cast<int> (*iter);
      } catch (...) {
        MCE_WARN("Translator::CodeGrade --> boost cast " << *iter
            << "to int error");
        return 0;
      }

      ++iter;
      /*if (begin == 61) { //对于博士，单独处理
       ans |= CodeGrade(61, stage);
       ++iter;
       continue;
       }*/

      try {
        end = lexical_cast<int> (*iter);
      } catch (...) {
        MCE_WARN("Translator::CodeGrade --> boost cast " << *iter
            << "to int error");
        return 0;
      }

      for (int i = begin; i <= end; ++i) {
        ans |= CodeGrade(i, stage);
      }
      ++iter;
    }
  }
  return ans;
}

/***********************************************************************************************************************/
const int AreaUtil::AREA_MAP_SIZE;
const int AreaUtil::AREA_MAP[AREA_MAP_SIZE] = { 0, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, 6, 7, 8, -1, -1, -1, -1, -1,
    -1, -1, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1, 16, 17, 18, 19, 20, 21, -1,
    -1, -1, 22, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1, 27, 28, 29, 30, 31, -1,
    -1, -1, -1, -1, 32, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, 34 };

vector<uint64_t> AreaUtil::ParseAreas(const string& area) {
  return SchoolUtil::instance().ParseSchools(area);
}

uint64_t AreaUtil::CodeArea(int area) {
  uint64_t ans = 1;
  if (0 == area) {
    return ONLY_AREA_SET;
  }
  if (area > 0 && area < 83) {
    int offset = AREA_MAP[area];
    if (offset != -1) {
      ans <<= offset;
    } else {
      MCE_INFO("Translator::CodeArea --> ERROR offset : area = " << area);
    }
  } else {
    MCE_INFO("Translator::CodeArea --> ERROR int_area = " << area);
  }
  return ans << AREA_POS;
}

uint64_t AreaUtil::CodeArea(const string& area) {
  uint64_t ans = 0;
  int integer = 0;
  string sub = "";
  boost::char_separator<char> sep(",");
  boost::tokenizer < boost::char_separator<char> > tok(area, sep);
  boost::tokenizer<boost::char_separator<char> >::iterator iter;
  for (iter = tok.begin(); iter != tok.end(); ++iter) {
    if ((*iter).length() >= 6 && "0086" == (*iter).substr(0, 4)) {
      try {
        sub = (*iter).substr(4, 2);
        integer = boost::lexical_cast<int>(sub);
      } catch (...) {
        MCE_WARN("Translator::CodeArea --> boost cast " << sub
            << "to int error");
        return 0;
      }
      MCE_DEBUG("Translator::CodeArea --> sub = " << sub << " integer = "
          << integer << " after code = " << CodeArea(integer));
      ans |= CodeArea(integer);
    }
  }
  return ans;
}

uint64_t AreaUtil::GetProvince(uint64_t area) {
  static uint64_t i = 10000000000;
  uint64_t province = area / i * i;
  if (province >= 86010000000000 && province <= 86990000000000) {
    return province;
  } else {
    return 0;
  }
}

uint64_t AreaUtil::GetCity(uint64_t area) {
  if (area == GetProvince(area)) { //city的位为0
    return 0;
  } else {
    return area;//保证city的位之后的所有位都为0
  }
}

/***********************************************************************************************************************/
vector<uint64_t> SchoolUtil::ParseSchools(const string& area) {
  vector<uint64_t> ans;
  uint64_t integer = 0;
  tokenizer<> tok(area);
  for (tokenizer<>::iterator iter = tok.begin(); iter != tok.end(); ++iter) {
    try {
      string sub = (*iter);
      if (sub.length() >= 4) {
        if ("area" == (*iter).substr(0, 4)) {
          sub = (*iter).substr(4);
        }
      }
      integer = boost::lexical_cast<uint64_t>(sub);
      ans.push_back(integer);
    } catch (...) {
      MCE_WARN("Translator::ParseSchools --> boost cast " << (*iter) << " to uint64_t error");
    }
  }
  return ans;
}

uint64_t SchoolUtil::CodeSchool(const string& school) {
  uint64_t ans = 0;
  if (school.empty()) {
    return ans;
  }
  vector < uint64_t > numbers = ParseSchools(school);
  if (numbers.empty()) {
    return ans;
  }
  for (vector<uint64_t>::iterator it = numbers.begin(); it != numbers.end(); ++it) {
    if (AreaUtil::instance().IsArea(*it)) {
      string str_a = "00" + boost::lexical_cast<string>(*it);
      ans |= AreaUtil::instance().CodeArea(str_a);
    } else {
      uint64_t area = SchoolPool::instance().GetSchoolAreaCode(*it);
      if (!area) {
        continue;
      }
      string str_a = "00" + boost::lexical_cast<string>(area);
      ans |= AreaUtil::instance().CodeArea(str_a);
    }
  }
  return ans;
}

