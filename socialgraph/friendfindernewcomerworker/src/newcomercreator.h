#ifndef __NEWCOMER_CREATOR_H__
#define __NEWCOMER_CREATOR_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "QueryRunner.h"
#include "newcomercachedataI.h"
#include "newcomerutil.h"

namespace xce {
namespace socialgraph {
namespace friendfindernewcomer {

//**************************************************************************************************************

class NewComerCreator: public MyUtil::Singleton<NewComerCreator>{
public:
	friend class MyUtil::Singleton<NewComerCreator>;
public:
	NewComerDbData createGuest(int,int,std::ostringstream&);
	MyUtil::Int2IntSeqMap findHosts(int,const MyUtil::IntSeq&);
	MyUtil::IntSeq filterHostFriends(int,const MyUtil::IntSeq&, const MyUtil::IntSeq&);
};

//**************************************************************************************************************

class NetworkCollector : public MyUtil::Singleton<NetworkCollector>{
public:
	friend class MyUtil::Singleton<NetworkCollector>;
public:
	NetworkCollector();
	vector< vector<string> > getUnivInfo(int userId);
	vector< vector<string> > getHighschoolInfo(int userId);
	vector< vector<string> > getJuniorschoolInfo(int userId);
	vector< vector<string> > getElementaryschoolInfo(int userId);
	vector< vector<string> > getWorkplaceInfo(int userId);
	int getUserAge(int userId);
	MyUtil::Int2IntMap getUserAgeBatch(const MyUtil::IntSeq& userIds);
};

//***********************************************************************************************

class UnivReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UnivReadHandler(std::vector< std::vector<string> >& univInfo): _univInfo(univInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _univInfo;
};

//***********************************************************************************************

class HighschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	HighschoolReadHandler(std::vector< std::vector<string> >& highschoolInfo): _highschoolInfo(highschoolInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _highschoolInfo;
};

//***********************************************************************************************

class JuniorschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	JuniorschoolReadHandler(std::vector< std::vector<string> >& juniorschoolInfo): _juniorschoolInfo(juniorschoolInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _juniorschoolInfo;
};

//***********************************************************************************************

class ElementaryschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	ElementaryschoolReadHandler(std::vector< std::vector<string> >& elementaryschoolInfo): _elementaryschoolInfo(elementaryschoolInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _elementaryschoolInfo;
};

//***********************************************************************************************

class WorkplaceReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	WorkplaceReadHandler(std::vector< std::vector<string> >& workplaceInfo): _workplaceInfo(workplaceInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _workplaceInfo;
};
	
//***********************************************************************************************

class UserAgeReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserAgeReadHandler(int& age): _age(age) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& _age;
};

//***********************************************************************************************

class UserAgeBatchReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserAgeBatchReadHandler(MyUtil::Int2IntMap& userAgeMap): _userAgeMap(userAgeMap) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	MyUtil::Int2IntMap& _userAgeMap;
};

}
}
}

#endif
