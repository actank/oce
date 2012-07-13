/*
 * AdUrlMaping.h
 *
 *  Created on: 2010-12-13
 *      Author: ark
 */

#ifndef ADURLMAPING_H_
#define ADURLMAPING_H_

#include <string>
#include <set>
#include <vector>
#include <Ice/Ice.h>
#include "Singleton.h"
#include "TaskManager.h"

namespace xce {
namespace ad {

class AdUrlMaping : public MyUtil::Singleton<AdUrlMaping> {
public:
  AdUrlMaping();
  virtual ~AdUrlMaping();
  void GetAdZone(const string& Url, const int Stage, set<Ice::Long>& AdZones);
  bool Load();

private:
  void PreProcess(const string& Url, vector<string>& UrlSeq);

  map<pair<string,int>, set<Ice::Long> > map_;
  IceUtil::RWRecMutex mutex_;
};

class AdUrlMapingLoadTimer : public MyUtil::Timer{
public:
  AdUrlMapingLoadTimer(int time = 60*1000):Timer(time){}
  virtual void handle() {

    MyUtil::TaskManager::instance().schedule(new AdUrlMapingLoadTimer(60*1000));//1分钟
    AdUrlMaping::instance().Load();
  }
};

}
}
#endif /* ADURLMAPING_H_ */
