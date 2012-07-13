#include "InfoResultCollector.h"
#include <boost/algorithm/string.hpp>

using namespace com::xiaonei::xce;
using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;
using namespace xce::md5;
using namespace std;
using namespace MyUtil;
using namespace boost;

void InfoResultCollector::createInfoResultBatch(int userId) {
	//TaskManager::instance().execute( new LoadWorkplaceTask(userId) );
	TaskManager::instance().execute( new LoadUnivTask(userId) );
	TaskManager::instance().execute( new LoadHighschoolTask(userId) );
	TaskManager::instance().execute( new LoadJuniorschollTask(userId) );
	TaskManager::instance().execute( new LoadElementschoolTask(userId) );
	TaskManager::instance().execute( new LoadBirthdayTask(userId) );
}

void InfoResultCollector::getInfoResultFromWorkplace(bool empty) {
	MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace");
	string tableName = "workplace_info";
    int lowboundary = 0;
    int last = 0;

	map<long, vector<int> > workplaceResultMap;

	if( empty ) {           //释放内存
		MCE_DEBUG("getInfoResultFromWorkplace true");
		return;
	}
	
	do {
		size_t queryCount = 0;

		Statement sql;       // 查数据库
        sql
            << "SELECT * FROM " << tableName
            << " where id>" << lowboundary
            << " limit 100000";

	   	try {
            queryCount = QueryRunner("workplace_info_friendfinder", CDbRServer).query( sql, WorkplaceBatchReadHandler(last, workplaceResultMap) );
        } catch (Ice::Exception& e) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] getInfoResultFromWorkplace" << tableName << " Connection Exception : " << e.what() << "\n");
        } catch ( ... ) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] DB unknow Exception ");
        }

        if(last > lowboundary) {
            lowboundary = last;
        } else if(queryCount <= 0) {
            MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace lowboundary(" << lowboundary << ") at end of " << tableName);
            break;
        }

	} while(true);

	ViewCountDataPtr obj = ServiceI::instance().locateObject<ViewCountDataPtr>( VIEW_COUNT, VIEW_INDEX );
	if (!obj) {
		MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace find no ViewCountDataPtr");
		obj = new ViewCountData();
	}

	LongSeq md5Seq;
	InfoDataSeq dataSeq;
	for(map< long, vector<int> >::iterator iter=workplaceResultMap.begin();
			iter!=workplaceResultMap.end(); ++iter) {
		InfoDataPtr data = new InfoData();	

		obj->GetDataOrderedByCount(iter->second);

		data->idSeq.swap( iter->second );
		
		md5Seq.push_back( iter->first );
		dataSeq.push_back( data );
		
		if( (int)md5Seq.size() >= 1000) {
			for(int cluster=begin; cluster<end; ++cluster) {
				try {
					FriendFinderInfoCacheReplicaAdapter::instance().setWorkplaceDataWithMask( cluster, md5Seq, dataSeq ); 	// 写WorkplaceDataCache 
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setWorkplaceDataWithMask Ice Exception : " << e.what() << "\n");
				} catch ( std::exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setWorkplaceDataWithMask std Exception : " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setWorkplaceDataWithMask Unknow Exception");
				}
			}
			md5Seq.clear();
			dataSeq.clear();
		}
	}

	if( (int)md5Seq.size() > 0 ) {
		for(int cluster=begin; cluster<end; ++cluster) {
			try {
				FriendFinderInfoCacheReplicaAdapter::instance().setWorkplaceDataWithMask( cluster, md5Seq, dataSeq ); 	// 写WorkplaceDataCache
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setWorkplaceDataWithMask Ice Exception : " << e.what() << "\n");
			} catch ( std::exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setWorkplaceDataWithMask std Exception : " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[InfoResultCollector] setWorkplaceDataWithMask Unknow Exception");
			}
		}
	}
}

