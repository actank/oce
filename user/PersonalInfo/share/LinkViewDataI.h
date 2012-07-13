#ifndef __XCE_PERSONALINFO_LINKVIEWDATA_H__
#define __XCE_PERSONALINFO_LINKVIEWDATA_H__

#include "PersonalInfo.h"
#include <Ice/Ice.h>
#include <TaskManager.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>

namespace xce {
namespace personalinfo {

//Cache Bucket Size
//--------------------------------------------------------------
static const int MAX_BUCKET = 1000000;

//DB
//--------------------------------------------------------------
const std::string DB_PERSONAL = "common";
const std::string TABLE_PERSONAL = "user_personalinfo";

//Data classes
//--------------------------------------------------------------
class LinkViewDataI;
typedef IceUtil::Handle<LinkViewDataI> LinkViewDataIPtr;
class LinkViewDataI : virtual public LinkViewData {
public:
  void setProps(const MyUtil::Str2StrMap& props);
  MyUtil::Str2StrMap getProps();
  void parse(mysqlpp::Row& row);
  void parse(Ice::ObjectPtr& obj);
private:
  IceUtil::Mutex _mutex;
};

class LinkDataI; 
typedef IceUtil::Handle<LinkDataI> LinkDataIPtr;
class LinkDataI : virtual public LinkData {
public:
  LinkDataI();
  LinkDataI(const LinkViewDataIPtr linkViewData);
  void setProps(const MyUtil::Str2StrMap& props);
  MyUtil::Str2StrMap getProps();
  void parse(mysqlpp::Row& row);
  void parse(Ice::ObjectPtr& obj);
private:
  IceUtil::Mutex _mutex;
};

class Link2UserIdDataI;
typedef IceUtil::Handle<Link2UserIdDataI> Link2UserIdDataIPtr;
class Link2UserIdDataI : virtual public Ice::Object {
public:
  int get(const string& link);
  void add(const string& link, Ice::Int userId);
  void remove(const string& link);
  int count(const string& link);
  int size();
private:
  MyUtil::Str2IntMap data;
  IceUtil::RWRecMutex _mutex;
};

//Factories
//--------------------------------------------------------------
class LinkViewDataFactoryI : public MyUtil::ServantFactory{
public:
  virtual Ice::ObjectPtr create(Ice::Int userId);
  virtual Ice::ObjectPtr create(Ice::Int userId, int wrflag);
};
typedef IceUtil::Handle<LinkViewDataFactoryI> LinkViewDataFactoryIPtr;

class LinkDataFactoryI : public MyUtil::ServantFactory{
public:
  virtual Ice::ObjectPtr create(Ice::Int userId);
};

class Link2UserIdDataFactoryI : public MyUtil::ServantFactory{
public:
  virtual Ice::ObjectPtr create(Ice::Int bucketId);
};

//ResultHandlers
//---------------------------------------------------------------
class LinkViewDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
  LinkViewDataResultHandlerI(const LinkViewDataIPtr& result);
  virtual bool handle(mysqlpp::Row&) const;
private:
  LinkViewDataIPtr _result;
};

class LinkDataResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
  LinkDataResultHandlerI(const LinkDataIPtr& result);
  virtual bool handle(mysqlpp::Row&) const;
private:
  LinkDataIPtr _result;
};

//Hash function from string to int
//---------------------------------------------------------------
class BucketTool {
public:
  static int getBucketId(const string& link);
};

}
}

#endif
