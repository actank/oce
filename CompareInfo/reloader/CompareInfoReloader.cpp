#include "CompareInfoReloader.h"
#include "CompareInfoAdapter.h"
#include "CompareInfo/data/CompareInfoData.h"
#include "util/cpp/TimeCost.h"
#include <ServiceI.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using MyUtil::TimeCost;


using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace ::xce::compareinfo;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::compareinfo::CompareInfoReloaderI::instance(), service.createIdentity(xce::compareinfo::COMPAREINFO_RELOADER, ""));
	TaskManager::instance().config(xce::compareinfo::TASK_LEVEL_RELOAD, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(xce::compareinfo::TASK_LEVEL_UPDATE, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(xce::compareinfo::TASK_LEVEL_INSERT, MyUtil::ThreadPoolConfig(1, 5));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CompareInfoReloader.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CompareInfoReloader.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerCompareInfoReloader", &xce::compareinfo::CompareInfoReloaderI::instance(), mod, interval);
	xce::compareinfo::CompareInfoReloaderI::instance().setValid(true);

}

void xce::compareinfo::CompareInfoReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	ostringstream oss;
	oss << "CompareInfoReloaderI::reload -> size: "<<ids.size() << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::IntSeq tempids;
	for (MyUtil::IntSeq::const_iterator i=ids.begin(); i!=ids.end();++i){
		if (*i > 0) {
			tempids.push_back(*i);
		}
	}
	MyUtil::TaskManager::instance().execute(new ReloadTask(tempids, current));
}

void xce::compareinfo::CompareInfoReloaderI::update(const xce::compareinfo::CompareInfoUpdateOldDataPtr& data, const Ice::Current& current) {
	ostringstream oss;
	oss << "CompareInfoReloaderI::update ->: "<<data->userId << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	
	MyUtil::TaskManager::instance().execute(new UpdateTask(data, current));
}


bool xce::compareinfo::CompareInfoReloaderI::isValid(const Ice::Current& current) {
	IceUtil::RWRecMutex::RLock lock(validmutex_);
	return valid_;
}

void xce::compareinfo::CompareInfoReloaderI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "CompareInfoReloaderI::setValid -> " << valid;
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	IceUtil::RWRecMutex::WLock lock(validmutex_);
	valid_ = valid;
	if (valid_) {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
	} else {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
	}
}


void xce::compareinfo::ReloadTask::handle()
{
        std::vector<std::vector<int> > ids;
	DistributeIds(ids);
	for (unsigned int i = 0; i < ids.size(); ++i) 
        {
	    if (ids[i].size() <= 0) 
            {
		continue;
	    }
	    std::vector<std::pair<int, Ice::ObjectPtr> > data;
	    for (std::vector<int>::const_iterator j = ids[i].begin(); j != ids[i].end(); ++j) 
            {
		data.push_back(std::pair<int, Ice::ObjectPtr>(*j, 0));
	    }
	    ReloadCallGuarantee::instance().DoInTime(data);
	}

}

void xce::compareinfo::ReloadTask::DistributeIds(std::vector<std::vector<int> >& ids) {
	int cluster = 1;
	for (int i = 0; i < cluster; ++i)
        {
  	    ids.push_back(vector<int>());
	}
	for (MyUtil::IntSeq::const_iterator i = ids_.begin(); i != ids_.end(); ++i) 
        {
	    ids[(*i) % cluster].push_back(*i);
	}
}




