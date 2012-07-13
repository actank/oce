#include "AdStruct.h"
#include "Date.h"
#include "AdStyleDispatcher.h"
#include <bitset>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "AdConfig.h"
#include <exception>
namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace boost;

AdStylePtr AdStyleDispatcher:: Dispatch(Ice::Long zoneId, UserProfile &user ) { 
  IceUtil::RWRecMutex::RLock lock(mutex_);
  MCE_DEBUG("id = "<< zoneId);
  map<Ice::Long, vector<Ice::Long> >::const_iterator mit = zone2style_.find(zoneId);
  if (mit != zone2style_.end()) {
    vector<Ice::Long> styles = mit->second;
    Ice::Long style_id = styles[user.id()%100];
    return AdStylePool::instance().GetAdStyle(style_id);
  }
  return 0;
}

map< Ice::Long , vector<Ice::Long> >  AdStyleDispatcher:: Update( ) {
  //生成zone2style
  MCE_INFO("AdStyleDispatcher::Update(); Start");
  vector<AdExperimentPtr> exps = AdExperimentPool::instance().ListAdExperiment();
  MCE_INFO("Experiment's size=" << exps.size());
  sort(exps.begin(),exps.end(),AdExperimentPool::compare);
  vector<AdExperimentPtr> myvec ;
  Ice::Long last = -1;
  map< Ice::Long , vector<Ice::Long> > zone2style;
  AdExperiment tmp ;
  tmp.set_experiment_id(last);
  AdExperimentPtr tmpPtr = &tmp;
  exps.push_back(tmpPtr);
  for(vector<AdExperimentPtr>::iterator it=exps.begin(); it != exps.end();++it){
    AdExperimentPtr ptr = *it;
    if( ptr->experiment_id() != last){
      if( last != -1 ){
        Ice::Long zoneid = (*myvec.begin())->zone_id();
        MCE_INFO("ZoneId: " << zoneid);
        vector<Ice::Long> styles;
        int style = 0;
        for(vector<AdExperimentPtr>::iterator iit = myvec.begin();iit!=myvec.end();iit++){
          AdExperimentPtr iptr = *iit;
          int traffic = iptr->traffic();
          style = iptr->style_id();
          MCE_INFO("AdExperimentPtr: " << style << "\t" << traffic);
          while( traffic>0){
            --traffic;
            styles.push_back(style);
          }
        }
        zone2style.insert(make_pair<Ice::Long,vector<Ice::Long> >(zoneid,styles));
      }
      myvec.clear();
    }
    myvec.push_back(ptr);
    last = ptr->experiment_id();
  }
  MCE_INFO("AdStyleDispatcher::Update(); DONE");
  return zone2style;
}

void AdStyleDispatcher::Init( ) {
  MCE_INFO("AdStyleDispatcher::Init --> AdStylePool::instance().Init(); Start");
  AdStylePool::instance().Init();
  MCE_INFO("AdStyleDispatcher::Init --> AdStylePool::instance().Init(); DONE");
  MCE_INFO("AdStyleDispatcher::Init --> AdExperimentPool::instance().Init(); Start");
  AdExperimentPool::instance().Init();
  MCE_INFO("AdStyleDispatcher::Init --> AdExperimentPool::instance().Init(); DONE");
  //生成zone2style
  //MCE_INFO("AdStyleDispatcher::Update(); Start");
  vector<AdExperimentPtr> exps = AdExperimentPool::instance().ListAdExperiment();
  MCE_INFO("Experiment's size=" << exps.size());
  sort(exps.begin(),exps.end(),AdExperimentPool::compare);
  vector<AdExperimentPtr> myvec ;
  Ice::Long last = -1;
  map< Ice::Long , vector<Ice::Long> > zone2style;
  AdExperimentPtr tmpPtr = new AdExperiment;
  tmpPtr->set_experiment_id(last);
  exps.push_back(tmpPtr);
  for(vector<AdExperimentPtr>::iterator it=exps.begin(); it != exps.end();++it){
    AdExperimentPtr ptr = *it;
    if( ptr->zone_id() != last){
      if( last != -1 ){
        Ice::Long zoneid = (*myvec.begin())->zone_id();
        MCE_INFO("ZoneId: " << zoneid);
        vector<Ice::Long> styles;
        int style = 0;
        for(vector<AdExperimentPtr>::iterator iit = myvec.begin();iit!=myvec.end();iit++){
          AdExperimentPtr iptr = *iit;
          int traffic = iptr->traffic();
          style = iptr->style_id();
          MCE_INFO("AdExperimentPtr: " << style << "\t" << traffic);
          while( traffic>0 && styles.size() < 100){
            --traffic;
            styles.push_back(style);
          }
        }
        zone2style.insert(make_pair<Ice::Long,vector<Ice::Long> >(zoneid,styles));
      }
      myvec.clear();
    }
    myvec.push_back(ptr);
    last = ptr->zone_id();
  }
  if( !zone2style.empty() ){
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      zone2style_.swap(zone2style);
    }
  }
  MCE_INFO("AdStyleDispatcher::Init(); DONE");
}

void AdStyleDispatcher::UpdateCheckTimer::handle() {
  AdStyleDispatcher::instance().Init();
}

}
}
