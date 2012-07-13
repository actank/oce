#include "ClassApplyCounterI.h"

using namespace com::xiaonei::service::classapplycounter;
using namespace MyUtil;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	string database = props->getPropertyWithDefault("Service.Database.Class",
			"class");
	service.registerObjectCache(ADMINED_CLASS, "ADC", new AsyncServantFactory(new AdminedClassFactory(database)));
	service.registerObjectCache(APPLIED_CLASS, "APL",
			new AsyncServantFactory(new AppliedClassFactory(database)));
	service.getAdapter()->add(&ClassApplyManagerI::instance(), service.createIdentity(
			CLASSAPPLY_COUNTER, ""));
}

//---------------------------------------------------------------------------
ClassApplySeq ClassApplyManagerI::getCounterSeq(::Ice::Int userId,
		const Ice::Current&) {
	MyUtil::IntSeq ids = locateAdminedClass(userId)->getClassIds();
	ClassApplySeq result;
	for (MyUtil::IntSeq::iterator pos = ids.begin(); pos != ids.end(); ++pos) {
		try {
			ClassApply ca = locateAppliedClass(*pos)->getClassApply();
			if(ca.count != 0) {
				result.push_back(ca);
			}
		} catch (...) {

		}
	}
	return result;
}

void ClassApplyManagerI::sendApply(::Ice::Int applicant, ::Ice::Int classId,
		const Ice::Current&) {
	AppliedClassPtr ac = findAppliedClass(classId);
	if (ac) {
		ac->sendApply(applicant);
	}
}

void ClassApplyManagerI::acceptApply(::Ice::Int userId, ::Ice::Int classId,
		const Ice::Current&) {
	AppliedClassPtr ac = findAppliedClass(classId);
	if (ac) {
		ac->acceptApply(userId);
	}
}

void ClassApplyManagerI::denyApply(::Ice::Int userId, ::Ice::Int classId,
		const Ice::Current&) {
	AppliedClassPtr ac = findAppliedClass(classId);
	if (ac) {
		ac->denyApply(userId);
	}
}

void ClassApplyManagerI::addAdmin(::Ice::Int adminId, ::Ice::Int classId,
		const Ice::Current&) {
	AdminedClassPtr ac = findAdminedClass(adminId);
	if (ac) {
		ac->addClass(classId);
	}
}

void ClassApplyManagerI::removeAdmin(::Ice::Int adminId, ::Ice::Int classId,
		const Ice::Current&) {
	AdminedClassPtr ac = findAdminedClass(adminId);
	if (ac) {
		ac->removeClass(classId);
	}
}

AdminedClassPtr ClassApplyManagerI::findAdminedClass(Ice::Int userId) {
	return ServiceI::instance().findObject<AdminedClassPtr>(ADMINED_CLASS, userId);
}

AdminedClassPtr ClassApplyManagerI::locateAdminedClass(Ice::Int userId) {
	return ServiceI::instance().locateObject<AdminedClassPtr>(ADMINED_CLASS, userId);
}

AppliedClassPtr ClassApplyManagerI::findAppliedClass(Ice::Int classId) {
	return ServiceI::instance().findObject<AppliedClassPtr>(APPLIED_CLASS, classId);
}

AppliedClassPtr ClassApplyManagerI::locateAppliedClass(Ice::Int classId) {
	return ServiceI::instance().locateObject<AppliedClassPtr>(APPLIED_CLASS, classId);
}

void ClassApplyManagerI::reloadAdmin(Ice::Int adminId, const Ice::Current&) {
	MCE_DEBUG("ClassApplyManagerI::reloadAdmin: " << adminId);
	ServiceI::instance().removeObject(ADMINED_CLASS, adminId);
}

void ClassApplyManagerI::reloadClass(Ice::Int classId, const Ice::Current&) {
	MCE_DEBUG("ClassApplyManagerI::reloadClass: " << classId);
	ServiceI::instance().removeObject(APPLIED_CLASS, classId);
}

//---------------------------------------------------------------------------
MyUtil::IntSeq AdminedClass::getClassIds() {
	IceUtil::Mutex::Lock lock(this->_mutex);
	return _classIds;
}