void InfoResultCollector::getInfoResultFromUniv(bool empty) {
	MCE_INFO("[InfoResultCollector] getInfoResultFromUniv");
	string tableName = "university_info";
    int lowboundary = 0;
    int last = 0;

	map<long, vector<int> > univResultMap;

	if( empty ) {
		MCE_DEBUG("getInfoResultFromUniv true");
		return;
	}
	
	do {
		size_t queryCount = 0;

		Statement sql;       // 查数据库
        sql
            << "SELECT * FROM " << tableName
            << " where id>" << lowboundary
            << " limit 100000";

	   	try {
            queryCount = QueryRunner("university_info_friendfinder", CDbRServer).query( sql, UnivBatchReadHandler(last, univResultMap) );
        } catch (Ice::Exception& e) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] getInfoResultFromUniv" << tableName << " Connection Exception : " << e.what() << "\n");
        } catch ( ... ) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] DB unknow Exception ");
        }

        if(last > lowboundary) {
            lowboundary = last;
        } else if(queryCount <= 0) {
            MCE_INFO("[InfoResultCollector] getInfoResultFromUniv lowboundary(" << lowboundary << ") at end of " << tableName);
            break;
        }

	} while(true);

    ViewCountDataPtr obj = ServiceI::instance().locateObject<ViewCountDataPtr>( VIEW_COUNT, VIEW_INDEX ); 
    if (!obj) {          
        MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace find no ViewCountDataPtr");
        obj = new ViewCountData();
    }

	LongSeq md5Seq;
	InfoDataSeq dataSeq;
	for(map< long, vector<int> >::iterator iter=univResultMap.begin();
			iter!=univResultMap.end(); ++iter) {
		InfoDataPtr data = new InfoData();	

		obj->GetDataOrderedByCount(iter->second);

		data->idSeq.swap( iter->second );
		
		md5Seq.push_back( iter->first );
		dataSeq.push_back( data );
		
		if( (int)md5Seq.size() >= 1000) {
			MCE_INFO( "[InfoResultCollector] getInfoResultFromUniv md5Seq size " << md5Seq.size() );
			for(int cluster=begin; cluster<end; ++cluster) {
				try {
					FriendFinderInfoCacheReplicaAdapter::instance().setUnivDataWithMask( cluster, md5Seq, dataSeq ); 	// 写UnivDataCache
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setUnivDataWithMask Ice Exception : " << e.what() << "\n");
				} catch ( std::exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setUnivDataWithMask std Exception : " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setUnivDataWithMask Unknow Exception");
				}
			}
			md5Seq.clear();
			dataSeq.clear();
		}
	}

	if( (int)md5Seq.size() > 0 ) {
		for(int cluster=begin; cluster<end; ++cluster) {
			try {
				FriendFinderInfoCacheReplicaAdapter::instance().setUnivDataWithMask( cluster, md5Seq, dataSeq ); 	// 写UnivDataCache
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setUnivDataWithMask Ice Exception : " << e.what() << "\n");
			} catch ( std::exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setUnivDataWithMask std Exception : " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[InfoResultCollector] setUnivDataWithMask Unknow Exception");
			}
		}
	}
}

