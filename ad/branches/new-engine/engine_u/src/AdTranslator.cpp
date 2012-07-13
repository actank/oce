#include "AdTranslator.h"

#include <bitset>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "AdStruct.h"

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace MyUtil;
using namespace boost;
using namespace com::xiaonei::xce;

BidUnit::BidUnit(int gender, int stage, string& grade, string& age, string& school, string& area) {
  gender_ = gender;
  stage_ = stage;
  vector<short> short_keys;
  vector<uint64_t> uint_keys;
  Translator::ProcessAge(age, age_);
  if(stage_ == 2) {
    Translator::ProcessSchool(school, school_area_, school_);
    Translator::ProcessGrade(grade, grade_);
  } else if(stage == 4) {
    Translator::ProcessArea(area, area_);
  } else if(stage == 6) {
    Translator::ProcessArea(area, area_);
    grade_.push_back(-1);
  }
}

bool BidUnit::Match(const UserProfile& userProfile) {
  //大客户专用定向算法，用于匹配轮播列表中的某一个广告
  //MCE_DEBUG("BidUnit::Match() --> userProfile.gender() = " << userProfile.gender() << ", gender_ = " << gender_);
  try {
    if(gender_ != 2 && userProfile.gender() != gender_) {
      return false;
    } 
    int user_stage = userProfile.stage();
    //MCE_DEBUG("BidUnit::Match() --> user_stage = " << user_stage << ", stage_ = " << stage_);
    if(!(user_stage & stage_)) {
      return false;
    }
    /*ostringstream oss;
      oss << "BidUnit::Match() --> userProfile.age() = " << userProfile.age();
      for(int i = 0; i < age_.size(); ++i) {
      oss << ", " << age_[i];
      }
      MCE_DEBUG(oss.str());*/
    //MCE_DEBUG("BidUnit::Match() --> " << (age_.front() != -1));
    //MCE_DEBUG("BidUnit::Match() --> " << (find(age_.begin(), age_.end(), (short)userProfile.age()) == age_.end()));
    if( (!age_.empty()) && age_.front() != -1 && find(age_.begin(), age_.end(), (short)userProfile.age()) == age_.end()) {
      //MCE_DEBUG("BidUnit::Match() --> age not match");
      return false;
    }
    //MCE_DEBUG("BidUnit::Match() --> age matched");
    if(user_stage == 2) {
      //大学用户，用学校或者地域做匹配
      //MCE_DEBUG("BidUnit::Match() --> user_stage == 2");
      //MCE_DEBUG("BidUnit::Match() --> school_.empty()" << school_.empty());
      //MCE_DEBUG("BidUnit::Match() --> (find(school_.begin(), school_.end(), (uint64_t)userProfile.school()) != school_.end())" << (find(school_.begin(), school_.end(), (uint64_t)userProfile.school()) != school_.end()));
      //MCE_DEBUG("BidUnit::Match() --> (AreaMatch(boost::lexical_cast<uint64_t>(userProfile.current_area()), school_area_))" << (AreaMatch(boost::lexical_cast<uint64_t>(userProfile.current_area()), school_area_)));
      if(school_.empty() || school_.front() == 0 ||
          (find(school_.begin(), school_.end(), (uint64_t)userProfile.school()) != school_.end()) ||
          (AreaMatch(boost::lexical_cast<uint64_t>(userProfile.current_area()), school_area_))) {
        //MCE_DEBUG("BidUnit::Match() --> school matched");
        return grade_.empty() ||  grade_.front() == -1 || find(grade_.begin(), grade_.end(), userProfile.grade()) != grade_.end();
      }
    } else if(user_stage == 4) {
      //白领用户，仅用地域做匹配
      //MCE_DEBUG("BidUnit::Match() --> user_stage == 4");
      //MCE_DEBUG(AreaMatch(boost::lexical_cast<uint64_t>(userProfile.ip_area()), area_));
      return AreaMatch(boost::lexical_cast<uint64_t>(userProfile.ip_area()), area_);
    }
    //MCE_DEBUG("BidUnit::Match() --> run to end");
    return false; 
  } catch (std::exception e) {
    MCE_WARN("BidUnit::Match() --> " << e.what());
  } catch (...) {
    MCE_WARN("BidUnit::Match() --> unkown exception");
  }
  return false; 
}

