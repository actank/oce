#ifndef USER_DANGER_H
#define USER_DANGER_H

#include <ServiceI.h>
#include <UserDanger.h>
#include <QueryRunner.h>
#include <bitset>

namespace xce {
namespace userdanger {

const int CHANGE_BITSET_SIZE = 10485760;
const int CHANGE_RBSET_SIZE = 1048576;
const int CLIENT_SIZE = 1;
const int STATUS_CACHE_SIZE = 2147483647;
const std::string TABLE_USER_DANGER = "user_danger";
const std::string DB_USER_DANGER = "user_danger";
const std::string FIELD_USERID = "user_id";
const std::string FIELD_TYPE = "type";
const int LOAD_SIZE = 3000;
const int BIT_SET_SIZE = 2147483647;

const int RB_SET_TYPE = 0;
const int BIT_SET_TYPE = 1;

class Updater : public xce::storm::Observer {
  virtual bool update(const MyUtil::Str2StrMap& context, const Ice::Current& e);
};

typedef IceUtil::Handle<Updater> UpdaterPtr;

class UpdateTask : public MyUtil::Task {
public:
  UpdateTask(const MyUtil::Str2StrMap& c) : context_ (c){}
  void handle();
private:
  MyUtil::Str2StrMap context_;
};

class Set : public Ice::Object {
public:
  virtual void add(int) = 0;
  virtual void remove(int) = 0;
  virtual bool has(int) const = 0;
  virtual int size() const = 0;
  virtual int type() const = 0;
};

class BitSet : public Set {
public:
  BitSet();
  void add(int);
  void remove(int);
  bool has(int) const;
  int size() const;
  int type() const {return BIT_SET_TYPE;}  
  std::bitset<BIT_SET_SIZE> _set;
};

class RBSet : public Set {
public:
  void add(int);
  void remove(int);
  bool has(int) const;
  int size() const;
  int type() const {return RB_SET_TYPE;}
  std::set<int> _set;
};

typedef IceUtil::Handle<RBSet> RBSetPtr;

typedef IceUtil::Handle<Set> SetPtr;

class DangerSet : public Ice::Object {
public:
  DangerSet(int type) : _type(type), _set(new RBSet), _wflag(false), _lastwritten(time(0)) {}
  void setOn(int userid);
  void setOff(int userid);
  bool getType(int userid);
  IntSeq getIds() const;
  Int2Bool getTypes(const IntSeq& userids);
  void check();
  int size() const;
private:
  SetPtr getSet() const;
  void setSet(const SetPtr& set);

  int _type;
  SetPtr _set;
  IceUtil::RWRecMutex _mutex;
  IceUtil::RWRecMutex _setmutex;
  bool _wflag;
  time_t _lastwritten;
};

typedef IceUtil::Handle<DangerSet> DangerSetPtr;

class UserDangerManagerI : virtual public MyUtil::Singleton<UserDangerManagerI>,
  virtual public UserDangerManager, virtual public IceUtil::Thread {
public:
  UserDangerManagerI() {start(128 * 1024).detach();}
  virtual bool isValid(const Ice::Current& current = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& current = Ice::Current());
  virtual void setTypeOn(int userid, int type, const Ice::Current& current = Ice::Current());
  virtual void setTypeOff(int userid, int type, const Ice::Current& current = Ice::Current());
  virtual bool isType(int userid,int type,  const Ice::Current& current = Ice::Current());
  virtual Int2Bool isTypes(const IntSeq& userids,int type,  const Ice::Current& current = Ice::Current());
  virtual IntSeq getUserIds(int type, const Ice::Current& current = Ice::Current());
  virtual Int2IntSeq getClientCache(const Ice::Current& current = Ice::Current());
  void check();
  void update(int id, int type, bool operation);
private:
  void run();
  void preload();
  void reload();

  DangerSetPtr getDangerSet(int type);
  DangerSetPtr setDangerSet(int type, const DangerSetPtr& ds, DangerSetPtr& ods);

  IceUtil::RWRecMutex _validMutex;
  bool _valid;

  map<int, DangerSetPtr> _dangers;
  IceUtil::RWRecMutex _dangersMutex;
};

class UserDangerResultHandler : public com::xiaonei::xce::ResultHandler {
public:
  UserDangerResultHandler(int& largeid, std::vector<std::pair<int, int> >& data) : _largeid(largeid), _data(data) {}
protected:
  virtual bool handle(mysqlpp::Row& row) const;
private:
  int& _largeid;
  std::vector<std::pair<int, int> >& _data;
};

}
}

#endif