void InfoResultCollector::getInfoResultFromHighschool(bool empty) {
	MCE_INFO("[InfoResultCollector] getInfoResultFromHighschool");
	string tableName = "high_school_info";
    int lowboundary = 0; 
	int last = 0;

	map<long, vector<int> > highschoolResultMap;

	if( empty ) {
		MCE_DEBUG("getInfoResultFromHighschool true");
		return;
	}
	
	do {
		size_t queryCount = 0;

		Statement sql;       // 查数据库
        sql
            << "SELECT * FROM " << tableName
            << " where id>" << lowboundary
            << " limit 100000";

	   	try {
            queryCount = QueryRunner("high_school_info_friendfinder", CDbRServer).query( sql, HighschoolBatchReadHandler(last, highschoolResultMap) );
        } catch (Ice::Exception& e) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] getInfoResultFromHighschool" << tableName << " Connection Exception : " << e.what() << "\n");
        } catch ( ... ) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] DB unknow Exception ");
        }

        if(last > lowboundary) {
            lowboundary = last;
        } else if(queryCount <= 0) {
            MCE_INFO("[InfoResultCollector] getInfoResultFromHighschool lowboundary(" << lowboundary << ") at end of " << tableName);
            break;
        }

	} while(true);

    ViewCountDataPtr obj = ServiceI::instance().locateObject<ViewCountDataPtr>( VIEW_COUNT, VIEW_INDEX ); 
    if (!obj) {          
        MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace find no ViewCountDataPtr");
        obj = new ViewCountData();
    }

	LongSeq md5Seq;
	InfoDataSeq dataSeq;
	for(map< long, vector<int> >::iterator iter=highschoolResultMap.begin();
			iter!=highschoolResultMap.end(); ++iter) {
		InfoDataPtr data = new InfoData();	

		obj->GetDataOrderedByCount(iter->second);

		data->idSeq.swap( iter->second );
		
		md5Seq.push_back( iter->first );
		dataSeq.push_back( data );
		
		if( (int)md5Seq.size() >= 1000) {
			for(int cluster=begin; cluster<end; ++cluster) {
				try {
					FriendFinderInfoCacheReplicaAdapter::instance().setHighschoolDataWithMask( cluster, md5Seq, dataSeq ); 	// 写HighschoolDataCache
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setHighschoolDataWithMask Ice Exception : " << e.what() << "\n");
				} catch ( std::exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setHighschoolDataWithMask std Exception : " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setHighschoolDataWithMask Unknow Exception");
				}
			}
			md5Seq.clear();
			dataSeq.clear();
		}
	}

	if( (int)md5Seq.size() > 0 ) {
		for(int cluster=begin; cluster<end; ++cluster) {
			try {
				FriendFinderInfoCacheReplicaAdapter::instance().setHighschoolDataWithMask( cluster, md5Seq, dataSeq ); 	// 写HighschoolDataCache
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setHighschoolDataWithMask Ice Exception : " << e.what() << "\n");
			} catch ( std::exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setHighschoolDataWithMask std Exception : " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[InfoResultCollector] setHighschoolDataWithMask Unknow Exception");
			}
		}
	}
}

void InfoResultCollector::getInfoResultFromJuniorschool(bool empty) {
	MCE_INFO("[InfoResultCollector] getInfoResultFromJuniorschool");
	string tableName = "junior_high_school_info";
    int lowboundary = 0; 
	int last = 0;

	map<long, vector<int> > juniorschoolResultMap;

	if( empty ) {
		MCE_DEBUG("getInfoResultFromJuniorschool true");
		return;
	}
	
	do {
		size_t queryCount = 0;

		Statement sql;       // 查数据库
        sql
            << "SELECT * FROM " << tableName
            << " where userid>" << lowboundary
            << " limit 100000";              

	   	try {
            queryCount = QueryRunner("junior_high_school_info_friendfinder", CDbRServer).query( sql, JuniorschoolBatchReadHandler(last, juniorschoolResultMap) );
        } catch (Ice::Exception& e) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] getInfoResultFromJuniorschool" << tableName << " Connection Exception : " << e.what() << "\n");
        } catch ( ... ) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] DB unknow Exception ");
        }

        if(last > lowboundary) {
            lowboundary = last;
        } else if(queryCount <= 0) {
            MCE_INFO("[InfoResultCollector] getInfoResultFromJuniorschool lowboundary(" << lowboundary << ") at end of " << tableName);
            break;
        }
	} while(true);

    ViewCountDataPtr obj = ServiceI::instance().locateObject<ViewCountDataPtr>( VIEW_COUNT, VIEW_INDEX ); 
    if (!obj) {          
        MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace find no ViewCountDataPtr");
        obj = new ViewCountData();
    }

	LongSeq md5Seq;
	InfoDataSeq dataSeq;
	for(map< long, vector<int> >::iterator iter=juniorschoolResultMap.begin();
			iter!=juniorschoolResultMap.end(); ++iter) {
		InfoDataPtr data = new InfoData();	

		obj->GetDataOrderedByCount(iter->second);

		data->idSeq.swap( iter->second );
		
		md5Seq.push_back( iter->first );
		dataSeq.push_back( data );
		
		if( (int)md5Seq.size() >= 1000) {
			for(int cluster=begin; cluster<end; ++cluster) {
				try {
					FriendFinderInfoCacheReplicaAdapter::instance().setJuniorschoolDataWithMask( cluster, md5Seq, dataSeq ); 	// 写JuniorschoolDataCache
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setJuniorschoolDataWithMask Ice Exception : " << e.what() << "\n");
				} catch ( std::exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setJuniorschoolDataWithMask std Exception : " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setJuniorschoolDataWithMask Unknow Exception");
				}
			}
			md5Seq.clear();
			dataSeq.clear();
		}
	}

	if( (int)md5Seq.size() > 0 ) {
		for(int cluster=begin; cluster<end; ++cluster) {
			try {
				FriendFinderInfoCacheReplicaAdapter::instance().setJuniorschoolDataWithMask( cluster, md5Seq, dataSeq ); 	// 写JuniorschoolDataCache
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setJuniorschoolDataWithMask Ice Exception : " << e.what() << "\n");
			} catch ( std::exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setJuniorschoolDataWithMask std Exception : " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[InfoResultCollector] setJuniorschoolDataWithMask Unknow Exception");
			}
		}
	}
}

