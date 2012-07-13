#include "AdTranslator.h"
#include "ServiceI.h"
#include "AdStruct.h"
#include <bitset>
#include  <boost/lexical_cast.hpp>

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace MyUtil;
using namespace boost;
using namespace com::xiaonei::xce;
vector<uint64_t> Translator::ParseAreas(const string& area){
  return ParseSchools(area);
}
vector<uint64_t> Translator::ParseSchools(const string& area){
  vector<uint64_t> ans;
  uint64_t integer = 0;
  tokenizer<> tok(area);
  for(tokenizer<>::iterator iter=tok.begin(); iter!=tok.end(); ++iter){
    try{
      string sub = (*iter);
      if(sub.length() >= 4){
        if("area" == (*iter).substr(0, 4)){
          sub = (*iter).substr(4);
        }
      }
      //MCE_DEBUG("Translator::ParseSchools --> area = " << area << "  *iter = " << *iter <<"    sub = " << sub);
      integer = boost::lexical_cast<uint64_t>(sub);
      ans.push_back(integer);
    }
    catch(...){
      MCE_WARN("Translator::ParseSchools --> boost cast " << (*iter) << " to uint64_t error");
    }
  }
  return ans;
}

vector<uint64_t> Translator::Split(const string& area){
  vector<uint64_t> ans;
  Ice::Long integer = 0;
  boost::char_separator<char> sep("-");
  boost::tokenizer<boost::char_separator<char> > tok(area, sep);
  for(boost::tokenizer<boost::char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); ++iter) {
    try{
      integer = boost::lexical_cast<uint64_t>(*iter);
      ans.push_back(integer);
    }
    catch(...){
      MCE_WARN("Translator::Split --> boost cast " << (*iter) << " to Ice::Long error");
    }
  }
  return ans;
}


uint64_t Translator::GetIndex(const User& obj){
	uint64_t ans = 0;
	ans |= CodeGender(obj.gender);
	ans |= CodeStage(obj.stage);
	ans |= CodeGrade(obj.grade, obj.stage);
	ans |= CodeAge(obj.age);
	ans |= CodeArea(obj.area);
	return ans;
}

uint64_t Translator::GetIndex(const BidUnit& obj){
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << " see after --- ");
	uint64_t ans = 0;
	uint64_t tmp = 0;
	tmp= CodeGender(obj.gender);ans |= tmp;
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << ",tmp" << bitset<64>(tmp) << ",ans" << bitset<64>(ans) );
  tmp= CodeStage(obj.stage);ans |= tmp;
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << ",tmp" << bitset<64>(tmp) << ",ans" << bitset<64>(ans) );
  tmp= CodeGrade(obj.grade, obj.stage);ans |= tmp;
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << ",tmp" << bitset<64>(tmp) << ",ans" << bitset<64>(ans) );
  tmp= CodeAge(obj.age);ans |= tmp;
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << ",tmp" << bitset<64>(tmp) << ",ans" << bitset<64>(ans) );
  tmp= CodeArea(obj.area);ans |= tmp;
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << ",tmp" << bitset<64>(tmp) << ",ans" << bitset<64>(ans) );
  tmp= CodeSchool(obj.school);ans |= tmp;
  //MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << ",tmp" << bitset<64>(tmp) << ",ans" << bitset<64>(ans) );
	//MCE_DEBUG("Translator::GetIndex --> bid_id = " << obj.bid_unit_id << " ans = " << ans);
	return ans;
}
uint64_t Translator::GetIndex(const UserProfile& obj){
  uint64_t ans = 0;
  //MCE_DEBUG("Translator::GetIndex--->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> obj.current_area = " << obj.current_area());
  ans |= CodeGender(obj.gender());
  ans |= CodeStage(obj.stage());
  ans |= CodeGrade(obj.grade(), obj.stage());
  ans |= CodeAge(obj.age());
  if(2 == obj.stage()){
    ans |= CodeArea(obj.current_area());
  }
  else if(4 == obj.stage()){
    ans |= CodeArea(obj.ip_area());
  }
  return ans;
}

uint64_t Translator::CodeAge(int age){
	if(age < 0){
		age = 0;
	}
	else if(age > 99){
		age = 99;
	}
	if(age % 10 != server_index){
	  return 0;
	}
  int pos = age / 10;
  uint64_t ans = 1;
  ans <<= pos;
  return ans << AGE_POS;
}

uint64_t Translator::CodeSchool(const string& school){
  //MCE_DEBUG("Translator::CodeSchool --> school = " << school);
  uint64_t ans = 0;
  if(school.empty()){
    return ans;
  }
  vector<uint64_t> numbers = ParseSchools(school);
  if(numbers.empty()){
    return ans;
  }
  for(vector<uint64_t>::iterator it = numbers.begin(); it != numbers.end(); ++it){
    if(IsArea(*it)){
      string str_a = "00" + boost::lexical_cast<string>(*it);
      ans |= CodeArea(str_a);
    }
    else{
      uint64_t area = SchoolPool::instance().GetSchoolAreaCode(*it);
      if(!area){
        continue;
      }
      //MCE_DEBUG("Translator::CodeSchool --> CONVERT school to area = " << area );
      string str_a = "00" + boost::lexical_cast<string>(area);
      ans |= CodeArea(str_a);
    }
  }
  return ans;
}

