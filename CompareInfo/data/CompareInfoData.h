#ifndef __COMPARE_INFO_DATA_H__
#define __COMPARE_INFO_DATA_H__

#include "CompareInfo.h"
#include <QueryRunner.h>
#include "util/cpp/MD5Util.h"
#include <Ice/Ice.h>

namespace xce
{
namespace compareinfo
{
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;

const string TABLE_NETWORK ="network_history";
const string TABLE_HIGH_SCHOOL_INFO = "high_school_info";
const string TABLE_UNIVERSITY_INFO = "university_info";
const string TABLE_WORKPLACE_INFO = "workplace_info";
const string TABLE_COLLEGE_INFO = "college_info";
const string TABLE_ELEMENTARY_SCHOOL_INFO = "elementary_school_info";
const string TABLE_JUNIOR_HIGH_SCHOOL_INFO = "junior_high_school_info";


const std::string DB_CMPAREINFO_TABLE_PASSPORT = "compareinfo_user_passport";
const std::string DB_CMPAREINFO_TABLE_NETWORK = "compareinfo_network_history";
const std::string DB_CMPAREINFO_TABLE_HIGH_SCHOOL_INFO = "compareinfo_high_school_info";
const std::string DB_CMPAREINFO_TABLE_UNIVERSITY_INFO = "compareinfo_university_info";
const std::string DB_CMPAREINFO_TABLE_WORKPLACE_INFO = "compareinfo_workplace_info";
const std::string DB_CMPAREINFO_TABLE_COLLEGE_INFO = "compareinfo_college_info";
const std::string DB_CMPAREINFO_TABLE_ELEMENTARY_SCHOOL_INFO = "compareinfo_elementary_school_info";
const std::string DB_CMPAREINFO_TABLE_JUNIOR_HIGH_SCHOOL_INFO = "compareinfo_junior_high_school_info";


class CompareInfoDataI;
typedef IceUtil::Handle<CompareInfoDataI>  CompareInfoDataIPtr;
class CompareInfoDataI : virtual public CompareInfoData
{
public:
    CompareInfoDataI(){}
    CompareInfoDataI(const CompareInfoDataIPtr& data)
    {
        this->userId = data->userId;
        this->networkName = data->networkName;
        this->highschoolName = data->highschoolName;
        this->universityName = data->universityName;
        this->workplaceName = data->workplaceName;
        this->collegeName = data->collegeName;
        this->elementarySchoolName = data->elementarySchoolName;
        this->juniorHighShoolName = data->juniorHighShoolName;
/*
  
        for (std::vector<long>::size_type i = 0; i != data->getnetworkName().size(); i ++)
        {
            this->addnetworkName(data->getnetworkName().at(i));
        }  
        for (std::vector<long>::size_type i = 0; i != data->gethightschoolName().size(); i ++)
        {
            this->addhightschoolName(data->gethightschoolName().at(i));
        }
        for (std::vector<long>::size_type i = 0; i != data->getuniversityName().size(); i ++)
        {
            this->adduniversityName(data->getuniversityName().at(i));
        }
        for (std::vector<long>::size_type i = 0; i != data->getworkplaceName().size(); i ++)
        {
            this->addworkplaceName(data->getworkplaceName().at(i));
        }
        for (std::vector<long>::size_type i = 0; i != data->getcollegeName().size(); i ++)
        {
            this->addcollegeName(data->getcollegeName().at(i));
        }              
*/ 
    }
    
    void parse (Ice::ObjectPtr& obj)
    {
        CompareInfoDataPtr data = CompareInfoDataPtr::dynamicCast(obj);
       
        this->userId = data->userId;
        this->networkName = data->networkName;
        this->highschoolName = data->highschoolName;
        this->universityName = data->universityName;
        this->workplaceName = data->workplaceName;
        this->collegeName = data->collegeName;
        this->elementarySchoolName = data->elementarySchoolName;
        this->juniorHighShoolName = data->juniorHighShoolName;
    }
 

    //long universityName() const {return }
    int getuserId() const {return userId;}
    std::vector<long> getnetworkName() const {return networkName;}
    std::vector<long> gethightschoolName() const {return highschoolName;}
    std::vector<long> getuniversityName() const {return universityName;}
    std::vector<long> getworkplaceName() const {return workplaceName;}
    std::vector<long> getcollegeName() const {return collegeName;}
    long getelementarySchoolName() const {return elementarySchoolName;};
    long getjuniorHighSchoolName() const {return juniorHighShoolName;};


    void clearnetworkName(){ networkName.clear(); }
    void clearhightschoolName(){ highschoolName.clear(); }
    void clearuniversityName() { universityName.clear(); }
    void clearworkplaceName(){ workplaceName.clear();}
    void clearcollegeName(){collegeName.clear(); }