void InfoResultCollector::getInfoResultFromElementschool(bool empty) {
	MCE_INFO("[InfoResultCollector] getInfoResultFromElementschool");
	string tableName = "elementary_school_info";
    int lowboundary = 0; 
	int last = 0;

	map<long, vector<int> > elementaryschoolResultMap;

	if( empty ) {
		MCE_DEBUG("getInfoResultFromElementschool true");
		return;
	}
	
	do {
		size_t queryCount = 0;

		Statement sql;       // 查数据库
        sql
            << "SELECT * FROM " << tableName
            << " where userid>" << lowboundary
            << " limit 100000";

	   	try {
            queryCount = QueryRunner("elementary_school_info_friendfinder", CDbRServer).query( sql, ElementaryschoolBatchReadHandler(last, elementaryschoolResultMap) );
        } catch (Ice::Exception& e) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] getInfoResultFromElementschool" << tableName << " Connection Exception : " << e.what() << "\n");
        } catch ( ... ) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] DB unknow Exception ");
        }

        if(last > lowboundary) {
            lowboundary = last;
        } else if(queryCount <= 0) {
            MCE_INFO("[InfoResultCollector] getInfoResultFromElementschool lowboundary(" << lowboundary << ") at end of " << tableName);
            break;
        }

	} while(true);

    ViewCountDataPtr obj = ServiceI::instance().locateObject<ViewCountDataPtr>( VIEW_COUNT, VIEW_INDEX ); 
    if (!obj) {          
        MCE_INFO("[InfoResultCollector] getInfoResultFromWorkplace find no ViewCountDataPtr");
        obj = new ViewCountData();
    }

	LongSeq md5Seq;
	InfoDataSeq dataSeq;
	for(map< long, vector<int> >::iterator iter=elementaryschoolResultMap.begin();
			iter!=elementaryschoolResultMap.end(); ++iter) {
		InfoDataPtr data = new InfoData();

		obj->GetDataOrderedByCount(iter->second);

		data->idSeq.swap( iter->second );
		
		md5Seq.push_back( iter->first );
		dataSeq.push_back( data );
		
		if( (int)md5Seq.size() >= 1000) {
			for(int cluster=begin; cluster<end; ++cluster) {
				try {
					FriendFinderInfoCacheReplicaAdapter::instance().setElementaryschoolDataWithMask( cluster, md5Seq, dataSeq ); 	// 写ElementaryschoolDataCache
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setElementaryschoolDataWithMask Ice Exception : " << e.what() << "\n");
				} catch ( std::exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setElementaryschoolDataWithMask std Exception : " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setElementaryschoolDataWithMask Unknow Exception");
				}
			}
			md5Seq.clear();
			dataSeq.clear();
		}
	}

	if( (int)md5Seq.size() > 0 ) {
		for(int cluster=begin; cluster<end; ++cluster) {
			try {
				FriendFinderInfoCacheReplicaAdapter::instance().setElementaryschoolDataWithMask( cluster, md5Seq, dataSeq ); 	// 写ElementaryschoolDataCache
			} catch ( Ice::Exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setElementaryschoolDataWithMask Ice Exception : " << e.what() << "\n");
			} catch ( std::exception& e ) {
				MCE_WARN(time(NULL) << " [InfoResultCollector] setElementaryschoolDataWithMask std Exception : " << e.what() << "\n");
			} catch ( ... ) {
				MCE_WARN(time(NULL) << "[InfoResultCollector] setElementaryschoolDataWithMask Unknow Exception");
			}
		}
	}
}

