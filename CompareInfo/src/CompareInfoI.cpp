#include "CompareInfoI.h"
#include <boost/lexical_cast.hpp>
#include "ServiceI.h"
#include "QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using namespace ::xce::compareinfo;
using namespace MyUtil;
using namespace ::xce::serverstate;

void
MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(COMPAREINFO_DATA, "N", new CompareInfoDataFactoryI, false);
	service.getAdapter()->add(&CompareInfoManagerI::instance(), service.createIdentity("M",""));    
        TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));	
        TaskManager::instance().config(TASK_LEVEL_UPDATE, MyUtil::ThreadPoolConfig(1, 10));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CompareInfo.Mod",0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CompareInfo.Interval",5);
	ServerStateSubscriber::instance().initialize("ControllerCompareInfo",&CompareInfoManagerI::instance(),mod,interval);
}


//--------------------------------------------------------------------------------

Ice::ObjectPtr CompareInfoDataFactoryI::create(Ice::Int userId) {
	MCE_DEBUG("CompareInfoDataFactoryI::create userid = "<< userId);
	MyUtil::IntSeq ids;
	ids.push_back(userId);
	CompareInfoReloaderAdapter::instance().getProxy(0)->reload(ids);	
        return NULL;

}

void CompareInfoManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) 
{
    ostringstream oss;
    oss << "CompareInfoManagerI::setData -> size: "<<userData->data.size()<< " " << current.con->toString().substr(current.con->toString().find("remote",25));
    TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_DEBUG);
    TaskManager::instance().execute(new FillTask(userData));
}

void CompareInfoManagerI::update(const CompareInfoUpdateNewDataPtr& data, const Ice::Current& current) {
	MCE_INFO("CompareInfoManagerI::update -> userid: "<<data->userId << " " <<current.con->toString().substr(current.con->toString().find("remote",25)));
	TaskManager::instance().execute(new UpdateTaskI(data, current));
}


void CompareInfoManagerI::setValid(bool valid, const Ice::Current& current) 
{
    ostringstream oss;
    oss << "CompareInfoManagerI::setValid -> "<<valid<< " " << current.con->toString().substr(current.con->toString().find("remote",25));
    TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
    IceUtil::RWRecMutex::WLock lock(_validMutex);
    _valid = valid;
    ServerStateSubscriber::instance().setStatus((int)valid);
}

bool CompareInfoManagerI::isValid( const Ice::Current& current) 
{
    IceUtil::RWRecMutex::RLock lock(_validMutex);
    return _valid;
}

bool CompareInfoManagerI::CompareInfo(int userId1, int userId2, const Ice::Current& current)
{
    try{
	    CompareInfoDataIPtr  newData1= ServiceI::instance().locateObject<CompareInfoDataIPtr>(COMPAREINFO_DATA, userId1); 
	    CompareInfoDataIPtr  newData2 = ServiceI::instance().locateObject<CompareInfoDataIPtr>(COMPAREINFO_DATA, userId2);

	    if( _compare_user_vector(newData1->getnetworkName() , newData2->getnetworkName()))  return true;
	    else if(_compare_user_vector(newData1->gethightschoolName() , newData2->gethightschoolName()))return true;
	    else if(_compare_user_vector(newData1->getuniversityName() , newData2->getuniversityName()))return true;
	    else if(_compare_user_vector(newData1->getworkplaceName() , newData2->getworkplaceName()))return true;
	    else if(_compare_user_vector(newData1->getcollegeName() , newData2->getcollegeName()))return true;
	    else if(newData1->getelementarySchoolName() == newData2->getelementarySchoolName())return true;
	    else if(newData1->getjuniorHighSchoolName() == newData2->getjuniorHighSchoolName())return true;
	    else return false;

    }catch(const std::exception& e){
          MCE_WARN("compareinfo() " << e.what());
    }catch (...) {
          MCE_WARN("compareinfo -> unknown exception");
    }
    return false;
}