    void setuserId(int userId)   {this->userId = userId;}
    void addnetworkName(long workName)   {this->networkName.push_back(workName);}
    void addhightschoolName(long highschoolName)   {this->highschoolName.push_back(highschoolName);}
    void adduniversityName(long universityName)   {this->universityName.push_back(universityName);}
    void addworkplaceName(long workplaceName)   {this->workplaceName.push_back(workplaceName);}
    void addcollegeName(long collageName)   {this->collegeName.push_back(collageName);}
    void setelementaryschoolName(long elementSchoolName)   {this->elementarySchoolName = elementSchoolName;}
    void setjuniorhighschoolName(long juniorHighSchoolName)   {this->juniorHighShoolName = juniorHighSchoolName;}
    
};

class BatchUsersIdProducerI : public com::xiaonei::xce::ResultHandler
{
public:
    BatchUsersIdProducerI (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};



class BatchNetworkResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchNetworkResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};

class BatchHighSchoolResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchHighSchoolResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};

class BatchUniversityResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchUniversityResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};

class BatchCollegeResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchCollegeResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};

class BatchWorkplaceResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchWorkplaceResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};


class BatchElementarySchoolResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchElementarySchoolResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};

class BatchJuniorHighSchoolResultHandler : public com::xiaonei::xce::ResultHandler
{
public:
    BatchJuniorHighSchoolResultHandler (const MyUtil::ObjectResultPtr& results) : _results(results){};
    virtual bool handle(mysqlpp::Row& row) const;
private:
    MyUtil::ObjectResultPtr _results;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BatchUsersIdProducerI::handle(mysqlpp::Row& row) const
{

    int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {
        CompareInfoDataIPtr newData = new CompareInfoDataI;
        newData->setuserId( id );
        _results->data.insert(make_pair(id, newData));
    } else
    {
        MCE_WARN("there are many data in user_passport" << __FILE__ << " " << __LINE__);
    }
    return true;

}



//把Network表中查取结果放大自定义数据对象CompareDataI中
bool BatchNetworkResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
    #ifndef NEWARCH
        CompareInfoDataIPtr::dynamicCast(it->second)->addnetworkName( MD5Util::stringToMD5 ( row["network_name"].get_string()) );
    #else
        CompareInfoDataIPtr::dynamicCast(it->second)->addnetworkName( MD5Util::stringToMD5 ( row["network_name"].data()) );
    #endif
    }
   return true;
}
                                   

//把HighSchool表中查取结果放大自定义数据对象CompareDataI中
bool BatchHighSchoolResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
    #ifndef NEWARCH
        CompareInfoDataIPtr::dynamicCast(it->second)->addhightschoolName( MD5Util::stringToMD5 ( row["high_school_name"].get_string()) );
    #else
        CompareInfoDataIPtr::dynamicCast(it->second)->addhightschoolName( MD5Util::stringToMD5 ( row["high_school_name"].data()) );
    #endif
    }
   return true;

}

//把Unive_resultsity表中查取结果放大自定义数据对象CompareDataI中
bool BatchUniversityResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
    #ifndef NEWARCH
        CompareInfoDataIPtr::dynamicCast(it->second)->adduniversityName( MD5Util::stringToMD5 ( row["university_name"].get_string()) );
    #else
        CompareInfoDataIPtr::dynamicCast(it->second)->adduniversityName( MD5Util::stringToMD5 ( row["university_name"].data()) );
    #endif
    }
   return true;
}


//把Workplace表中查取结果放大自定义数据对象CompareDataI中
bool BatchWorkplaceResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
    #ifndef NEWARCH
       CompareInfoDataIPtr::dynamicCast(it->second)->addworkplaceName( MD5Util::stringToMD5 ( row["workplace_name"].get_string()) );
    #else
       CompareInfoDataIPtr::dynamicCast(it->second)->addworkplaceName( MD5Util::stringToMD5 ( row["workplace_name"].data()) );
    #endif
    }
   return true;
}


//把CollegeResultHandler表中查取结果放大自定义数据对象CompareDataI中
bool BatchCollegeResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
    #ifndef NEWARCH
       CompareInfoDataIPtr::dynamicCast(it->second)->addcollegeName( MD5Util::stringToMD5 ( row["college_name"].get_string()) );
    #else
       CompareInfoDataIPtr::dynamicCast(it->second)->addcollegeName( MD5Util::stringToMD5 ( row["college_name"].data()) );
    #endif
    }
   return true;
}

//把ElementarySchool表中查取结果放大自定义数据对象CompareDataI中
bool BatchElementarySchoolResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
#ifndef NEWARCH
    CompareInfoDataIPtr::dynamicCast(it->second)->setelementaryschoolName( MD5Util::stringToMD5 ( row["elementary_school_name"].get_string()) );
#else
    CompareInfoDataIPtr::dynamicCast(it->second)->setelementaryschoolName( MD5Util::stringToMD5 ( row["elementary_school_name"].data()) );
#endif
   }
   return true;

}

//把ElementarySchool表中查取结果放大自定义数据对象CompareDataI中
bool BatchJuniorHighSchoolResultHandler::handle(mysqlpp::Row& row) const
{
    int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
    {}
    else
    {
    #ifndef NEWARCH
        CompareInfoDataIPtr::dynamicCast(it->second)->setjuniorhighschoolName( MD5Util::stringToMD5 ( row["junior_high_school_name"].get_string()) );
    #else
        CompareInfoDataIPtr::dynamicCast(it->second)->setjuniorhighschoolName( MD5Util::stringToMD5 ( row["junior_high_school_name"].data()) );
    #endif
    }
    return true;

}


};
};
#endif

