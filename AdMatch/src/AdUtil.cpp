/*
 * AdUtil.cpp
 *
 *  Created on: Jul 22, 2010
 *      Author: yejingwei
 */

#include "AdUtil.h"
#include <vector>
#include "LogWrapper.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <QueryRunner.h>
#include <DbDescriptor.h>

using namespace xce::ad;
using namespace MyUtil;
using namespace boost;
using namespace com::xiaonei::xce;
//stage 第3个字节的[1-3]位 0x0E000000
//gender 第3个字节的第0位  0x01000000
//age第2个字节            0x00FF0000
//province第1个字节      0x0000FF00
//grade第0个字节         0x000000FF
int AdUtil::Feature2Int(Params & p) {

  int out = 0;
  out |= p.stage << 25;
  out |= p.gender << 24;
  out |= p.age << 16;
  out |= p.province << 8;
  out |= p.grade;
  //
  //  out |= param["stage"] << 25;
  //  out |= param["gender"] << 24;
  //  out |= param["age"] << 16;
  //  out |= param["province"] << 8;
  //  out |= param["grade"];
  return out;
}

map<string, int> AdUtil::Int2Feature(int int_v) {
  map<string, int> features;
  features["stage"] = (int_v & 0x0E000000) >> 25;
  features["gender"] = (int_v & 0x01000000) >> 24;
  features["age"] = (int_v & 0x00FF0000) >> 16;
  features["province"] = (int_v & 0x0000FF00) >> 8;
  features["grade"] = int_v & 0x000000FF;
  //  MCE_DEBUG( "AdUtil::Int2Feature --> int = " << int_v << ",stage = "
  //      << features["stage"] << ",gender = " << features["gender"] << ",age = "
  //      << features["age"] << ",province = " << features["province"]
  //      << ",grade = " << features["grade"]);
  return features;
}

int AdUtil::GetUserKey(Params & params) {
  if (params.stage == 2) { //学生
    params.province = params.current_province;
    return Feature2Int(params);
  } else { //白领
    params.province = params.ip_province;
    params.grade = 0;
    return Feature2Int(params);
  }
}

bool AdUtil::IsChina(Ice::Long area_code) {
  area_code /= 1000000000000;
  return (area_code == 86);
}
//调用这个函数，必须先确保是在中国
bool AdUtil::IsProvince(Ice::Long area_code) {
  Ice::Long head = area_code / 10000000000 * 10000000000;
  return (area_code - head == 0);
}
int AdUtil::ProvinceOf(Ice::Long area_code) {
  Ice::Long head = area_code / 10000000000;
  head -= head / 100 * 100;//减去86
  return head;
}

int AdUtil::CityOf(Ice::Long area_code) {
  Ice::Long head = area_code / 100000000;
  return head % 100;

}

int AdUtil::GetProvinceOfSchool(Ice::Long school) {
  //todo 判断
  if (school_province_map_.find(school) == school_province_map_.end()) {
    return -1;
  }
  return school_province_map_[school];
}

void AdUtil::LoadSchools() {
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select * from school"; //TODO
  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdUtil::LoadSchools:" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("AdUtil::LoadSchools:" << e.what() << ", line:" << __LINE__);
  }

  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_WARN("AdUtil::LoadSchools --> no school in school ");
    return;
  }

  Ice::Long area, school;
  int province;
  for (size_t i = 0; i < res.size(); ++i) {
    school = (Ice::Long) (res.at(i)["id"]);
    area = (Ice::Long) (res.at(i)["area_code"]);
    province = AdUtil::ProvinceOf(area);
    school_province_map_[school] = province;
  }
  MCE_INFO("AdUtil::LoadSchools --> school_province_map_ --> count = " << school_province_map_.size());
}

//将省编号转换成方块坐标

