#include "UseredClassI.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "ClassAdapter.h"
#include "MainObjectCacheAdapter.h"
//#include "IrcdNotifyAdapter.h"
using namespace mop::hi::svc::model;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace std;

extern "C"
{

//
// Factory function
//
USERED_CLASS_API IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return &UseredClassServiceI::instance();
}

}

//---------------------------------------------------------------------------
void
UseredClassServiceI::initialize()
{
    addServantCache<Ice::Int>(CLASS_SET, "U", new SyncServantFactory<Ice::Int>(new ServantFactoryI));



    getAdapter()->add(new UseredClassManagerI,
            createIdentity(USERED_CLASS_MANAGER,""));

    ClassAdapter::instance().initializeWith(getCommunicator());
  //  IrcdNotifyAdapter::instance().initializeWith(getCommunicator());
    MainObjectCacheAdapter::instance().initializeWith(getCommunicator());
	ConnectionPoolManager::instance().initialize(getAdapter());
}


UseredClassSetPtr
UseredClassServiceI::getClassSet(Ice::Int userId)
{
    return UseredClassSetPtr::dynamicCast(findObject(CLASS_SET, userId));
}


//---------------------------------------------------------------------------
Ice::ObjectPtr
ServantFactoryI::create(const Ice::Int& id)
{
    MCE_DEBUG("Loading object: " << id);
	UseredClassSetPtr res= new UseredClassSetI();
    ostringstream sql;
    sql << "select * from classmember where memberid = "<<id
        <<" and  role <> " << APPLYTYPE;

	ConnectionQuery conn("class",CDbRServer);
	
    ResSetPtr rs = conn.doSelect( sql.str());
	DefResSetPtr dr = DefResSetPtr::dynamicCast(rs);


    if(dr->empty()) {
    	return res;
    }

    for(int i = 0 ; i < dr->size(); ++ i)
    {

        res->add((int)dr->getCol(i,"SCHOOLFLAG"),
                 (int)dr->getCol(i,"CLASSID"),
                 (int)dr->getCol(i,"ROLE"));
    }


    MCE_DEBUG("Loading object: " << id << " done.");
    return res;
}

//---------------------------------------------------------------------------
Ice::Int
mop::hi::svc::model::UseredClassManagerI::getAllClassCount(Ice::Int userId,
        Ice::Int schoolType,
        const Ice::Current& current)
{
    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->size(schoolType,MyUtil::IntSeq());
}

Ice::Int
mop::hi::svc::model::UseredClassManagerI::getAdminedClassCount(Ice::Int userId,
        Ice::Int schoolType,
        const Ice::Current& current)
{
    MyUtil::IntSeq roles ;
    roles.push_back(ADMINTYPE);
    roles.push_back(FOUNDERTYPE);

    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->size(schoolType,roles);

}

Ice::Int
mop::hi::svc::model::UseredClassManagerI::getMemberedClassCount(Ice::Int userId,
        Ice::Int schoolType,
        const Ice::Current& current)
{
    MyUtil::IntSeq roles ;
    roles.push_back(ADMINTYPE);
    roles.push_back(FOUNDERTYPE);
    roles.push_back(MEMBERTYPE);
    roles.push_back(BLOCKTYPE);

    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->size(schoolType,roles);
}

Ice::Int
mop::hi::svc::model::UseredClassManagerI::getFoundedClassCount(Ice::Int userId,
        Ice::Int schoolType,
        const Ice::Current& current)
{
    MyUtil::IntSeq roles ;

    roles.push_back(FOUNDERTYPE);

    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->size(schoolType,roles);
}

MyUtil::IntSeq
mop::hi::svc::model::UseredClassManagerI::getAllClasses(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current& current)
{

    
    UseredClassServiceI& service = UseredClassServiceI::instance();
    if(schoolType >=0)
    {
        return service.getClassSet(userId)->getAll(schoolType,MyUtil::IntSeq(),offset,limit);
    }else
    {
        MyUtil::IntSeq tmp1 = service.getClassSet(userId)->getAll(0,MyUtil::IntSeq(),offset,limit);
        MyUtil::IntSeq tmp2 = service.getClassSet(userId)->getAll(1,MyUtil::IntSeq(),offset,limit);
        tmp1.insert(tmp1.begin(),tmp2.begin(), tmp2.end());
        return tmp1;
    }
}