void InfoResultCollector::getInfoResultFromBirthday(bool empty) {
	MCE_INFO("[InfoResultCollector] getInfoResultFromBirthday");
	string tableName = "user_born";
    int lowboundary = 0; 
	int last = 0;

	map< int, vector<int> > birthdayResultMap;

	if( empty ) {
		MCE_DEBUG("getInfoResultFromBirthday true");
		return;
	}

	do {
		size_t queryCount = 0;

		Statement sql;       // 查数据库
        sql
            << "SELECT * FROM " << tableName
            << " where id>" << lowboundary
            << " limit 100000";

	   	try {
            queryCount = QueryRunner("user_born_friendfinder", CDbRServer).query( sql, BirthdayBatchReadHandler(last, birthdayResultMap) );
        } catch (Ice::Exception& e) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] getInfoResultFromBirthday" << tableName << " Connection Exception : " << e.what() << "\n");
        } catch ( ... ) {
            MCE_WARN(time(NULL) << " [InfoResultCollector] DB unknow Exception ");
        }

        if(last > lowboundary) {
            lowboundary = last;
        } else if(queryCount <= 0) {
            MCE_INFO("[InfoResultCollector] getInfoResultFromBirthday lowboundary(" << lowboundary << ") at end of " << tableName);
            break;
        }

	} while(true);

	for(map< int, vector<int> >::iterator iter=birthdayResultMap.begin();
			iter!=birthdayResultMap.end(); ++iter) {
		sort(iter->second.begin(), iter->second.end());
	}

	MCE_INFO("birthdayResultMap size : " << birthdayResultMap.size());

	vector<int> tmpResult;
	for(map< int, vector<int> >::iterator iter=birthdayResultMap.begin();
			iter!=birthdayResultMap.end(); ++iter) {
		const vector<int>& item = iter->second;	
		for(vector<int>::const_iterator it = item.begin(); 
				it != item.end(); ++it) {
			if( (int)tmpResult.size() >= 5000 ) {
				for(int cluster=begin; cluster<end; ++cluster) {
					try {
						FriendFinderInfoCacheReplicaAdapter::instance().setBirthdayData( cluster, iter->first, tmpResult); 	// 写BirthdayDataCache
					} catch ( Ice::Exception& e ) {
						MCE_WARN(time(NULL) << " [InfoResultCollector] setBirthdayData Ice Exception : " << e.what() << "\n");
					} catch ( std::exception& e ) {
						MCE_WARN(time(NULL) << "[InfoResultCollector] setBirthdayData std Exception : " << e.what() << "\n");
					} catch ( ... ) {
						MCE_WARN(time(NULL) << "[InfoResultCollector] setBirthdayData Unknow Exception");
					}
				}
				tmpResult.clear();
			}

			tmpResult.push_back(*it);
		}

		if( (int)tmpResult.size() > 0 ) {
			for(int cluster=begin; cluster<end; ++cluster) {
				try {
					FriendFinderInfoCacheReplicaAdapter::instance().setBirthdayData( cluster, iter->first, tmpResult); 	// 写BirthdayDataCache
				} catch ( Ice::Exception& e ) {
					MCE_WARN(time(NULL) << " [InfoResultCollector] setBirthdayData Ice Exception : " << e.what() << "\n");
				} catch ( std::exception& e ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setBirthdayData std Exception : " << e.what() << "\n");
				} catch ( ... ) {
					MCE_WARN(time(NULL) << "[InfoResultCollector] setBirthdayData Unknow Exception");
				}
			}
		}
		tmpResult.clear();
	}

	for(int cluster=begin; cluster<end; ++cluster) {
		try {
			FriendFinderInfoCacheReplicaAdapter::instance().setBirthdayData(cluster, 0, vector<int>()); 	// 输入结束位 
		} catch ( Ice::Exception& e ) {
			MCE_WARN(time(NULL) << " [InfoResultCollector] setBirthdayData Ice Exception : " << e.what() << "\n");
		} catch ( std::exception& e ) {
			MCE_WARN(time(NULL) << "[InfoResultCollector] setBirthdayData std Exception : " << e.what() << "\n");
		} catch ( ... ) {
			MCE_WARN(time(NULL) << "[InfoResultCollector] setBirthdayData Unknow Exception");
		}
	}
}

