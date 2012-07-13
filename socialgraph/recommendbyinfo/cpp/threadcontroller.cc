#include "socialgraph/recommendbyinfo/cpp/threadcontroller.h"

#include "socialgraph/recommendbyinfo/cpp/recommendbyinfo.h"
#include "socialgraph/recommendbyinfo/cpp/dbhelper.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "DbCxxPool/src/ConnectionPoolManager.h"

namespace xce {
namespace socialgraph {

void ViewCountThread::run() {
	using namespace com::xiaonei::xce;
	CLOG(INFO) << "start to load view_count";

  std::string table_name = "view_count"; 
  int lowboundary = 0;
  int last = 0;          

  do {
    size_t queryCount = 0;    

    Statement sql;
    sql << "SELECT * FROM " << table_name << " WHERE id>" << lowboundary << " LIMIT 100000";

    try {
    	queryCount = QueryRunner("user_view_count_preloader", 
					CDbRServer).query(sql, ViewCountHandler(last, RecommendByInfo::view_count_map));
    } catch (Ice::Exception& e) {
			CLOG(INFO) << "ViewCountThread " << table_name << " Ice Exception : " << e.what();
    } catch (...) {
			CLOG(INFO) << "ViewCountThread DB unknow Exception";
    }

    if (last > lowboundary) { 
      lowboundary = last;
    } else if (queryCount <= 0) {
			CLOG(INFO) << "ViewCountThread lowboundary(" << lowboundary << "at the end of " << table_name;
      break;
    }
  } while(true);
}

//---------------------------------------------------------------------------------------

void LoadUnivThread::run() {
	using namespace com::xiaonei::xce;

	CLOG(INFO) << "start to LoadUnivThread";
  std::string table_name = "university_info";
  int lowboundary = 0;
  int last = 0;

  do {                   
		size_t queryCount = 0;    
		
		Statement sql;       // 查数据库
		sql << "SELECT * FROM " << table_name << " where id>" << lowboundary << " limit 100000";       

    try {
    	queryCount = QueryRunner("university_info_friendfinder", 
				CDbRServer).query(sql, UnivBatchReadHandler(last, RecommendByInfo::univ_info_map));
    } catch (Ice::Exception& e) { 
			CLOG(WARN) << "LoadUnivThread " << table_name << " Connection Exception : " << e.what();
    } catch ( ... ) {
			CLOG(WARN) << "LoadUnivThread DB unknow Exception!";
    }

    if(last > lowboundary) {
    	lowboundary = last;
    } else if(queryCount <= 0) {
			CLOG(INFO) << "LoadUnivThread lowboundary(" << lowboundary << ") at end of " << table_name;
    	break;
    }
  } while(true);
}

//---------------------------------------------------------------------------------------

void LoadHighSchoolThread::run() {
	using namespace com::xiaonei::xce;

	CLOG(INFO) << "start to LoadHighSchoolThread";
  std::string table_name = "high_school_info";
  int lowboundary = 0;
  int last = 0;

  do {                   
		size_t queryCount = 0;    
		
		Statement sql;       // 查数据库
		sql << "SELECT * FROM " << table_name << " where id>" << lowboundary << " limit 100000";       

    try {
    	queryCount = QueryRunner("high_school_info_friendfinder", 
				CDbRServer).query(sql, HighschoolBatchReadHandler(last, RecommendByInfo::highschool_info_map));
    } catch (Ice::Exception& e) { 
			CLOG(WARN) << "LoadHighSchoolThread " << table_name << " Connection Exception : " << e.what();
    } catch ( ... ) {
			CLOG(WARN) << "LoadHighSchoolThread DB unknow Exception!";
    }

    if(last > lowboundary) {
    	lowboundary = last;
    } else if(queryCount <= 0) {
			CLOG(INFO) << "LoadHighSchoolThread lowboundary(" << lowboundary << ") at end of " << table_name;
    	break;
    }
  } while(true);
}

//---------------------------------------------------------------------------------------

void LoadJuniorSchoolThread::run() {
	using namespace com::xiaonei::xce;

	CLOG(INFO) << "start to LoadJuniorSchoolThread";
  std::string table_name = "junior_high_school_info";
  int lowboundary = 0;
  int last = 0;

  do {                   
		size_t queryCount = 0;    
		
		Statement sql;       // 查数据库
		sql << "SELECT * FROM " << table_name << " where userid>" << lowboundary << " limit 100000";

    try {
    	queryCount = QueryRunner("junior_high_school_info_friendfinder", 
				CDbRServer).query(sql, JuniorschoolBatchReadHandler(last, RecommendByInfo::juniorschool_info_map));
    } catch (Ice::Exception& e) { 
			CLOG(WARN) << "LoadJuniorSchoolThread " << table_name << " Connection Exception : " << e.what();
    } catch ( ... ) {
			CLOG(WARN) << "LoadJuniorSchoolThread DB unknow Exception!";
    }

    if(last > lowboundary) {
    	lowboundary = last;
    } else if(queryCount <= 0) {
			CLOG(INFO) << "LoadJuniorSchoolThread lowboundary(" << lowboundary << ") at end of " << table_name;
    	break;
    }
  } while(true);
}

//---------------------------------------------------------------------------------------

void LoadElementarySchoolThread::run() {
	using namespace com::xiaonei::xce;

	CLOG(INFO) << "start to LoadElementarySchoolThread";
  std::string table_name = "elementary_school_info";
  int lowboundary = 0;
  int last = 0;

  do {                   
		size_t queryCount = 0;    
		
		Statement sql;       // 查数据库
    sql << "SELECT * FROM " << table_name << " where userid>" << lowboundary << " limit 100000";

    try {
    	queryCount = QueryRunner("elementary_school_info_friendfinder", 
				CDbRServer).query(sql, ElementaryschoolBatchReadHandler(last, RecommendByInfo::elementary_info_map));
    } catch (Ice::Exception& e) { 
			CLOG(WARN) << "LoadElementarySchoolThread " << table_name << " Connection Exception : " << e.what();
    } catch ( ... ) {
			CLOG(WARN) << "LoadElementarySchoolThread DB unknow Exception!";
    }

    if(last > lowboundary) {
    	lowboundary = last;
    } else if(queryCount <= 0) {
			CLOG(INFO) << "LoadElementarySchoolThread lowboundary(" << lowboundary << ") at end of " << table_name;
    	break;
    }
  } while(true);
}

}
}