uint64_t Translator::CodeAge(const string& age){
  //MCE_DEBUG("Translator::CodeAge --> age = " << age);
	uint64_t ans = ONLY_AGE_SET;
	if(!age.empty()){
	  ans = 0;
    int begin = 0, end = 0, integer = 0;
    vector<int> ints;
    if(!age.empty()){
      tokenizer<> tok(age);
      for(tokenizer<>::iterator iter=tok.begin(); iter!=tok.end(); ++iter){
        try{
          integer = boost::lexical_cast<int>(*iter);

          ints.push_back(integer);
        }
        catch(...){
          MCE_WARN("Translator::CodeAge --> boost cast " << (*iter) << " to int error");
        }
      }
    }
    begin = ints.at(0);
    end = ints.at(1);
    //MCE_INFO("Translator::CodeAge --> age = " << age << " begin = " << begin << " end = " << end);
    for(int i = begin; i <= end; ++i){
      ans |= CodeAge(i);
      //MCE_INFO("Translator::CodeAge --> i = " << i << " ans = " << ans << " server_index = " << server_index);
    }
	}
	return ans;
}

uint64_t Translator::CodeArea(int area){
	uint64_t ans = 1;
	if(0 == area){
	  return ONLY_AREA_SET;
	}
	if(area > 0 && area < 83){
		int offset = AREA_MAP[area];
		if(offset != -1){
			ans <<= offset;
		}
		else{
			MCE_WARN("Translator::CodeArea --> ERROR offset : area = " << area);
		}
	}
	else{
		MCE_WARN("Translator::CodeArea --> ERROR int_area = " << area);
	}
	return ans << AREA_POS;
}

uint64_t Translator::CodeArea(const string& area){
  uint64_t ans = 0;
  int integer = 0;
  string sub = "";
  boost::char_separator<char> sep(",");
  boost::tokenizer<boost::char_separator<char> > tok(area, sep);
  boost::tokenizer<boost::char_separator<char> >::iterator iter;
  for(iter = tok.begin(); iter != tok.end(); ++iter) {
    if((*iter).length() >= 6 && "0086" == (*iter).substr(0, 4)){
      try{
        sub = (*iter).substr(4, 2);
        integer = boost::lexical_cast<int>(sub);
      }
      catch(...){
        MCE_WARN("Translator::CodeArea --> boost cast " << sub << "to int error");
        return 0;
      }
      //MCE_DEBUG("Translator::CodeArea --> sub = " << sub << " integer = " << integer << " after code = " << CodeArea(integer));
      ans |= CodeArea(integer);
    } else if(("0001" == (*iter).substr(0, 4)) || ("0002" == (*iter).substr(0, 4))) { //临时代码，如果是美国或者加拿大，则特殊处理
      //左移35位，因为第35位目前暂时没有用到
      MCE_DEBUG("[CodeArea] enter if");
      uint64_t tempans = 1;
      ans |= (tempans << 35 << AREA_POS);
    } else {//其他国家，看北京地区的广告
      ans |= CodeArea(11);
    }
  }
  MCE_DEBUG("[CodeArea] area = " << area << ", ans = " << ans);
  return ans;
}

uint64_t Translator::CodeGender(int gender){
	uint64_t ans = 3;
	if(gender == 1){
		ans = 2;
	}
	else if(gender == 0){
		ans = 1;
	}
	else if(gender == 2){
		ans = 3;
	}
	else{
		MCE_WARN("Translator::CodeGender --> error int_gender = " << gender);
	}
  //MCE_INFO("Translator::CodeGender --> gender = " << gender << " ans = " << ans);
	return ans << GENDER_POS;
}

uint64_t Translator::CodeStage(int stage){
	uint64_t ans = 4;
	if(stage == 1 || stage == 2 || stage == 4 || stage == 6)
		ans = stage;
	else{
		MCE_WARN("Translator::CodeStage --> error int_stage = " << stage);
	}
	return ans << STAGE_POS;
}

uint64_t Translator::CodeGrade(int grade, int stage){
  //MCE_INFO("Translator::CodeGrade INT --> grade = " << grade);
	uint64_t ans = 0;
	if(stage == 4 || stage == 6){
	  return ans;
	}
	if(grade >= 41 && grade < 70){
	  ans = 1;
		int offset = GREADE_MAP[grade];
		if(offset != -1){
			ans <<= GREADE_MAP[grade];
		}
		else{
			MCE_WARN("Translator::CodeGrade --> error offset : grade =  " << grade);
			ans = 0;
		}
	}
	else{
		//MCE_WARN("Translator::CodeGrade --> error int_grade = " << grade);
	}
	return ans << GRADE_POS;
}

uint64_t Translator::CodeGrade(const string& grade, int stage){
  //MCE_INFO("Translator::CodeGrade --> grade = " << grade );
	uint64_t ans = ONLY_GRADE_SET;
	if(!grade.empty()){
    int integer = 41, begin = 0, end = 0;
    ans = 0;
    tokenizer<> tok(grade);
    for (tokenizer<>::iterator iter = tok.begin(); iter != tok.end();) {


      try{
        begin = lexical_cast<int> (*iter);
      }
      catch(...){
        MCE_WARN("Translator::CodeGrade --> boost cast " << *iter << "to int error");
        return 0;
      }



      ++iter;
      /*if (begin == 61) { //对于博士，单独处理
        ans |= CodeGrade(61, stage);
        ++iter;
        continue;
      }*/


      try{
        end = lexical_cast<int> (*iter);
      }
      catch(...){
        MCE_WARN("Translator::CodeGrade --> boost cast " << *iter << "to int error");
        return 0;
      }


      for(int i = begin; i <= end; ++i){
        ans |= CodeGrade(i, stage);
      }
      ++iter;
    }

	}
	return ans;


}