MyUtil::ObjectResultPtr xce::compareinfo::CompareInfoReloaderI::GetObjectResult(const MyUtil::IntSeq& ids)
{
       ObjectResultPtr results = new ObjectResult;
       std::stringstream sqlcom;
       sqlcom<<"(";
       for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end();) 
       {
             sqlcom << *it;
             ++it;
             if (it != ids.end())
             {
                 sqlcom<< ", ";
             }
       }
        sqlcom << ")";

      //get userId from userpassport
       {        BatchUsersIdProducerI handler(results);
                Statement sql;
                sql << "SELECT id FROM user_passport  WHERE id IN ";
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_PASSPORT, CDbRServer ).query( sql, handler );
        }
        {
                BatchNetworkResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , network_name FROM " << TABLE_NETWORK << " WHERE userId IN ";
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_NETWORK, CDbRServer ).query(sql, handler);                
        }
        {
                BatchHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , high_school_name" <<  " FROM " << TABLE_HIGH_SCHOOL_INFO << " WHERE userId IN";
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_HIGH_SCHOOL_INFO, CDbRServer ).query(sql, handler);
                
        }
        {
                BatchUniversityResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , university_name " <<  " FROM " << TABLE_UNIVERSITY_INFO << " WHERE userId IN" ;
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_UNIVERSITY_INFO, CDbRServer ).query(sql, handler);
        }
        {
                BatchWorkplaceResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , workplace_name" <<  " FROM " << TABLE_WORKPLACE_INFO << " WHERE userId IN";
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_WORKPLACE_INFO , CDbRServer ).query(sql, handler);
        }
        {
                BatchCollegeResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , college_name"  <<  " FROM " << TABLE_COLLEGE_INFO << " WHERE userId IN ";
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_COLLEGE_INFO, CDbRServer ).query(sql, handler);
         }

        {
                BatchElementarySchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , elementary_school_name" <<  " FROM " << TABLE_ELEMENTARY_SCHOOL_INFO << " WHERE userId IN";
                sql << sqlcom.str();
                QueryRunner( DB_CMPAREINFO_TABLE_ELEMENTARY_SCHOOL_INFO , CDbRServer ).query(sql, handler);
        }
        {
                BatchJuniorHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , junior_high_school_name" <<  " FROM " << TABLE_JUNIOR_HIGH_SCHOOL_INFO << " WHERE userId IN ";
                sql << sqlcom.str();
                QueryRunner(DB_CMPAREINFO_TABLE_JUNIOR_HIGH_SCHOOL_INFO , CDbRServer ).query(sql, handler);
        }

        return results;                                   
}

bool xce::compareinfo::ReloadCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	if (params.size() <= 0) {
                return true;
        }

	try {
		std::vector<int> ids;
	
		for (std::vector<std::pair<int, Ice::ObjectPtr> >::const_iterator i = params.begin(); i != params.end(); ++i) {
			ids.push_back(i->first);
		}

		MyUtil::ObjectResultPtr result = xce::compareinfo::CompareInfoReloaderI::instance().GetObjectResult(ids);

		if (result->data.size() <= 0) {
			return true;
		}

		int mod = *ids.begin();
		std::vector<CompareInfoManagerPrx> proxies = xce::compareinfo::CompareInfoAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("ReloadCallGuarantee::Do -> no proxy");
			return false;
		}
	

              for (std::vector<CompareInfoManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->setData(result);
		}

		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("ReloadCallGuarantee::Do -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("ReloadCallGuarantee::Do -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("ReloadCallGuarantee::Do -> unknown exception");
	}
	return false;
}