MyUtil::IntSeq
mop::hi::svc::model::UseredClassManagerI::getAdminedClasses(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current& current)
{
    MyUtil::IntSeq roles ;
    roles.push_back(ADMINTYPE);
    roles.push_back(FOUNDERTYPE);
    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->getAll(schoolType,roles,offset,limit);
}

MyUtil::IntSeq
mop::hi::svc::model::UseredClassManagerI::getMemberedClasses(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current& current)
{
    MyUtil::IntSeq roles ;
    roles.push_back(ADMINTYPE);
    roles.push_back(FOUNDERTYPE);
    roles.push_back(MEMBERTYPE);
    roles.push_back(BLOCKTYPE);
    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->getAll(schoolType,roles,offset,limit);
}

MyUtil::IntSeq
mop::hi::svc::model::UseredClassManagerI::getFoundedClasses(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current& current)
{
    MyUtil::IntSeq roles ;

    roles.push_back(FOUNDERTYPE);
    UseredClassServiceI& service = UseredClassServiceI::instance();
    return service.getClassSet(userId)->getAll(schoolType,roles,offset,limit);
}


MyUtil::Str2StrMapSeq
mop::hi::svc::model::UseredClassManagerI::getAllClassInfo(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current&)
{
    MyUtil::IntSeq ids = getAllClasses(userId,schoolType,offset,limit);
    MyUtil::Str2StrMapSeq res;
    for(int i = 0 ;i<ids.size();++i)
    {
        res.push_back(getClassInfo(ids.at(i)));
    }
    return res;
}

MyUtil::Str2StrMapSeq
mop::hi::svc::model::UseredClassManagerI::getAdminedClassInfo(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current&)
{
    MyUtil::IntSeq ids = getAdminedClasses(userId,schoolType,offset,limit);
    MyUtil::Str2StrMapSeq res;
    for(int i = 0 ;i<ids.size();++i)
    {
        res.push_back(getClassInfo(ids.at(i)));
    }
    return res;
}

MyUtil::Str2StrMapSeq
mop::hi::svc::model::UseredClassManagerI::getMemberedClassInfo(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current&)
{
    MyUtil::IntSeq ids = getMemberedClasses(userId,schoolType,offset,limit);
    MyUtil::Str2StrMapSeq res;
    for(int i = 0 ;i<ids.size();++i)
    {
        res.push_back(getClassInfo(ids.at(i)));
    }
    return res;
}

MyUtil::Str2StrMapSeq
mop::hi::svc::model::UseredClassManagerI::getFoundedClassInfo(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int offset,
        Ice::Int limit,
        const Ice::Current&)
{
    MyUtil::IntSeq ids = getFoundedClasses(userId,schoolType,offset,limit);
    MyUtil::Str2StrMapSeq res;
    for(int i = 0 ;i<ids.size();++i)
    {
        res.push_back(getClassInfo(ids.at(i)));
    }
    return res;
}



MyUtil::IntSeq
mop::hi::svc::model::UseredClassManagerI::getDispatchClasses(Ice::Int userId,
        const Ice::Current& current)
{
    MCE_DEBUG("--------call getDispatchClasses ---------");
    UseredClassServiceI& service = UseredClassServiceI::instance();
    MCE_DEBUG("--------call getDispatchClasses 1---------");
    MyUtil::IntSeq res = service.getClassSet(userId)->getAll(0,MyUtil::IntSeq(),0,-1);
    MCE_DEBUG("--------call getDispatchClasses 2---------"<<res.size());
    MyUtil::IntSeq tmp = service.getClassSet(userId)->getAll(1,MyUtil::IntSeq(),0,-1);
    MCE_DEBUG("--------call getDispatchClasses 3---------"<<tmp.size());
    if(tmp.size()){
        res.insert(res.end(),tmp.begin(),tmp.end());
    }
    return res;
}


