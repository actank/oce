#ifndef __USER_BORN_ADAPTER_H__
#define __USER_BORN_ADAPTER_H__

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
  UserBornInfoPtr parse(const UserBornDataPtr& source) {astrology_=source->astrology;birthDay_=source->birthDay;
    birthMonth_=source->birthMonth;birthYear_=source->birthYear;cityCode_=source->cityCode;gender_=source->gender;
    homeCity_=source->homeCity;homeProvince_=source->homeProvince;id_ = source->id;return this;}
  int id() const { return id_;}
  int birthMonth() const { return birthMonth_;}
  int birthDay() const { return birthDay_;}
  int birthYear() const { return birthYear_;}
  std::string astrology() const { return astrology_;}
  std::string cityCode() const { return cityCode_;}
  std::string gender() const { return gender_;}
  std::string homeCity() const { return homeCity_;}
  std::string homeProvince() const { return homeProvince_;}
  void set_id(int id) {id_ = id;mark("ID", boost::lexical_cast<std::string>(id));}
  void set_birthMonth(int birthMonth) {birthMonth_ = birthMonth;mark("BIRTH_MONTH", boost::lexical_cast<std::string>(birthMonth));}
  void set_birthDay(int birthDay) {birthDay_ = birthDay;mark("BIRTH_DAY", boost::lexical_cast<std::string>(birthDay));}
  void set_birthYear(int birthYear) {birthYear_ = birthYear;mark("BIRTH_YEAR", boost::lexical_cast<std::string>(birthYear));}
  void set_astrology(const std::string astrology) {astrology_ = astrology;mark("ASTROLOGY", astrology);}
  void set_cityCode(const std::string cityCode) {cityCode_ = cityCode;mark("CITY_CODE", cityCode);}
  void set_gender(const std::string gender) {gender_ = gender;mark("GENDER", gender);}
  void set_homeCity(const std::string homeCity) {homeCity_ = homeCity;mark("HOME_CITY", homeCity);}
  void set_homeProvince(const std::string homeProvince) {homeProvince_ = homeProvince;mark("HOME_PROVINCE", homeProvince);}
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
};

class UserBornAdapter : public MyUtil::ReplicatedClusterAdapterI<UserBornManagerPrx>,
	public MyUtil::Singleton<UserBornAdapter>
{
public:
	UserBornAdapter() : MyUtil::ReplicatedClusterAdapterI<UserBornManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel,true,"UBOM"){
	}
	UserBornInfoPtr getUserBorn(int id, const Ice::Context& ctx = Ice::Context());
	void reload(int id);
  void setUserBorn(int id, const UserBornInfoPtr& info);
  void createUserBorn(int id, const UserBornInfoPtr& info);
		

	UserBornManagerPrx getUserBornManager(int userId);
	UserBornManagerPrx getUserBorn10sManager(int userId);

};

}
}
}

#endif