bool xce::compareinfo::ReloadCallGuarantee::Redo(const std::vector<int>& ids) {
	if (ids.size() <= 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = xce::compareinfo::CompareInfoReloaderI::instance().GetObjectResult(ids);
		if (result->data.size() <= 0) {
			return true;
		}
		int mod = *ids.begin();
		std::vector<CompareInfoManagerPrx> proxies = xce::compareinfo::CompareInfoAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("ReloadCallGuarantee::Redo -> no proxy");
		}
		for (std::vector<CompareInfoManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->setData(result);
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("ReloadCallGuarantee::Redo -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("ReloadCallGuarantee::Redo -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("ReloadCallGuarantee::Redo -> unknown exception");
	}
	return false;
}


xce::compareinfo::CompareInfoUpdateNewDataPtr xce::compareinfo::UpdateTask::GetNewData() {

        ObjectResultPtr results = new ObjectResult;
	CompareInfoUpdateNewDataPtr newdata = new CompareInfoUpdateNewData;
	newdata->userId = data_->userId;
        BatchUsersIdProducerI handler(results);
        Statement sql;
        sql << "SELECT id FROM user_passport  WHERE id = " << data_->userId;
        QueryRunner( DB_CMPAREINFO_TABLE_PASSPORT, CDbRServer ).query( sql, handler );
        
	for( MyUtil::ByteSeq::const_iterator i = data_->table.begin(); i != data_->table.end(); ++i)
        {
 	   switch ( *i )
           {
	   case NETWORK:
             {
                BatchNetworkResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , network_name FROM " << TABLE_NETWORK << " WHERE userId = " << data_->userId ;
		QueryRunner( DB_CMPAREINFO_TABLE_NETWORK, CDbRServer ).query(sql, handler);
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
		if (it == results->data.end())
		{
                   MCE_WARN("UpdateTask::GetNewData -> no data in network id = "<< data_->userId );
                }
		else
		{
                   std::vector<long> networkName = CompareInfoDataIPtr::dynamicCast(it->second)->getnetworkName();
                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = NETWORK ;	   
                   for (std::vector<long>::size_type i = 0; i != networkName.size(); i ++)
		   {
                           tablevaluetmp->value.push_back(networkName.at(i));

		   }
                  newdata->tablevalue.push_back( tablevaluetmp );
		}
              
		break;
             }
  	   case HIGHTSCHOOL:
            {
                BatchHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , high_school_name FROM " <<  TABLE_HIGH_SCHOOL_INFO << " WHERE userId = " << data_->userId ;
                QueryRunner( DB_CMPAREINFO_TABLE_HIGH_SCHOOL_INFO, CDbRServer ).query(sql, handler);      
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
                if (it == results->data.end())
                {
                   MCE_WARN("UpdateTask::GetNewData -> no data in highschool id = "<< data_->userId );
                }
                else
                {
                   std::vector<long> highschoolName = CompareInfoDataIPtr::dynamicCast(it->second)->gethightschoolName();
                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = HIGHTSCHOOL;
                   for (std::vector<long>::size_type i = 0; i != highschoolName.size(); i ++)
                   {
                           tablevaluetmp->value.push_back(highschoolName.at(i));

                   }
                  newdata->tablevalue.push_back( tablevaluetmp );

                }
                break;
            }
	  case UNIVERSITY:
            {
                BatchUniversityResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , university_name FROM " << TABLE_UNIVERSITY_INFO << " WHERE userId = " << data_->userId ;
                QueryRunner( DB_CMPAREINFO_TABLE_UNIVERSITY_INFO, CDbRServer ).query(sql, handler);
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
                if (it == results->data.end())
                {
                   MCE_WARN("UpdateTask::GetNewData -> no data in university id = "<< data_->userId );
                }
                else
                {
                   std::vector<long>  universityName = CompareInfoDataIPtr::dynamicCast(it->second)->getuniversityName();
                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = UNIVERSITY;
                   for (std::vector<long>::size_type i = 0; i != universityName.size(); i ++)
                   {
                           tablevaluetmp->value.push_back(universityName.at(i));

                   }
                   newdata->tablevalue.push_back( tablevaluetmp );
                }

		break;
            }
	  case COLLEGE:
           {
                BatchCollegeResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , college_name"  <<  " FROM " << TABLE_COLLEGE_INFO << " WHERE userId = " << data_->userId ;
                QueryRunner( DB_CMPAREINFO_TABLE_COLLEGE_INFO, CDbRServer ).query(sql, handler);
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
                if (it == results->data.end())
                {
                   MCE_WARN("UpdateTask::GetNewData -> no data in college id = "<< data_->userId );
                }
                else
                {
                   std::vector<long>  collegeName = CompareInfoDataIPtr::dynamicCast(it->second)->getcollegeName();
                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = COLLEGE;
                   for (std::vector<long>::size_type i = 0; i != collegeName.size(); i ++)
                   {
                           tablevaluetmp->value.push_back(collegeName.at(i));

                   }
                    newdata->tablevalue.push_back( tablevaluetmp );
                }    
                break;
            }
          case WORKPLACE:
            {
                BatchWorkplaceResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , workplace_name"  <<  " FROM " << TABLE_WORKPLACE_INFO << " WHERE userId = " << data_->userId ;
		QueryRunner( DB_CMPAREINFO_TABLE_WORKPLACE_INFO , CDbRServer ).query(sql, handler);
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
                if (it == results->data.end())
                {
                   MCE_WARN("UpdateTask::GetNewData -> no data in workplace id = "<< data_->userId );
                }
                else
                {
                   std::vector<long>   workplaceName = CompareInfoDataIPtr::dynamicCast(it->second)->getworkplaceName();
                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = WORKPLACE;
                   for (std::vector<long>::size_type i = 0; i != workplaceName.size(); i ++)
                   {
                           tablevaluetmp->value.push_back(workplaceName.at(i));

                   }
                  newdata->tablevalue.push_back( tablevaluetmp );
                }
                break;
            }
	  case ELEMENTARYSCHOOL:
           {
                BatchElementarySchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , elementary_school_name" <<  " FROM " << TABLE_ELEMENTARY_SCHOOL_INFO <<" WHERE userId = " << data_->userId ;
                QueryRunner( DB_CMPAREINFO_TABLE_ELEMENTARY_SCHOOL_INFO , CDbRServer ).query(sql, handler);               
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
                if (it == results->data.end())
                {
                    MCE_WARN("UpdateTask::GetNewData -> no data in elmentaryschool id = "<< data_->userId );
                }
                else
                {
                   std::vector<long>  elementarySchoolName;
                   elementarySchoolName.push_back( CompareInfoDataIPtr::dynamicCast(it->second)->getelementarySchoolName());
 
                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = ELEMENTARYSCHOOL;
                   for (std::vector<long>::size_type i = 0; i != elementarySchoolName.size(); i ++)
                   {
                           tablevaluetmp->value.push_back(elementarySchoolName.at(i));

                   }
                  newdata->tablevalue.push_back( tablevaluetmp );
                }
                break;
            }
	  case JUNIORHIGHTSCHOOL:
            { 
                BatchJuniorHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userId , junior_high_school_name"  <<  " FROM " << TABLE_JUNIOR_HIGH_SCHOOL_INFO <<" WHERE userId = " << data_->userId ;
    	        QueryRunner(DB_CMPAREINFO_TABLE_JUNIOR_HIGH_SCHOOL_INFO , CDbRServer ).query(sql, handler);
                MyUtil::ObjectMap::iterator it = results->data.find( newdata->userId );
                if (it == results->data.end())
                {
                    MCE_WARN("UpdateTask::GetNewData -> no data in juniorhischool id = "<< data_->userId );
                }
                else
                {
                   std::vector<long>  juniorHighSchoolName;
                   juniorHighSchoolName.push_back( CompareInfoDataIPtr::dynamicCast(it->second)->getjuniorHighSchoolName());

                   tableAndvaluePtr tablevaluetmp = new tableAndvalue;
                   tablevaluetmp->table = JUNIORHIGHTSCHOOL;
                   for (std::vector<long>::size_type i = 0; i != juniorHighSchoolName.size(); i ++)
                   {
                           tablevaluetmp->value.push_back(juniorHighSchoolName.at(i));

                   }
                 newdata->tablevalue.push_back( tablevaluetmp );
                }
	  	break;
             }
	  default:
	     MCE_WARN("UpdateTask::GetNewData -> Oh, no!");
	     
          }
      
     }

	return newdata;
 
}

bool xce::compareinfo::UpdateCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	if (params.size() != 1)
        {
		MCE_WARN("UpdateCallGuarantee::Do -> params.size is not 1 but "<<params.size());
		return true;
	}

	try {
		int mod = params.begin()->first;
		std::vector<CompareInfoManagerPrx> proxies = xce::compareinfo::CompareInfoAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("UpdateCallGuarantee::Do -> no proxy");
			return false;
		}
		
                for (std::vector<CompareInfoManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->update(CompareInfoUpdateNewDataPtr::dynamicCast(params.begin()->second));
		}
		return true;
	} catch (const std::exception& e) {
		MCE_WARN("UpdateCallGuarantee::Do -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("UpdateCallGuarantee::Do -> unknown exception");
	}
	return false;
}
bool xce::compareinfo::UpdateCallGuarantee::Redo(const std::vector<int>& ids) {
	if (ids.size() <= 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = xce::compareinfo::CompareInfoReloaderI::instance().GetObjectResult(ids);
		if (result->data.size() <= 0) {
                        return true;
                }
		int mod = *ids.begin();
		std::vector<CompareInfoManagerPrx> proxies = xce::compareinfo::CompareInfoAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("UpdateCallGuarantee::Redo -> no proxy");
		}
		for (std::vector<CompareInfoManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->setData(result);
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("UpdateCallGuarantee::Redo -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("UpdateCallGuarantee::Redo -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("UpdateCallGuarantee::Redo -> unknown exception");
	}
	return false;
}


void xce::compareinfo::UpdateTask::handle() {
	CompareInfoUpdateNewDataPtr newdata = GetNewData();
	std::vector<std::pair<int, Ice::ObjectPtr> > data;
	data.push_back(std::pair<int, Ice::ObjectPtr>(newdata->userId, newdata));
	UpdateCallGuarantee::instance().DoInTime(data);
}