void
mop::hi::svc::model::UseredClassManagerI::addMemberedClass(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int classId,
        const Ice::Current&)
{
	if (UseredClassServiceI::instance().hasObject(CLASS_SET, userId)) {
    	UseredClassServiceI::instance().getClassSet(userId)->add(schoolType,classId,MEMBERTYPE);
    }
    MainObjectCacheAdapter::instance().ClassMemChangeNotify(classId);
    MainObjectCacheAdapter::instance().MyClassChangeNotify(userId);
//    IrcdNotifyAdapter::instance().ClassMemberChangeNotify(classId,userId,1);
}

void
mop::hi::svc::model::UseredClassManagerI::removeMemberedClass(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int classId,
        const Ice::Current&)
{
	if (UseredClassServiceI::instance().hasObject(CLASS_SET, userId)) {
    	UseredClassServiceI::instance().getClassSet(userId)->remove(schoolType,classId);
    }
    MainObjectCacheAdapter::instance().ClassMemChangeNotify(classId);
    MainObjectCacheAdapter::instance().MyClassChangeNotify(userId);
  //  IrcdNotifyAdapter::instance().ClassMemberChangeNotify(classId,userId,0);
}

void
mop::hi::svc::model::UseredClassManagerI::addAdminedClass(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int classId,
        const Ice::Current&)
{
	if (UseredClassServiceI::instance().hasObject(CLASS_SET, userId)) {
    	UseredClassServiceI::instance().getClassSet(userId)->add(schoolType,classId,ADMINTYPE);
    }
    MainObjectCacheAdapter::instance().ClassMemChangeNotify(classId);
    MainObjectCacheAdapter::instance().MyClassChangeNotify(userId);
}

void
mop::hi::svc::model::UseredClassManagerI::removeAdminedClass(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int classId,
        const Ice::Current&)
{
	if (UseredClassServiceI::instance().hasObject(CLASS_SET, userId)) {
    	UseredClassServiceI::instance().getClassSet(userId)->remove(schoolType,classId);
    }
    MainObjectCacheAdapter::instance().ClassMemChangeNotify(classId);
    MainObjectCacheAdapter::instance().MyClassChangeNotify(userId);
    //IrcdNotifyAdapter::instance().ClassMemberChangeNotify(classId,userId,0);
}


void
mop::hi::svc::model::UseredClassManagerI::addFoundedClass(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int classId,
        const Ice::Current&)
{
    MCE_DEBUG("addFoundedClass")
    if (UseredClassServiceI::instance().hasObject(CLASS_SET, userId)) {
    	UseredClassServiceI::instance().getClassSet(userId)->add(schoolType,classId,FOUNDERTYPE);
    }
    MainObjectCacheAdapter::instance().ClassMemChangeNotify(classId);
    MainObjectCacheAdapter::instance().MyClassChangeNotify(userId);
    //IrcdNotifyAdapter::instance().ClassMemberChangeNotify(classId,userId,1);
}

void
mop::hi::svc::model::UseredClassManagerI::removeFoundedClass(Ice::Int userId,
        Ice::Int schoolType,
        Ice::Int classId,
        const Ice::Current&)
{
	if (UseredClassServiceI::instance().hasObject(CLASS_SET, userId)) {
    	UseredClassServiceI::instance().getClassSet(userId)->remove(schoolType,classId);
    }
    MainObjectCacheAdapter::instance().ClassMemChangeNotify(classId);
    MainObjectCacheAdapter::instance().MyClassChangeNotify(userId);
   // IrcdNotifyAdapter::instance().ClassMemberChangeNotify(classId,userId,0);
}