//**************************************************************************************************************

bool UnivBatchReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];

	vector<string> info;

	char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["enroll_year"]);
		
	string univName( row["university_name"] );
	string department( row["department"] );
	string enrollYear( tmpstr );

	if(univName == "" || department == "" || enrollYear == "") {
		return true;
	}

	//去空格
	trim(univName);
	trim(department);
	trim(enrollYear);

	MCE_DEBUG( "id(" << (int)row["id"] << ")  university_name(" << univName 
				<< ")  department(" << department << ")  enroll_year("  << enrollYear << ")");

	info.push_back( univName );
	info.push_back( department );
	info.push_back( enrollYear );

	long key = MD5Util::stringSeqToMD5(info);
	_resultMap[key].push_back( (int)row["userid"] );
	return true;
}

//**************************************************************************************************************

bool HighschoolBatchReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];

	vector<string> info;

	char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["enroll_year"]);

	string highschoolName( row["high_school_name"] );
	string enrollYear( tmpstr );

	if(highschoolName == "" || enrollYear == "") {
		return true;
	}
	
	trim(highschoolName);
	trim(enrollYear);

	MCE_DEBUG( "id(" << (int)row["id"] << ")  high_school_name(" << highschoolName 
				<< ")  enroll_year("  << enrollYear << ")");
	
	info.push_back( highschoolName );
	info.push_back( enrollYear );

	long key = MD5Util::stringSeqToMD5(info);
	_resultMap[key].push_back( (int)row["userid"] );
	return true;
}

//**************************************************************************************************************

bool JuniorschoolBatchReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["userid"] ? _last : (int)row["userid"];

	vector<string> info;

	char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["junior_high_school_year"]);

	string juniorschoolName( row["junior_high_school_name"] );
	string enrollYear( tmpstr );

	if(juniorschoolName == "" || enrollYear == "") {
		return true;
	}
	
	trim(juniorschoolName);
	trim(enrollYear);
	MCE_DEBUG( "userid(" << (int)row["userid"] << ")  junior_high_school_name(" << juniorschoolName 
				<< ")  enroll_year("  << enrollYear << ")");

	info.push_back( juniorschoolName );
	info.push_back( enrollYear );

	long key = MD5Util::stringSeqToMD5(info);
	_resultMap[key].push_back( (int)row["userid"] );
	return true;
}

//**************************************************************************************************************

