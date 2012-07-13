/*
 * AdData.cpp
 *
 *  Created on: 2010-12-13
 *      Author: ark
 */

#include "IceLogger.h"
#include "AdUrlMaping.h"
//#include "AdData.h"
#include "AdTwDebugI.h"

namespace xce {
namespace ad {

MyUtil::LongSeq AdTwDebugI::UrlMaping(const ::std::string& url, ::Ice::Int stage, const ::Ice::Current&) {
  set<Ice::Long> result;
  AdUrlMaping::instance().GetAdZone(url, (int)stage, result);
  return MyUtil::LongSeq(result.begin(), result.end());
}

}
}