void AdminedClass::addClass(Ice::Int classId) {
	IceUtil::Mutex::Lock lock(this->_mutex);
	if (find(_classIds.begin(), _classIds.end(), classId) == _classIds.end()) {
		_classIds.reserve(_classIds.size() + 1);
		_classIds.push_back(classId);
	}
}

void AdminedClass::removeClass(Ice::Int classId) {
	IceUtil::Mutex::Lock lock(this->_mutex);
	_classIds.erase(remove(_classIds.begin(), _classIds.end(), classId),
			_classIds.end());
	MyUtil::IntSeq helper(_classIds);
	_classIds.swap(helper);
}

//---------------------------------------------------------------------------
ClassApply AppliedClass::getClassApply() {
	IceUtil::Mutex::Lock lock(this->_mutex);
	ClassApply result;
	result.classId = _classId;
	result.className = _className;
	result.count = _applicants.size();
	return result;
}

void AppliedClass::sendApply(Ice::Int applicant) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (find(_applicants.begin(), _applicants.end(), applicant)
			== _applicants.end()) {
		_applicants.reserve(_applicants.size() + 1);
		_applicants.push_back(applicant);
	}
}

void AppliedClass::acceptApply(Ice::Int userId) {
	IceUtil::Mutex::Lock lock(_mutex);
	_applicants.erase(remove(_applicants.begin(), _applicants.end(), userId),
			_applicants.end());
	MyUtil::IntSeq helper(_applicants);
	_applicants.swap(helper);
}

void AppliedClass::denyApply(Ice::Int userId) {
	IceUtil::Mutex::Lock lock(_mutex);
	_applicants.erase(remove(_applicants.begin(), _applicants.end(), userId),
			_applicants.end());
	MyUtil::IntSeq helper(_applicants);
	_applicants.swap(helper);
}

//---------------------------------------------------------------------------
Ice::ObjectPtr AdminedClassFactory::create(Ice::Int userId) {
	MCE_DEBUG("AdminedClassFactory::create -> " << userId);
	ConnectionHolder conn(this->_database, CDbRServer);
	mysqlpp::Query query = conn.query();
	query << "select classid from classmember where memberid="<< userId
			<< " and (role=1 or role=-1);";
#ifndef NEWARCH
	mysqlpp::Result res = query.store();
	AdminedClassPtr result = new AdminedClass;
	for (size_t i = 0; i < res.size(); ++i) {
		result->_classIds.push_back(res.at(i)["classid"]);
	}
#else
	mysqlpp::StoreQueryResult res = query.store();
	AdminedClassPtr result = new AdminedClass;
	for (size_t i = 0; i < res.size(); ++i) {
		result->_classIds.push_back(res.at(i)["classid"]);
	}
#endif
	MCE_DEBUG("AdminedClassFactory::create -> " << userId << ", done");
	return result;
}

Ice::ObjectPtr AppliedClassFactory::create(Ice::Int classId) {
	MCE_DEBUG("AppliedClassFactory::create -> " << classId);
	ConnectionHolder conn(this->_database, CDbRServer);
	mysqlpp::Query query = conn.query();
	query << "select memberid from classmember where classid=" << classId
			<<" and role=10;";
#ifndef NEWARCH
	mysqlpp::Result res = query.store();
	AppliedClassPtr result = new AppliedClass;
	for (size_t i = 0; i < res.size(); ++i) {
		result->_applicants.push_back(res.at(i)["memberid"]);
	}
	result->_classId = classId;
	query << "select classname from schoolclass where id=" << classId << ";";
	res = query.store();
	if (!res.empty()) {		
		result->_className = res.at(0)["classname"].get_string();
	}
#else
	mysqlpp::StoreQueryResult res = query.store();
	AppliedClassPtr result = new AppliedClass;
	for (size_t i = 0; i < res.size(); ++i) {
		result->_applicants.push_back(res.at(i)["memberid"]);
	}
	result->_classId = classId;
	query << "select classname from schoolclass where id=" << classId << ";";
	res = query.store();
	if (!res.empty()) {		
		result->_className = res.at(0)["classname"].c_str();
	}
#endif
	MCE_DEBUG("AppliedClassFactory::create -> " << classId << ", done");
	return result;
}