bool BidUnit::AreaMatch(uint64_t key, const vector<uint64_t>& area) {
  if(area.empty() || area.front() == 0) {
    return true;
  }
  /* ostringstream oss;
  oss << "key = " << key;
  for(int i = 0; i<area.size(); ++i) {
    oss << ", " << area[i];
  }
  MCE_DEBUG(oss.str());*/
 uint64_t province = (key/10000000000)*10000000000;
  uint64_t COUNTRY = 86000000000000; 
  //MCE_DEBUG("key = " << key);
  if(find(area.begin(), area.end(), key) != area.end()) {
    return true;
  }
  if(province != key) {
    //MCE_DEBUG("province = " << province);
    if(find(area.begin(), area.end(), province) != area.end()) {
      return true;
    }
  }
  if(province != COUNTRY) {
    //MCE_DEBUG("COUNTRY = " << COUNTRY);
    if(find(area.begin(), area.end(), COUNTRY) != area.end()) {
      return true;
    }
  }
  return false;
}

bool Translator::ProcessGender(const int gender_code, vector<short>& genders) {
  try {
    if(gender_code == 0 || gender_code == 1) {
      genders.push_back((short)gender_code);
    } else if(gender_code == 2) {
      genders.push_back(0);
      genders.push_back(1);
    } 
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process gender: " << e.what() << ", gender_code = " << gender_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process gender, gender_code = " << gender_code);
  }
  return false;
}

bool Translator::ProcessStage(const int stage_code, vector<short>& stages) {
  try {
    if(stage_code & 1) {
      stages.push_back(1);
    }
    if(stage_code & 2) {
      stages.push_back(2);
    }
    if(stage_code & 4) {
      stages.push_back(4);
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process stage: " << e.what() << ", stage_code = " << stage_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process stage, stage_code = " << stage_code);
  }
  return false;
}


bool Translator::ProcessGrade(const string& grade_code, vector<short>& grades) {
  //41,41;43,45;51,51;53,53;61,68
  try {
    if(grade_code.empty()) {
      //-1表示所有
      grades.push_back(-1);
    } else {
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(";");
      tokenizer tokens(grade_code, sep);
      for(tokenizer::iterator tok_iter = tokens.begin();
          tok_iter != tokens.end(); ++tok_iter) {
        boost::char_separator<char> sep2(",");
        tokenizer tokens2(*tok_iter, sep2);
        int begin = 0;
        int end = 0;
        int num = 1;
        for (tokenizer::iterator tok_iter2 = tokens2.begin();
            tok_iter2 != tokens2.end(); ++tok_iter2, ++num) {
          if(num == 1) {
            begin = boost::lexical_cast<short>(*tok_iter2);
          } else if(num == 2) {
            end = boost::lexical_cast<short>(*tok_iter2);
          }
        }
        if(!end && begin > 0) {
          end = begin;
        }
        for(short i = begin; i <= end; ++i) {
          grades.push_back(i);
        }
      }
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process grade: " << e.what() << ", grade_code = " << grade_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process grade, grade_code = " << grade_code);
  }
  return false;
}
bool Translator::ProcessAge(const string& age_code, vector<short>& ages) {
  try {
    if(age_code.empty()) {
      //-1表示所有
      ages.push_back(-1);
    } else {
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(";");
      tokenizer tokens(age_code, sep);
      for(tokenizer::iterator tok_iter = tokens.begin();
          tok_iter != tokens.end(); ++tok_iter) {
        boost::char_separator<char> sep2(",");
        tokenizer tokens2(*tok_iter, sep2);
        int begin = 0;
        int end = 0;
        int num = 1;
        for (tokenizer::iterator tok_iter2 = tokens2.begin();
            tok_iter2 != tokens2.end(); ++tok_iter2, ++num) {
          if(num == 1) {
            begin = boost::lexical_cast<short>(*tok_iter2);
          } else if(num == 2) {
            end = boost::lexical_cast<short>(*tok_iter2);
          }
        }
        if(!end && begin > 0) {
          end = begin;
        }
        for(short i = begin; i <= end; ++i) {
          ages.push_back(i);
        }
      }
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process age: " << e.what() << ", age_code = " << age_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process age, age_code = " << age_code);
  }
  return false;
}

