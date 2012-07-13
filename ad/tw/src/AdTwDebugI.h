/*
 * AdData.h
 *
 *  Created on: 2010-12-13
 *      Author: ark
 */

#ifndef ADTWDEBUG_H_
#define ADTWDEBUG_H_

#include "Singleton.h"
#include "AdTwDebug.h"

namespace xce {
namespace ad {

class AdTwDebugI : public AdTwDebug, public MyUtil::Singleton<AdTwDebugI> {
public:
  virtual ::MyUtil::LongSeq UrlMaping(const ::std::string&, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());
};

}
}

#endif 
