/*
 * AdUtil.h
 *
 *  Created on: Jul 22, 2010
 *      Author: yejingwei
 */

#ifndef ADUTIL_H_
#define ADUTIL_H_
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <IceUtil/RWRecMutex.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <Util.h>
#include "LogWrapper.h"
#include <QueryRunner.h>
#include <DbDescriptor.h>

#include "TaskManager.h"
#include "Ice/Config.h"

namespace xce {
namespace ad {
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace std;
using namespace Ice;
static const string db_instance = "ad_db";//TODO 修改

const int kProvinces[] = { 0, 11, 12, 13, 14, 15, 21, 22, 23, 31, 32, 33, 34,
    35, 36, 37, 41, 42, 43, 44, 45, 46, 50, 51, 52, 53, 54, 61, 62, 63, 64, 65,
    71, 81, 82 };
const int kGrades[] = { 41, 42, 43, 44, 45, 51, 52, 53, 61 };
const unsigned kStudentCube = 4;
const unsigned kWorkerCube = 3;

const unsigned kMaxAge = 9, kMaxProvince = 34, kMaxStage = 2, kMaxGender = 1,
    kMaxGrade = 9;

template<typename T>
static string PrintHelper(const T & container) { //将容器内容打印成字符串
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  for (typename T::const_iterator itr = container.begin(); itr
      != container.end(); ++itr) {
    if (first_item == true) {
      first_item = false;
    } else {
      ios << ",";
    }
    ios << *itr;
  }
  ios << ")";
  return ios.str();
}


struct Params {
  int age;
  int stage;
  int grade;
  int province;
  int gender;
  int current_province;
  int ip_province;

  Ice::Long current_area;
  Ice::Long ip_area;
  Ice::Long school;

};

class BidParams: public IceUtil::Shared {
public:
  Ice::Long group_id;
  Ice::Long bid_id;
  string genders;
  string ages;
  string stages;
  string grades;
  string areas;
  string schools;
};

typedef IceUtil::Handle<BidParams> BidParamsPtr;
typedef vector<BidParamsPtr> BidParamsSeq;

class AdUtil: public Singleton<AdUtil> {
public:
  void Init() {
    MCE_DEBUG( __FILE__ << ":" << __LINE__);
    LoadSchools();
  }
  static int GetUserKey(Params & param);

  static int Feature2Int(Params & p);
  static map<string, int> Int2Feature(int int_V);

  static bool IsChina(Ice::Long area_code);
  static bool IsAllProvince(Ice::Long area_code) {
    return true;
  }
  static bool IsProvince(Ice::Long area_code);
  static int ProvinceOf(Ice::Long area_code);
  static bool IsCity(Ice::Long area_code);
  static int CityOf(Ice::Long area_code);

  void LoadSchools();
  int GetProvinceOfSchool(Ice::Long school);

  map<Ice::Long, int> school_province_map_;

};

class BidUnit: public IceUtil::Shared {
public:
  void Init(BidParamsPtr & params, unsigned index);

  vector<int> GetKeys();

  //  bool Match(const Params & params);

  void ParseAges(string & param);
  void ParseGenders(string & param);
  void ParseProvinces(string & param);
  void ParseGrades(string & param);
  void ParseSchools(string & param);

  set<int> stages_, genders_, ages_, provinces_, grades_;

  set<Ice::Long> areas_;
  set<Ice::Long> schools_;

  int stages_int_;

  unsigned service_index_;
};
typedef IceUtil::Handle<BidUnit> BidUnitPtr;
}
}

#endif /* ADUTIL_H_ */
