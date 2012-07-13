/*
 * AdData.h
 *
 *  Created on: 2010-12-13
 *      Author: ark
 */

#ifndef ADDATA_H_
#define ADDATA_H_

namespace xce {

namespace ad {

class TwZone : public IceUtil::Shared {
public:
  TwZone();
  TwZone(Ice::Long id, int count);
  virtual ~TwZone();
  int count() {
    return count_;
  }
  void set_count(int count) {
    count_ = count;
  }
  Ice::Long id() {
    return id_;
  }
  void set_id(Ice::Long id) {
    id_ = id;
  }
  bool operator<(const TwZone& right) const {
    return this->id_ < right.id_;
  }
//  bool operator<(const TwZone& right) const {
//    return this->id_ < right->id_;
//  }
//  bool operator<(const TwZone& right) const {
//    return this->id_ < right->id_;
//  }

private:
  Ice::Long id_; //zone id
  int count_;
};
typedef IceUtil::Handle<TwZone> TwZonePtr;

class TwZonePool : public MyUtil::Singleton<TwZonePool> {
public:
  TwZonePtr GetAdZone(const Ice::Long adzone_id);
  void GetAdZone(const set<Ice::Long>& adzone_ids, set<TwZonePtr>& adzones);
  bool Load();
private:
  map<Ice::Long, TwZonePtr> zone_pool_;
  IceUtil::RWRecMutex mutex_;
};

class TwZoneLoadTimer : public MyUtil::Timer{
public:
  TwZoneLoadTimer(int time = 0):Timer(time){}
  virtual void handle() {
    MyUtil::TaskManager::instance().schedule(new xce::ad::TwZoneLoadTimer(60*1000));
    try{
      TwZonePool::instance().Load();
    } catch(Ice::Exception& e) {
      MCE_WARN("TwZonePool::Load() error Ice : " << e);
    } catch(std::exception& e) {
      MCE_WARN("TwZonePool::Load() error std : " << e.what());
    } catch(...) {
      MCE_WARN("TwZonePool::Load() error : unknown error ...");
    }
  }
};


}

}

#endif /* ADDATA_H_ */
