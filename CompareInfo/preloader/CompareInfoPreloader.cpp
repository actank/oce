#include "CompareInfoPreloader.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "CompareInfo/data/CompareInfoData.h"
#include "util/cpp/MD5Util.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace ::xce::compareinfo;
//using namespace xce::md5;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&CompareInfoPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("CompareInfo.Controller", "ControllerCompareInfo");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CompareInfo.Proxy.Timeout", 300);
		
        int cluster= 1;
		
	GeneralPreloader<xce::compareinfo::CompareInfoManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new CompareInfoPreloaderFactory,
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


ObjectResultPtr CompareInfoPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) 
{
	MCE_INFO("CompareInfoPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	ObjectResultPtr results = new ObjectResult;
        //get userid from userpassport
       {        BatchUsersIdProducerI handler(results);
		Statement sql;	
                sql << "SELECT id FROM user_passport WHERE id >= " << minId
                      << " ORDER BY id LIMIT " << PRODUCE_BATCH_SIZE;
		QueryRunner( DB_CMPAREINFO_TABLE_PASSPORT, CDbRServer ).query( sql, handler );
	}

       
        {
                BatchNetworkResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , network_name FROM " << TABLE_NETWORK << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_NETWORK, CDbRServer ).query(sql, handler);
        }
        {
                BatchHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , high_school_name" 
                        <<  " FROM " << TABLE_HIGH_SCHOOL_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_HIGH_SCHOOL_INFO, CDbRServer ).query(sql, handler);
        }
        {
                BatchUniversityResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , university_name " 
                        <<  " FROM " << TABLE_UNIVERSITY_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_UNIVERSITY_INFO, CDbRServer ).query(sql, handler);
        }
        {
                BatchWorkplaceResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , workplace_name" 
                        <<  " FROM " << TABLE_WORKPLACE_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_WORKPLACE_INFO , CDbRServer ).query(sql, handler);
        }
        {
                BatchCollegeResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid  , college_name" 
                        <<  " FROM " << TABLE_COLLEGE_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_COLLEGE_INFO, CDbRServer ).query(sql, handler);
        }

	{
                BatchElementarySchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , elementary_school_name" 
                        <<  " FROM " << TABLE_ELEMENTARY_SCHOOL_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_ELEMENTARY_SCHOOL_INFO , CDbRServer ).query(sql, handler);
        }
        {
                BatchJuniorHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , junior_high_school_name" 
                        <<  " FROM " << TABLE_JUNIOR_HIGH_SCHOOL_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner(DB_CMPAREINFO_TABLE_JUNIOR_HIGH_SCHOOL_INFO , CDbRServer ).query(sql, handler);
        }
	
	return results;
}

ObjectResultPtr CompareInfoPreloaderFactory::create(const MyUtil::IntSeq& ids) {

  	MCE_DEBUG("UserConfigPreloader::create group -> id size: "<<ids.size());
        int minId = ids.front();
        int maxId = ids[ids.size()-1];
	MCE_INFO("CompareInfoPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);

	ObjectResultPtr results = new ObjectResult;
       
        for(std::vector<int>::const_iterator it = ids.begin(); it != ids.end() ; ++it)
        {
           CompareInfoDataIPtr newData = new CompareInfoDataI;
           newData->setuserId( *it );
           results->data.insert(make_pair(*it, newData));
        }
   
       
        {
                BatchNetworkResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , network_name FROM " << TABLE_NETWORK << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_NETWORK, CDbRServer ).query(sql, handler);
        }
        {
                BatchHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , high_school_name" 
                        <<  " FROM " << TABLE_HIGH_SCHOOL_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_HIGH_SCHOOL_INFO, CDbRServer ).query(sql, handler);
        }
        {
                BatchUniversityResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , university_name " 
                        <<  " FROM " << TABLE_UNIVERSITY_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_UNIVERSITY_INFO, CDbRServer ).query(sql, handler);
        }
        {
                BatchWorkplaceResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , workplace_name" 
                        <<  " FROM " << TABLE_WORKPLACE_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_WORKPLACE_INFO , CDbRServer ).query(sql, handler);
        }
        {
                BatchCollegeResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid  , college_name" 
                        <<  " FROM " << TABLE_COLLEGE_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_COLLEGE_INFO, CDbRServer ).query(sql, handler);
        }

	{
                BatchElementarySchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , elementary_school_name" 
                        <<  " FROM " << TABLE_ELEMENTARY_SCHOOL_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner( DB_CMPAREINFO_TABLE_ELEMENTARY_SCHOOL_INFO , CDbRServer ).query(sql, handler);
        }
        {
                BatchJuniorHighSchoolResultHandler handler(results );
                Statement sql;
                sql << "SELECT userid , junior_high_school_name" 
                        <<  " FROM " << TABLE_JUNIOR_HIGH_SCHOOL_INFO << " WHERE userid >= "<< minId <<" AND userid <= "<< maxId;
                QueryRunner(DB_CMPAREINFO_TABLE_JUNIOR_HIGH_SCHOOL_INFO , CDbRServer ).query(sql, handler);
        }
	
	return results;
}