bool ElementaryschoolBatchReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["userid"] ? _last : (int)row["userid"];

	vector<string> info;

	char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["elementary_school_year"]);

	string elementaryschoolName( row["elementary_school_name"] );
	string enrollYear( tmpstr );

	if(elementaryschoolName == "" || enrollYear == "") {
		return true;
	}
	
	trim(elementaryschoolName);
	trim(enrollYear);
	MCE_DEBUG( "userid(" << (int)row["userid"] << ")  elementary_school_name(" << elementaryschoolName 
				<< ")  enroll_year("  << enrollYear << ")");

	info.push_back( elementaryschoolName );
	info.push_back( enrollYear );

	long key = MD5Util::stringSeqToMD5(info);
	_resultMap[key].push_back( (int)row["userid"] );
	return true;
}

//**************************************************************************************************************

bool WorkplaceBatchReadHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];

	vector<string> info;

	string workplaceName( row["workplace_name"] );

	if(workplaceName == "") {
		return true;
	}
	
	trim(workplaceName);
	MCE_DEBUG( "id(" << (int)row["id"] << ")  workplace_name(" << workplaceName << ")" );

	info.push_back( workplaceName );

	long key = MD5Util::stringSeqToMD5(info);
	_resultMap[key].push_back( (int)row["userid"] );
	return true;
}

//**************************************************************************************************************

bool BirthdayBatchReadHandler::handle(mysqlpp::Row& row) const {
	if (row["id"] && row["birth_year"]) {
		_last = _last > (int)row["id"] ? _last : (int)row["id"];

		int userId = (int)row["id"]; 
		int birthYear = (int)row["birth_year"];

		_resultMap[birthYear].push_back( userId );
	}

	return true;
} 

//**************************************************************************************************************

void LoadUnivTask::handle() {
	InfoResultCollector::getInfoResultFromUniv();
	FriendFinderInfoWorkerI::instance().g_univ_task = 1;
	MCE_DEBUG("[LoadUnivTask] handle g_univ_task = " << FriendFinderInfoWorkerI::instance().g_univ_task);
}

//**************************************************************************************************************

void LoadHighschoolTask::handle() {
	InfoResultCollector::getInfoResultFromHighschool();
	FriendFinderInfoWorkerI::instance().g_highschool_task = 1;
	MCE_DEBUG("[LoadHighschoolTask] handle g_highschool_task = " << FriendFinderInfoWorkerI::instance().g_highschool_task);
}

//**************************************************************************************************************

void LoadJuniorschollTask::handle() {
	InfoResultCollector::getInfoResultFromJuniorschool();
	FriendFinderInfoWorkerI::instance().g_juniorschool_task = 1;
	MCE_DEBUG("[LoadJuniorschollTask] handle g_juniorschool_task = " << FriendFinderInfoWorkerI::instance().g_juniorschool_task);
}

//**************************************************************************************************************

void LoadElementschoolTask::handle() {
	InfoResultCollector::getInfoResultFromElementschool();
	FriendFinderInfoWorkerI::instance().g_elementaryschool_task = 1;
	MCE_DEBUG("[LoadElementschoolTask] handle g_elementaryschool_task = " << FriendFinderInfoWorkerI::instance().g_elementaryschool_task);
}

//**************************************************************************************************************

void LoadWorkplaceTask::handle() {
	InfoResultCollector::getInfoResultFromWorkplace();
	FriendFinderInfoWorkerI::instance().g_workplace_task = 1;
	MCE_DEBUG("[LoadWorkplaceTask] handle g_workplace_task = " << FriendFinderInfoWorkerI::instance().g_workplace_task);
}

//**************************************************************************************************************

void LoadBirthdayTask::handle() {
	InfoResultCollector::getInfoResultFromBirthday();
	FriendFinderInfoWorkerI::instance().g_birthday_task = 1;
	MCE_DEBUG("[LoadBirthdayTask] handle g_birthday_task = " << FriendFinderInfoWorkerI::instance().g_birthday_task);
}
