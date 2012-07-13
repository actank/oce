#ifndef __USER_BORN_READER_ADAPTER_H__
#define __USER_BORN_READER_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Markable.h"
#include "boost/lexical_cast.hpp"

namespace xce {
namespace adapter {
namespace userborn {

using namespace ::xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;

class UserBornInfo;

typedef IceUtil::Handle<UserBornInfo> UserBornInfoPtr;

class UserBornInfo : virtual public Ice::Object, public Markable {
public:
  UserBornInfoPtr parse(const UserBornDataNPtr& source) {astrology_=source->astrology;birthDay_=source->birthDay;
    birthMonth_=source->birthMonth;birthYear_=source->birthYear;cityCode_=source->cityCode;gender_=source->gender;
    homeCity_=source->homeCity;homeProvince_=source->homeProvince;id_ = source->id;isLunarBirth_=source->isLunarBirth;return this;}
  int id() const { return id_;}
  int birthMonth() const { return birthMonth_;}
  int birthDay() const { return birthDay_;}
  int birthYear() const { return birthYear_;}
  std::string astrology() const { return astrology_;}
  std::string cityCode() const { return cityCode_;}
  std::string gender() const { return gender_;}
  std::string homeCity() const { return homeCity_;}
  std::string homeProvince() const { return homeProvince_;}
  bool isLunarBirth() const { return isLunarBirth_;}
  void set_id(int id) {id_ = id;mark("ID", boost::lexical_cast<std::string>(id));}
  void set_birthMonth(int birthMonth) {birthMonth_ = birthMonth;mark("BIRTH_MONTH", boost::lexical_cast<std::string>(birthMonth));}
  void set_birthDay(int birthDay) {birthDay_ = birthDay;mark("BIRTH_DAY", boost::lexical_cast<std::string>(birthDay));}
  void set_birthYear(int birthYear) {birthYear_ = birthYear;mark("BIRTH_YEAR", boost::lexical_cast<std::string>(birthYear));}
  void set_astrology(const std::string astrology) {astrology_ = astrology;mark("ASTROLOGY", astrology);}
  void set_cityCode(const std::string cityCode) {cityCode_ = cityCode;mark("CITY_CODE", cityCode);}
  void set_gender(const std::string gender) {gender_ = gender;mark("GENDER", gender);}
  void set_homeCity(const std::string homeCity) {homeCity_ = homeCity;mark("HOME_CITY", homeCity);}
  void set_homeProvince(const std::string homeProvince) {homeProvince_ = homeProvince;mark("HOME_PROVINCE", homeProvince);}
  void set_isLunarBirth(bool isLunarBirth) {isLunarBirth_= isLunarBirth;mark("IS_LUNARBIRTH", boost::lexical_cast<std::string>(isLunarBirth));}
private:
  int id_;
  int birthMonth_;
  int birthDay_;
  int birthYear_;
  std::string astrology_;
  std::string cityCode_;
  std::string gender_;
  std::string homeCity_;
  std::string homeProvince_;
  bool isLunarBirth_;
};

class UserBornReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserBornReaderPrx>,
	public MyUtil::Singleton<UserBornReaderAdapter>
{
public:
	UserBornReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<UserBornReaderPrx>("ControllerUserBaseReader",120,300,new MyUtil::OceChannel,true,"UBOM"){
	}
	UserBornInfoPtr getUserBorn(int id, const Ice::Context& ctx = Ice::Context());
	void setUserBorn(int id, const MyUtil::Str2StrMap& props);

	void setData(int id, const Ice::ObjectPtr& data);
		

	UserBornReaderPrx getUserBornReaderPrx(int userId);

};

}
}
}

#endif