bool Translator::ProcessSchool(const string& school_code, vector<uint64_t>& areas, vector<uint64_t>& schools) {
  try {
    if(school_code.empty()) {
      //0表示所有
      schools.push_back(0);
    } else {
      vector<string> result;
      boost::algorithm::split(result, school_code, is_any_of(";"));
      int num = 1;
      vector<uint64_t> temp;
      for(vector<string>::iterator it = result.begin(); it != result.end(); ++it, ++num) {
        if(num == 1 && !(*it).empty()) {
          if(ProcessLeadingArea(*it, areas)) {
          }
        } else if(num == 2 && !(*it).empty()) {
          if(ProcessRealSchool(*it, schools)) {
          }
        }
      }
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process school: " << e.what() << ", school_code = " << school_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process school, school_code = " << school_code);
  }
  return false;
}

bool Translator::ProcessLeadingArea(const string& area_code, vector<uint64_t>& areas) {
  //area0086110000000000,area0086210000000000,area0086310000000000,area0086440000000000,area0086510000000000
  try {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",");
    tokenizer tokens(area_code, sep);
    for(tokenizer::iterator tok_iter = tokens.begin();
        tok_iter != tokens.end(); ++tok_iter) {
      const string& str = *tok_iter;
      size_t pos = str.find("0086");
      if(pos != string::npos) {
        uint64_t area = boost::lexical_cast<uint64_t>(str.substr(pos));
        areas.push_back(area);
      }
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process LeadingArea: " << e.what() << ", LeadingArea_code = " << area_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process LeadingArea, LeadingArea_code = " << area_code);
  }
  return false;
}

bool Translator::ProcessRealSchool(const string& school_code, vector<uint64_t>& schools) {
  //400000001011,400000001003,400000001056,400000001020,400000001032,400000001036
  try {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",");
    tokenizer tokens(school_code, sep);
    for(tokenizer::iterator tok_iter = tokens.begin();
        tok_iter != tokens.end(); ++tok_iter) {
      uint64_t school = boost::lexical_cast<uint64_t>(*tok_iter);
      schools.push_back(school);
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process RealSchool: " << e.what() << ", RealSchool_code = " << school_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process RealSchool, RealSchool_code = " << school_code);
  }
  return false;
}

bool Translator::ProcessArea(const string& area_code, vector<uint64_t>& areas) {
  //0086110000000000,0086310000000000,0086440100000000,0086440300000000
  try {
    if(area_code.empty()) {
      //0表示所有
      areas.push_back(0);
    } else {
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(",");
      tokenizer tokens(area_code, sep);
      for(tokenizer::iterator tok_iter = tokens.begin();
          tok_iter != tokens.end(); ++tok_iter) {
        uint64_t area = boost::lexical_cast<uint64_t>(*tok_iter);
        areas.push_back(area);
      }
    }
    return true;
  } catch (std::exception e) {
    MCE_WARN("[ProcessBid] exception when process area: " << e.what() << ", area_code = " << area_code);
  } catch (...) {
    MCE_WARN("[ProcessBid] unknown exception when process area, area_code = " << area_code);
  }
  return false;
}

/***********************************************************************************************************************/
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