/*
void
mop::hi::svc::model::UseredClassManagerI::setClassInfo(Ice::Int classId, const MyUtil::Str2StrMap & props, const Ice::Current&)
{

    IceUtil::Mutex::Lock lock(*this);
    try{
        MyUtil::Str2StrMap info = classInfo.locate(classId);

        for(MyUtil::Str2StrMap::const_iterator it = props.begin() ; it != props.end(); ++ it)
        {
            info[it->first] = it ->second;
        }

        //classInfo.remove(classId);
        //classInfo.add(info, classId);

    }catch(Ice::ObjectNotExistException& e)
    {

    }
}*/

MyUtil::Str2StrMap
mop::hi::svc::model::UseredClassManagerI::getClassInfo(Ice::Int classId, const Ice::Current&)
{

    /*IceUtil::Mutex::Lock lock(*this);
    try{
        return classInfo.locate(classId);

    }catch(Ice::ObjectNotExistException& e)
    {*/
        Str2StrMap info = ClassAdapter::instance().getSchoolClass(classId)->getProperties();
        //classInfo.add(info,classId);
        return info;
    //}
}

/*
void
mop::hi::svc::model::UseredClassManagerI::removeClassInfo(Ice::Int classId, const Ice::Current&)
{
    IceUtil::Mutex::Lock lock(*this);
    try{
        classInfo.remove(classId);
    }catch(...)
    {
    }
}*/

//---------------------------------------------------------------------------
Ice::Int
mop::hi::svc::model::UseredClassSetI::size(Ice::Int schoolType, const MyUtil::IntSeq& roles, const Ice::Current& current)
{
    IceUtil::Mutex::Lock lock(*this);
	Ice::Int result = 0;
    MyUtil::Int2IntMap& classIds = schoolType == 0 ? highschoolIds : univIds;
    for (Int2IntMap::const_iterator it = classIds.begin(); it != classIds.end(); ++it) {
    	if (roles.empty() ||
    		find(roles.begin(), roles.end(), it->second) != roles.end()) {
    		++result;
    	}
    }

    return result;
}

void
mop::hi::svc::model::UseredClassSetI::add(Ice::Int schoolType,Ice::Int classId, Ice::Int role, const Ice::Current& current)
{

    MCE_DEBUG("-----------all add --------------");
    IceUtil::Mutex::Lock lock(*this);
    if(schoolType == 0)
    {
        highschoolIds[classId] = role;
    }else{
        univIds[classId] = role;
    }

}

void
mop::hi::svc::model::UseredClassSetI::remove(Ice::Int schoolType, Ice::Int classId, const Ice::Current& current)
{

    MCE_DEBUG("UseredClassSet remove  schoolType="<< schoolType <<"  classid= "<<classId);
    IceUtil::Mutex::Lock lock(*this);
    if(schoolType == 0)
    {
        highschoolIds.erase(classId);
    }else{
        univIds.erase(classId);
    }
}

MyUtil::IntSeq
mop::hi::svc::model::UseredClassSetI::getAll(Ice::Int schoolType,
        const MyUtil::IntSeq &  roles,
        Ice::Int begin,
        Ice::Int limit,
        const Ice::Current& current)
{
    IceUtil::Mutex::Lock lock(*this);
    MCE_DEBUG("--------call getall-----------");

    MyUtil::IntSeq seq = match(schoolType, roles);
    MCE_DEBUG("--------seq--------------"<<seq.size());

    IntSeq result;
    for(IntSeq::iterator it = seq.begin();
            it != seq.end();
            ++it)
    {
        if(begin-- > 0)	continue;
        result.push_back(*it);
        if(limit > 0 && result.size() >= limit) break;
    }
    return result;
}


MyUtil::IntSeq
mop::hi::svc::model::UseredClassSetI::match(
    Ice::Int schoolType,
    const MyUtil::IntSeq& roles)
{
    MyUtil::IntSeq res;
    MyUtil::Int2IntMap& classIds = schoolType == 0 ? highschoolIds : univIds;
    for (Int2IntMap::const_iterator it = classIds.begin(); it != classIds.end(); ++it) {
    	if (roles.empty() ||
    		find(roles.begin(), roles.end(), it->second) != roles.end()) {
    		res.push_back(it->first);
    	}
    }

    return res;
}

//---------------------------------------------------------------------------