bool CompareInfoManagerI::CompareInfoByTable(int userId1, int userId2, const ::xce::compareinfo::byteSeq& table, const Ice::Current& current)
{
    try{
	    CompareInfoDataIPtr  newData1= ServiceI::instance().locateObject<CompareInfoDataIPtr>(COMPAREINFO_DATA, userId1); 
	    CompareInfoDataIPtr  newData2 = ServiceI::instance().locateObject<CompareInfoDataIPtr>(COMPAREINFO_DATA, userId2);
	    
	    for ( MyUtil::ByteSeq::const_iterator itr = table.begin(); itr != table.end(); ++itr) 
	    {
		    switch (*itr)
		    {
			    case NETWORK:
                                    if( _compare_user_vector(newData1->getnetworkName() , newData2->getnetworkName()))  return true;
		                    break;
			    case HIGHTSCHOOL:
				    if(_compare_user_vector(newData1->gethightschoolName() , newData2->gethightschoolName()))return true;
                                    break;
			    case UNIVERSITY:
                                    if(_compare_user_vector(newData1->getuniversityName() , newData2->getuniversityName()))return true;
				    break;
			    case COLLEGE:
				    if(_compare_user_vector(newData1->getcollegeName() , newData2->getcollegeName()))return true;
				    break;
			    case WORKPLACE:
                                    if(_compare_user_vector(newData1->getworkplaceName() , newData2->getworkplaceName()))return true;
				    break;
			    case ELEMENTARYSCHOOL:
                  		    if(newData1->getelementarySchoolName() == newData2->getelementarySchoolName())return true;
 				    break;
			    case JUNIORHIGHTSCHOOL:
                                    if(newData1->getjuniorHighSchoolName() == newData2->getjuniorHighSchoolName())return true;
				    break;
			    default:
				    MCE_WARN("Compareinfo -> Invalid table: "<< *itr);
		    }
	    }


    }catch(const std::exception& e){
          MCE_WARN("compareinfo2() " << e.what());
    }catch (...) {
          MCE_WARN("compareinfo2() -> unknown exception");
    }
    return false;
}


bool CompareInfoManagerI::_compare_user_vector(std::vector<long>  vector1,std::vector<long>  vector2)
{
    
    for(vector<long>::iterator it1 = vector1.begin() ; it1!= vector1.end(); ++it1)
    {
        for(vector<long>::iterator it2 = vector2.begin() ; it2!= vector2.end(); ++it2)
        {
            if(*it1 == *it2) { return true;}
        }  
    }
    return false;
}

void FillTask::handle() 
{
    ObjectResultPtr results = new ObjectResult;
    for(MyUtil::ObjectMap::iterator it = _userData->data.begin();it != _userData->data.end();++it) 
    {  
         CompareInfoDataIPtr obj = new CompareInfoDataI;
         obj->parse(it->second);
         results->data.insert(make_pair(obj->userId,obj));
    }
    MyUtil::ServiceI::instance().getObjectCache()->addObjects(COMPAREINFO_DATA , results->data); 
}


void UpdateTaskI::handle()
{
    CompareInfoDataIPtr olddata = ServiceI::instance().locateObject<CompareInfoDataIPtr>(COMPAREINFO_DATA , data_->userId);
    if (olddata == 0)
    {
        MCE_WARN("UpdateTaskI::handle -> Need Reload ID: "<<data_->userId);
    } 
    else 
    {
        
        CompareInfoDataIPtr newdata = new CompareInfoDataI(olddata);
	UpdateData(newdata, data_);
	MyUtil::ServiceI::instance().getObjectCache()->addObject(COMPAREINFO_DATA , (long)newdata->getuserId() , newdata);
    }

} 

void UpdateTaskI::UpdateData(CompareInfoDataIPtr& data, const CompareInfoUpdateNewDataPtr& updateddata) 
{
     for ( ::xce::compareinfo::tableAndvalueSeq::const_iterator itr = updateddata->tablevalue.begin(); itr != updateddata->tablevalue.end(); ++itr) 
    {
	::Ice::Byte table =( *itr )->table;
	switch (table)
       {
           case NETWORK:
                data->clearnetworkName();
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->addnetworkName(*it);
                } 
                break;
           case HIGHTSCHOOL:
                data->clearhightschoolName();
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->addhightschoolName(*it);
                }
                break;
           case UNIVERSITY:
                data->clearuniversityName();
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->adduniversityName(*it);
                }
                break;
           case COLLEGE:
                data->clearcollegeName();
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->addcollegeName(*it);
                }
                break;
         
           case WORKPLACE:
                data->clearworkplaceName();
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->addworkplaceName(*it);
                }
                break;
          case ELEMENTARYSCHOOL:
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->setelementaryschoolName(*it);
                }
                break;
          case JUNIORHIGHTSCHOOL:
                for (MyUtil::LongSeq::iterator it = ( *itr )->value.begin(); it != ( *itr )->value.end(); ++it)
                {
                    data->setjuniorhighschoolName(*it);
                }
                break;

       	  default:
              MCE_WARN("UpdateTaskI::UpdateData -> Invalid table: "<<table);
	  }
       }
}


