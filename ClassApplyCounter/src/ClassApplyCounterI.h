#ifndef _CLASSAPPLYMANAGERI_H_
#define _CLASSAPPLYMANAGERI_H_

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include "ServiceI.h"
#include "ClassApplyCounter.h"

namespace com {
namespace xiaonei {
namespace service {
namespace classapplycounter {

const int ADMINED_CLASS = 0;
const int APPLIED_CLASS = 1;
const string CLASSAPPLY_COUNTER = "M";

class AdminedClass;
typedef IceUtil::Handle<AdminedClass> AdminedClassPtr;
class AppliedClass;
typedef IceUtil::Handle<AppliedClass> AppliedClassPtr;

class ClassApplyManagerI : virtual public ClassApplyManager,
		virtual public MyUtil::Singleton<ClassApplyManagerI> {
public:
	virtual ClassApplySeq getCounterSeq(::Ice::Int,
			const Ice::Current& = Ice::Current());

	virtual void sendApply(::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	virtual void acceptApply(::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	virtual void denyApply(::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	virtual void addAdmin(::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	virtual void removeAdmin(::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());
	
	virtual void reloadAdmin(::Ice::Int,
				const Ice::Current& = Ice::Current());
	
	virtual void reloadClass(::Ice::Int,
				const Ice::Current& = Ice::Current());

private:
	AdminedClassPtr findAdminedClass(Ice::Int userId);
	AdminedClassPtr locateAdminedClass(Ice::Int userId);
	AppliedClassPtr findAppliedClass(Ice::Int classId);
	AppliedClassPtr locateAppliedClass(Ice::Int classId);
};


class AdminedClassFactory : public MyUtil::ServantFactory {
public:
	AdminedClassFactory(string database) : _database(database) {};
	
	virtual Ice::ObjectPtr create(Ice::Int userId);
private:
	string _database;
};

class AppliedClassFactory : public MyUtil::ServantFactory {
public:
	AppliedClassFactory(string database) : _database(database) {};
	
	virtual Ice::ObjectPtr create(Ice::Int classId);
private:
	string _database;
};


class AdminedClass : public Ice::Object {
friend class AdminedClassFactory;
public:
	MyUtil::IntSeq getClassIds();
	void addClass(Ice::Int classId);
	void removeClass(Ice::Int classId);
private:
	MyUtil::IntSeq _classIds;
	IceUtil::Mutex _mutex;
};

class AppliedClass : public Ice::Object {
friend class AppliedClassFactory;
public:
	ClassApply getClassApply();
	void sendApply(Ice::Int applicant);
	void acceptApply(Ice::Int userId);
	void denyApply(Ice::Int userId);
private:

	int _classId;
	string _className;
	MyUtil::IntSeq _applicants;
	IceUtil::Mutex _mutex;
};

};
};
};
}
;

#endif /*CLASSAPPLYMANAGERI_H_*/
