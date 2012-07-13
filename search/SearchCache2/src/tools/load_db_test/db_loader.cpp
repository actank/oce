#include "db_loader.h"
#include "util/cpp/TimeCost.h"
#include <limits>

void ReadTask::handle(){
		MyUtil::TimeCost tc = MyUtil::TimeCost::create(table_,1);
		int count = 0;
		vector<vector<string> > rows;
		rows.reserve(READ_BATCH);
		Statement sql;
		sql << "SELECT "<<field_<<" FROM " << table_ << extra_;
		{
				ostringstream oss;
				oss << "SELECT "<<field_<<" FROM " << table_ << extra_;
				cout<<oss.str()<<endl;
		}
		BatchResultHandlerI handler(id_, table_, id_field_, count, rows);
		QueryRunner(desc_, CDbRServer).query(sql, handler);
		MCE_INFO(table_<<" "<<count);
		ostringstream oss;
		tc.step(oss.str());
		manager->finish(id_);
}
int main(){
		MyUtil::init_logger("Mce", "db_loader.log", "DEBUG");
		com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
		MyUtil::TaskManager::instance().config(0, MyUtil::ThreadPoolConfig(1, 1));
		MyUtil::TaskManager::instance().config(1, MyUtil::ThreadPoolConfig(3, 3));
		MyUtil::TaskManager::instance().config(2, MyUtil::ThreadPoolConfig(1, 1));
		MyUtil::TaskManager::instance().config(3, MyUtil::ThreadPoolConfig(4, 4));
		MyUtil::TaskManager::instance().config(4, MyUtil::ThreadPoolConfig(1, 1));
		MyUtil::TaskManager::instance().config(5, MyUtil::ThreadPoolConfig(3, 3));
		MyUtil::TaskManager::instance().config(TASK_LEVEL_LOAD, MyUtil::ThreadPoolConfig(12, 20));

		for(int i = 0;i<BNUM;++i){
				BucketPtr bp(new Bucket);
				buckets_.push_back(bp);
		}
		manager = new BufferManager();
		manager->addBuffer("id",&SearchCacheData::setStatus,MAX_SIZE*2);
		manager->addBuffer("id",&SearchCacheData::setUrl,MAX_SIZE*2);
		manager->addBuffer("id",&SearchCacheData::setName,MAX_SIZE*2);
		manager->addBuffer("id",&SearchCacheData::setStage,MAX_SIZE/2);
		manager->addBuffer("id",&SearchCacheData::setConfig,MAX_SIZE*4);

		manager->addBuffer("userid",&SearchCacheData::setElementarySchools,MAX_SIZE);
		manager->addBuffer("userid",&SearchCacheData::setJuniorSchools,MAX_SIZE);

		manager->addBuffer("userid",&SearchCacheData::setNetworks,MAX_SIZE*4);
		manager->addBuffer("userid",&SearchCacheData::setHighSchools,MAX_SIZE);
		manager->addBuffer("userid",&SearchCacheData::setColleges,MAX_SIZE/2);
		manager->addBuffer("userid",&SearchCacheData::setUniversitys,MAX_SIZE*3);
		manager->addBuffer("userid",&SearchCacheData::setWorkplaces,MAX_SIZE*2);
		manager->addBuffer("userid",&SearchCacheData::setRegionCitys,MAX_SIZE);

		MyUtil::TaskManager::instance().execute(new ReadTask(0,0,"search3_userpassport","user_passport","id","id"," where status=0 and id >= 305000000")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(1,1,DB_SEARCH2_USERURL,"user_url",USER_URL_FIELD,"id", " where id >= 305000000")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(1,2,DB_SEARCH2_USERNAME,"user_names",USER_NAME_FIELD,"id", " where id >= 305000000")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(1,3,DB_SEARCH2_USERSTAGE,"user_stage",USER_STAGE_FIELD,"id", " where id >= 305000000")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(2,4,DB_SEARCH2_USERCONFIG,"user_config",USER_CONFIG_FIELD,"id"," where id >= 305000000")); 


		MyUtil::TaskManager::instance().execute(new ReadTask(3,5,DB_SEARCH2_USERBASIC,"elementary_school_info",ELEMENTARY_SCHOOL_INFO_FIELD,"userid", " where userid>=305000000")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(3,6,DB_SEARCH2_USERBASIC,"junior_high_school_info",JUNIOR_HIGH_SCHOOL_INFO_FIELD,"userid"," where userid>=305000000")); 

		MyUtil::TaskManager::instance().execute(new ReadTask(4,7,DB_SEARCH2_USERBASIC,"network_history",NETWORK_HISTORY_FIELD,"userid"," where userid>=305000000")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(3,8,DB_SEARCH2_USERBASIC,"high_school_info",HIGH_SCHOOL_INFO_FIELD,"userid"," force index(idx_userid) WHERE userid!=0 and userid>=305000000 order by userid asc ")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(3,9,DB_SEARCH2_USERBASIC,"college_info",COLLEGE_INFO_FIELD,"userid"," force index(idx_userid) WHERE userid!=0 and userid>=305000000 order by userid asc "));
		MyUtil::TaskManager::instance().execute(new ReadTask(5,10,DB_SEARCH2_USERBASIC,"university_info",UNIVERSITY_INFO_FIELD,"userid"," force index(idx_userid) where userid>=305000000 order by userid asc ")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(5,11,DB_SEARCH2_USERBASIC,"workplace_info",WORKPLACE_INFO_FIELD,"userid"," force index(idx_userid) where userid>=305000000 order by userid asc ")); 
		MyUtil::TaskManager::instance().execute(new ReadTask(5,12,DB_SEARCH2_USERBASIC,"region_info",REGION_INFO_FIELD,"userid"," force index(idx_userid) where userid>=305000000 order by userid asc ")); 
		IceUtil::ThreadPtr t = manager;
		t->start();
		getchar();
		return 0;
}

