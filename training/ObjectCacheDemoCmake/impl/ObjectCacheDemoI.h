#ifndef _OBJECTCACHEDEMOI_
#define _OBJECTCACHEDEMOI_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include <sstream>
//#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "ObjectCacheDemo.h"
#include "UtilCxx/src/LogWrapper.h"

using namespace xce::demos;
using namespace std;
using namespace MyUtil;

const static int UE = 0;

//ObjectCacheDemo
//-------------------------------------------------------------
class ObjectCacheDemoI : public ObjectCacheDemo{
public:
//	ObjectCacheDemoI(const Ice::ObjectAdapterPtr& adapter);
	ObjectCacheDemoI(const Ice::ObjectAdapterPtr& adapter, int capacity);
	virtual void load(const Ice::Current&);
	virtual void preload(const Ice::Current&);
	virtual void reload(const Ice::Current&);
	virtual int size(const Ice::Current&);
	virtual string getEmail(long id, const Ice::Current&);
	virtual string touchEmail(long id, const Ice::Current&);
	virtual void setEmail(long id, const string& email, const Ice::Current&);
	void addObject(long id);
	void addObject(LongSeq& ids);
	void addObject(long id, long range);
	MyUtil::ObjectCacheIPtr& getCache();

protected:
	//调用TaskManager的例子
	class LoadTask: public MyUtil::Task {
		public:
			LoadTask(Ice::Long id, ObjectCacheDemoI& ocd) :
				MyUtil::Task(TASK_LEVEL_PRELOAD), _ocd(ocd) {
				_ids.push_back(id);
			}
			LoadTask(const MyUtil::LongSeq ids, ObjectCacheDemoI& ocd) :
				MyUtil::Task(TASK_LEVEL_PRELOAD), _ids(ids), _ocd(ocd) {
			}
			virtual void handle() {
				_ocd.addObject(_ids);
			}
		private:
			MyUtil::LongSeq _ids;
			ObjectCacheDemoI& _ocd;
		};

private:
	MyUtil::ObjectCacheIPtr _cache;
	Ice::CommunicatorPtr _ic;
};
typedef IceUtil::Handle<ObjectCacheDemoI> ObjectCacheDemoIPtr;

//Object (UserEmail)
//-------------------------------------------------------------
class UserEmail : virtual public Ice::Object{
private:
	long _id;
	string _email;
public:
	UserEmail(long id):_id(id){
		stringstream ss;
		ss << _id << "@renren.com";
		_email = string(ss.str());
	}
	string mail(){return _email;}
	long id(){return _id;}
	void mail(string email){
		_email = string(email);
	}
	void id(int id){
		_id = id;
	}
};
typedef IceUtil::Handle<UserEmail> UserEmailPtr;

//FactoryI
//-------------------------------------------------------------
class DomoCacheFactoryI: public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long id);
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
};
#endif