/*------------------------------------------------------------------------------------
 所有可能的省，包括直辖市 34个
 国外映射到0
 11,12,13,14,15,\  11映射到1
 21,22,23,\
31,32,33,34,35,36,37,\
41,42,43,44,45,46,\
50,51,52,53,54,\
61,62,63,64,65,\
71,\
81,82
 */

/*
 所有可能的年级
 41,42,43,44,45\
51,52,53\
61\
小学初中高中全部映射为0
 所有博士都归为61

 */

void BidUnit::Init(BidParamsPtr & params, unsigned index) {
  service_index_ = index;
  stages_int_ = atoi(params->stages.c_str());
  if (stages_int_ == 2 || stages_int_ == 6) {
    stages_.insert(2);
  }
  if (stages_int_ == 4 || stages_int_ == 6) {
    stages_.insert(4);
  }

  ParseGenders(params->genders);
  ParseAges(params->ages);
  ParseProvinces(params->areas);
  ParseGrades(params->grades);
  ParseSchools(params->schools);

}

vector<int> BidUnit::GetKeys() {
  vector<int> keys;
  keys.reserve(20000);
  int i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0;

  MCE_DEBUG("BidUnit::GetKeys --> stages_ = " << PrintHelper(stages_) << ",genders_ = " << PrintHelper(genders_)
      << ",ages = " << PrintHelper(ages_) << ", provinces_ " << PrintHelper(provinces_) << ", grades_ " << PrintHelper(grades_));
  for (set<int>::iterator itr0 = stages_.begin(); itr0 != stages_.end(); ++itr0, ++i0) {
    for (set<int>::iterator itr4 = grades_.begin(); itr4 != grades_.end(); ++itr4, ++i4) {
      if (*itr0 == 4 && *itr4 != 0) {
        continue;
      }
      if (*itr0 == 2 && *itr4 == 0) {
        continue;
      }
      for (set<int>::iterator itr1 = genders_.begin(); itr1 != genders_.end(); ++itr1, ++i1) {
        for (set<int>::iterator itr2 = ages_.begin(); itr2 != ages_.end(); ++itr2, ++i2) {
          for (set<int>::iterator itr3 = provinces_.begin(); itr3
              != provinces_.end(); ++itr3, ++i3) {
            Params p;
            p.stage = *itr0;
            p.gender = *itr1;
            p.age = *itr2;
            p.province = *itr3;
            p.grade = *itr4;

            //            map<string, int> params;
            //            params["stage"] = *itr0;
            //            params["gender"] = *itr1;
            //            params["age"] = *itr2;
            //            params["province"] = *itr3;
            //            params["grade"] = *itr4;
            int key = AdUtil::Feature2Int(p);
            keys.push_back(key);

            // MCE_DEBUG("BidUnit::GetKeys --> " << *itr0 << ","<< *itr1<< "," << *itr2 << ","<< *itr3 << ","<< *itr4)

          }
        }
      }
    }
  }
  //MCE_INFO(__LINE__ << ", " << __FILE__);
  return keys;
}

void BidUnit::ParseGenders(string & param) {

  int genders_int = atoi(const_cast<char *> (param.c_str()));
  if (genders_int == 0 || genders_int == 2) {
    genders_.insert(0);
  }
  if (genders_int == 1 || genders_int == 2) {
    genders_.insert(1);
  }
}

