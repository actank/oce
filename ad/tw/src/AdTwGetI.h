/*
 * AdTwGetI.h
 *
 *  Created on: 2010-9-13
 *      Author: ark
 */

#ifndef ADTWGETI_H_
#define ADTWGETI_H_

#include "Singleton.h"
#include "AdEngine.h"

namespace xce {

namespace ad {
using namespace MyUtil;

class AdTwGetI : public AdMatchManager, public Singleton<AdTwGetI> {
public:
	AdTwGetI();
	virtual ~AdTwGetI();

	virtual AdResultMap GetAds(const string& userProfile,
	                           const string& referer,
	                           int rotate_index,
	                           const Ice::Current& = Ice::Current());

  virtual AdZone2Creatives GetAdsByZone(const string&,
                                        const string&,
                                        Ice::Int,
                                        const ::Ice::Current& = ::Ice::Current());
  virtual AdZone2Creatives GetAdsByZoneInfo(const std::string&,
                                            const std::string&,
                                            Ice::Int,
                                            const xce::ad::ZoneInfoMap&,
                                            const Ice::Current&) {
    return AdZone2Creatives();
  }
  virtual AdZone2Creatives GetAds4Monitor(::Ice::Int uid, ::Ice::Int stage,
      ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
      const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
      int count, const string & referer, int rotate_index, const Ice::Current& = Ice::Current()){
    return AdZone2Creatives();
  }
  virtual void Close(Ice::Int, Ice::Long, const Ice::Current&) { }
  virtual void Click(int uid,
                     const string& ip,
                     const string& text,
                     const string& referer,
                     const string& click_url,
                     const Ice::Current& = Ice::Current());

  virtual AdResultMap GetAdsTest(Ice::Int uid,
                                 Ice::Int stage,
                                 Ice::Int gender,
                                 Ice::Int age,
                                 const ::std::string& currentArea,
                                 const ::std::string& ipArea,
                                 Ice::Long school,
                                 Ice::Int grade,
                                 int count,
                                 const string & referer,
                                 int rotate_index,
                                 const Ice::Current& = Ice::Current());

  virtual void BindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id, const Ice::Current& = Ice::Current()) {};
  virtual void UnbindUser(int user_id, Ice::Long zone_id, const Ice::Current& = Ice::Current()) {};
  virtual void NotifyBindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id, const Ice::Current& = Ice::Current()) {};
  virtual void NotifyUnbindUser(int user_id, Ice::Long zone_id, const Ice::Current& = Ice::Current()) {};

public:
  static int index;
};

}

}

#endif /* ADTWGETI_H_ */
