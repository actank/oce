#include "UserNetworkPreloader.h"
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::usernetworkpreloader;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserNetworkPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserNetwork.Controller", "ControllerUserNetwork");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetwork.Proxy.Timeout", 300);
		
        int cluster = 10;
		
		GeneralPreloader<mop::hi::svc::model::NetworkManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserNetworkPreloaderFactory,
                ctlEndpoint,
				cluster,
				subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE);
}


ObjectResultPtr UserNetworkPreloaderFactory::create(const MyUtil::IntSeq& ids)
{
	
	ObjectResultPtr results = new ObjectResult;

	ostringstream idsSql;

	idsSql<<" WHERE userid IN (";
	
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
	{
		mop::hi::svc::model::NetworkDataIPtr tempData = new mop::hi::svc::model::NetworkDataI;
		tempData->userId = *it;
		results->data.insert(make_pair(*it, tempData)); 
		
		if (it != ids.begin()) 
		{
			idsSql<< ", ";
		}
		idsSql << *it;
	}
	
	idsSql << ")";
	
	MCE_INFO("UserNetworkPreloaderFactory::create group -> id from: "<<idsSql.str());
	
	{
		BatchUserNetworkResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, stage, network_id, network_name," 
			<< 	" info_id, join_time, status" 
			<<  " FROM " << TABLE_NETWORK << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserHighSchoolResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, high_school_id, high_school_name," 
			<< 	" enroll_year, h_class1, h_class2, h_class3" 
			<<  " FROM " << TABLE_HIGH_SCHOOL_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserUniversityResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, university_id, university_name, dorm," 
			<< 	" department, type_of_course, enroll_year" 
			<<  " FROM " << TABLE_UNIVERSITY_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserWorkplaceResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, workplace_id, workplace_name, address," 
			<< 	" province, city_id, city_name, " 
			<< 	" description, join_year, join_month, " 
			<< 	" quit_year, quit_month, type, " 
			<< 	" job_title_id, position_id" 
			<<  " FROM " << TABLE_WORKPLACE_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserCollegeResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, college_id, college_name, department," 
			<< 	" enroll_year" 
			<<  " FROM " << TABLE_COLLEGE_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserElementarySchoolResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, elementary_school_id, elementary_school_name, elementary_school_year" 
			<<  " FROM " << TABLE_ELEMENTARY_SCHOOL_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserJuniorHighSchoolResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, junior_high_school_id, junior_high_school_name, junior_high_school_year" 
			<<  " FROM " << TABLE_JUNIOR_HIGH_SCHOOL_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserRegionResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, region_id, province_name, city_name" 
			<<  " FROM " << TABLE_REGION_INFO << idsSql.str();
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	
	return results;
}

bool BatchUserNetworkResultHandler::handle(mysqlpp::Row& row) const {
	
	int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setNetworks(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setNetworks(row); 
    }
    return true;
}

bool BatchUserHighSchoolResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setHighSchools(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setHighSchools(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

bool BatchUserUniversityResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setUniversities(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setUniversities(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

bool BatchUserWorkplaceResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setWorkspaces(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setWorkspaces(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

bool BatchUserCollegeResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setColleges(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setColleges(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

bool BatchUserElementarySchoolResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setElementaries(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setElementaries(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

bool BatchUserJuniorHighSchoolResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setJuniorHighSchools(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setJuniorHighSchools(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

bool BatchUserRegionResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setRegions(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setRegions(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}

