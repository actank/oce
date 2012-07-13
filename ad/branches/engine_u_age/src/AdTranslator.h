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

class BidUnit : public IceUtil::Shared {
public:
  BidUnit(int gender,
          int stage,
          string& grade,
          string& age,
          string& school,
          string& area);
  bool Match(const UserProfile& userProfile);

private:
  enum MatchScope { Area, SchoolArea };
  static bool AreaMatch(uint64_t area_key, const vector<uint64_t>& area);
  short gender_;
  short stage_;
  vector<short> grade_;
  vector<short> age_;
  vector<uint64_t> school_;
  vector<uint64_t> area_;
  vector<uint64_t> school_area_;
};
typedef IceUtil::Handle<BidUnit> BidUnitPtr;


class Translator : public MyUtil::Singleton<Translator> {
public:

  static bool ProcessGender(const int gender_code, vector<short>& genders);
  static bool ProcessStage(const int stage_code, vector<short>& stages);
  static bool ProcessGrade(const string& grade_code, vector<short>& grades);
  static bool ProcessAge(const string& age_code, vector<short>& ages);
  static bool ProcessSchool(const string& school_code, vector<uint64_t>& areas, vector<uint64_t>& schools);
  static bool ProcessArea(const string& area_code, vector<uint64_t>& areas);
private:
  static bool ProcessLeadingArea(const string& area_code, vector<uint64_t>& areas);
  static bool ProcessRealSchool(const string& school_code, vector<uint64_t>& schools);
};

class CommonUtil {
public:
  static vector<uint64_t> Split(const string& str, boost::char_separator<char> separator); // Split the input with separator, and turn per to unit64_t.
};

} // end namespace ad
} // end namespace xce
#endif // ADTRANSLATOR_H