void BidUnit::ParseAges(string & param) {

  //只选年纪，没选年龄
  if (param.empty()) {
    for (int i = 0; i <= 8; ++i) { //不能超过80岁
      ages_.insert(i * 10 + service_index_);
    }
    return;
  }

  //根据广告系统，一个group只能投放一个年龄段
  tokenizer<> tok(param);
  //  tokenizer<>::iterator tit = tok.begin();
  int size = 0;
  for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
    ++size;
  }

  if (size == 0 || size % 2 == 1) {
    for (int i = 0; i <= 8; ++i) {
      ages_.insert(i * 10 + service_index_);
    }
    return;
  }

  for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
    int begin, end;
    begin = lexical_cast<int> (*tit);
    ++tit;
    end = lexical_cast<int> (*tit);
    end = end < 100 ? end : 100;
    for (int i = begin; i <= end; ++i) {
      if (i % 10 == service_index_) {
        ages_.insert(i);
      }
    }
  }

}
void BidUnit::ParseProvinces(string & param) {
  if (stages_int_ == 2) {
    return;
  }

  //说明投了白领，stage是4或者6
  if (param == "0086000000000000") { //全国
    for (size_t i = 0; i < sizeof(kProvinces) / sizeof(kProvinces[0]); ++i) { //TODO 是否从0开始
      provinces_.insert(kProvinces[i]); //对于白领，投放在中国所有地区
    }
    areas_.insert(86000000000000);
    if (stages_int_ == 6) { //如果也投了学生
      schools_.insert(-86000000000000);
    }
    return;
  }
  tokenizer<> tok(param);
  for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
    Ice::Long value = lexical_cast<Ice::Long> (*tit);
    int prv = AdUtil::ProvinceOf(value);
    provinces_.insert(prv);
    areas_.insert(value);
    if (stages_int_ == 6) { //如果也投了学生
      schools_.insert(-value);
    }
  }
}

void BidUnit::ParseGrades(string & param) {

  if (stages_int_ == 6) { //投学生并且白领
    for (int i = 0; i < sizeof(kGrades) / sizeof(kGrades[0]); ++i) {
      grades_.insert(kGrades[i]);
    }
    grades_.insert(0);//TODO 有问题
    return;
  }
  if (stages_int_ == 4) { //只投白领
    grades_.insert(0);
    return;
  }

  //只投学生
  //1 选年龄，没选年级
  if (param.empty()) {
    for (int i = 0; i < sizeof(kGrades) / sizeof(kGrades[0]); ++i) {
      grades_.insert(kGrades[i]);
    }
    return;
  }
  //2 选年级，没选年龄
  tokenizer<> tok(param);
  int size = 0;
  for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
    ++size;
  }

  if (size == 0 || size % 2 == 1) {
    for (size_t i = 0; i < sizeof(kGrades) / sizeof(kGrades[0]); ++i) {
      grades_.insert(kGrades[i]);
    }
    return;
  }

  for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end();) {
    int left = lexical_cast<int> (*tit);
    ++tit;
    if (left == 61) { //对于博士，单独处理
      grades_.insert(61);
      ++tit;
      continue;
    }

    int right = lexical_cast<int> (*tit);
    for (size_t i = left; i <= right; ++i) {
      grades_.insert(i);
    }
    ++tit;
  }
}
void BidUnit::ParseSchools(string & param) {

  if (stages_int_ == 4 || stages_int_ == 6) { //只投白领 或者投  学生+白领
    return;
  }

  //只投学生
  set<Ice::Long> school_prv;
  tokenizer<> tok(param);
  for (tokenizer<>::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
    string token = *tit;
    if (token.empty()) {
      continue;
    }
    if (token.substr(0, 4) == "area") {
      Ice::Long china_or_province = lexical_cast<Ice::Long> (token.substr(4));
      if (china_or_province == 86000000000000) {
        for (size_t i = 0; i < sizeof(kProvinces) / sizeof(kProvinces[0]); ++i) { //TODO 是否从0开始
          provinces_.insert(kProvinces[i]); //对于白领，投放在中国所有地区
        }
        schools_.insert(-86000000000000);
        return;
      } else {
        int prv = AdUtil::ProvinceOf(china_or_province);
        provinces_.insert(prv);
        schools_.insert(-china_or_province);
      }
    } else {
      Ice::Long school_id = lexical_cast<Ice::Long> (token);
      //得到这个school所在的area;
      int school_province = AdUtil::instance().GetProvinceOfSchool(school_id);
      if (school_province == -1) { //找不到这个学校所在的省份
        continue;
      }
      provinces_.insert(school_province);
      schools_.insert(school_id);
    }
  }

}

