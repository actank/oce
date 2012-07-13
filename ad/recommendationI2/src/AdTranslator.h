/*
 * AdTranslator.h
 *
 * The Translator module
 *
 *  Created on: Oct 11, 2010
 *      Author: zhaohui.tang
 */

#ifndef ADTRANSLATOR_H
#define ADTRANSLATOR_H

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
//#include <Ice/Ice.h>
#include <iostream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"
#include "Singleton.h"
#include "ServiceI.h"

namespace xce {
namespace ad {

using namespace std;

const int AREA_POS = 0; //0
const int GRADE_POS = 0 + 36; //36
const int STAGE_POS = 0 + 36 + 10; //46
const int GENDER_POS = 0 + 36 + 10 + 3;//49
const int AGE_POS = 0 + 36 + 10 + 3 + 2;//51

const uint64_t ONLY_AREA_SET = 68719476735;
const uint64_t ONLY_GRADE_SET = 70300024700928;
const uint64_t ONLY_STAGE_SET = 492581209243648;
const uint64_t ONLY_GENDER_SET = 1688849860263936;
const uint64_t ONLY_AGE_SET = 2303591209400008704;

class BidUnit {
public:
  BidUnit() {
    gender_ = 1;
    stage_ = 4;
    grade_ = "";
    age_ = "";
    area_ = "";
    school_ = "";
  }
  string school() const {
    return school_;
  }
  void set_school(string school) {
    this->school_ = school;
  }
  string area() const {
    return area_;
  }
  void set_area(string area) {
    this->area_ = area;
  }
  string age() const {
    return age_;
  }
  void set_age(string age) {
    this->age_ = age;
  }
  string grade() const {
    return grade_;
  }
  void set_grade(string grade) {
    this->grade_ = grade;
  }
  int stage() const {
    return stage_;
  }
  void set_stage(int stage) {
    this->stage_ = stage;
  }
  int gender() const {
    return gender_;
  }
  void set_gender(int gender) {
    this->gender_ = gender;
  }
  Ice::Long bid_unit_id() const {
    return bid_unit_id_;
  }
  void set_bid_unit_id(Ice::Long bid_unit_id) {
    this->bid_unit_id_ = bid_unit_id;
  }
private:
  Ice::Long bid_unit_id_;
  int gender_;  //投放性别（0：女，1：男，2：全部）
  int stage_;  //投放用户阶段，1：中学，2：大学，4：白领；投放组合数字相加即可
  string grade_;
  string age_;
  string area_;
  string school_;
};

class UserBase {
public:
  UserBase() {
    gender_ = 1;
    stage_ = 4;
    grade_ = 41;
    age_ = 25;
    area_ = "";
  }
  static UserBase MakeUser(UserProfile & profile);
  static void CookieEmptyCheck(UserProfile& upf); // if cookie is empty, return default UserProfile
  string area() const {
    return area_;
  }
  void set_area(string area) {
    this->area_ = area;
  }
  int age() const {
    return age_;
  }
  void set_age(int age) {
    this->age_ = age;
  }
  int grade() const {
    return grade_;
  }
  void set_grade(int grade) {
    this->grade_ = grade;
  }
  int stage() const {
    return stage_;
  }
  void set_stage(int stage) {
    this->stage_ = stage;
  }
  int gender() const {
    return gender_;
  }
  void set_gender(int gender) {
    this->gender_ = gender;
  }
private:
  int gender_;
  int stage_;
  int grade_;
  int age_;
  string area_;
};

/***********************************************************************************************************************/
/*
 * Provide the service translating UserBase and BidUnit into the Index key,
 * and other other related services.
 */
class Translator : public MyUtil::Singleton<Translator> {
public:
  uint64_t GetIndex(const UserBase& user);
  uint64_t GetIndex(const BidUnit& bid_unit);
  uint64_t GetIndex(const UserProfile& profile);
  bool Match(const uint64_t& user_key, const uint64_t& bid_unit_key);  // Is this user_key in this bidUnit_key.
  int server_index() const {
    return server_index_;
  }
  void set_server_index(int server_index) {
    this->server_index_ = server_index;
  }
  bool IsGlobal(const uint64_t& user_key) const { // 是否为投放到所有有效用户的bidunit_key
    return user_key == 1150599336048984063;
  }
private:
  int server_index_;
};

/***********************************************************************************************************************/
/*
 * All bidunit's utility methods except area and school.
 */
class CommonUtil : public MyUtil::Singleton<CommonUtil> {
public:
  vector<uint64_t> Split(const string& str, boost::char_separator<char> separator); // Split the input with separator, and turn per to unit64_t.
  uint64_t CodeAge(int age);
  uint64_t CodeAge(const string& age);
  uint64_t CodeGender(int gender);
  uint64_t CodeStage(int stage);
  uint64_t CodeGrade(int grade, int stage);
  uint64_t CodeGrade(const string& grade, int stage);
private:
  const static int GRADE_MAP_SIZE = 70;
  const static int GRADE_MAP[GRADE_MAP_SIZE];
};

/***********************************************************************************************************************/
/*
 * All utility methods about area.
 */
class AreaUtil : public MyUtil::Singleton<AreaUtil> {
public:
  vector<uint64_t> ParseAreas(const string& str);
  uint64_t CodeArea(int area);
  uint64_t CodeArea(const string& area);
  uint64_t GetProvince(uint64_t area);
  uint64_t GetCity(uint64_t area);
  bool IsArea(uint64_t area_or_school) {
    return area_or_school > 999999999999;//学校码小于这个数，地域码都大于这个数
  }
  uint64_t GetChina() {
    return 86000000000000;
  }
private:
  const static int AREA_MAP_SIZE = 83;
  const static int AREA_MAP[AREA_MAP_SIZE];
};

/***********************************************************************************************************************/
/*
 * All utility methods about school.
 */
class SchoolUtil : public MyUtil::Singleton<SchoolUtil> {
public:
  vector<uint64_t> ParseSchools(const string& str);
  uint64_t CodeSchool(const string& school);
};

} // end namespace ad
} // end namespace xce

#endif // ADTRANSLATOR_H
