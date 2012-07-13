#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "FriendFinderInfoWorkerI.h"

using namespace std;
using namespace xce::friendfinder;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::serverstate;

bool IsBigger(const pair<int, int>& s1, const pair<int, int>& s2) {
	return s1.second > s2.second;
}

void MyUtil::initialize() {
    ServiceI& service = ServiceI::instance();

	service.registerObjectCache(VIEW_COUNT, "VC", new ViewCountFactory);

    service.getAdapter()->add( &FriendFinderInfoWorkerI::instance(), 
			service.createIdentity("FFIW", "") );   //改endpoint

	TaskManager::instance().config(TASK_WORKPLACE, ThreadPoolConfig(1, 3));
	TaskManager::instance().config(TASK_UNIV, ThreadPoolConfig(1, 3));
	TaskManager::instance().config(TASK_HIGHSCHOOL, ThreadPoolConfig(1, 3));
	TaskManager::instance().config(TASK_JUNIORSCHOOL, ThreadPoolConfig(1, 3));
	TaskManager::instance().config(TASK_ELEMENTARYSCHOOL, ThreadPoolConfig(1, 3));
	TaskManager::instance().config(TASK_BIRTHDAYDATA, ThreadPoolConfig(1, 3));

	ConnectionPoolManager::instance().initialize();        //连接数据库
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderInfoWorkerReplica.Mod");       
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderInfoWorkerReplica.Interval", 5);
	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerFriendFinderInfoWorker", &FriendFinderInfoWorkerI::instance(), mod, sInterval);
}

//*************************************************************************************************

void FriendFinderInfoWorkerI::createDataFromDB(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("[FriendFinderInfoWorkerI] createDataFromDB userId(" << userId << ")");
	g_birthday_task = 0;       //初始化变量
	g_elementaryschool_task = 0;
	g_juniorschool_task = 0;
	g_highschool_task = 0;
	g_univ_task = 0;
	g_workplace_task = 0;

	CreateViewCountData();   //加入人气排序
	InfoResultCollector::createInfoResultBatch(userId);  //加载NetWork信息
	
	while (1) {
		sleep(60);
		MCE_DEBUG("g_birthday_task (" << g_birthday_task << ") g_elementaryschool_task(" << g_elementaryschool_task<< ") g_juniorschool_task("
			<< g_juniorschool_task << ") g_highschool_task(" << g_highschool_task << ") g_univ_task(" << g_univ_task << ")");

		if (g_birthday_task == 1 && g_elementaryschool_task == 1 &&
			g_juniorschool_task == 1 && g_highschool_task == 1 &&
			g_univ_task == 1) {
			MCE_INFO("[FriendFinderInfoWorkerI] createInfoResultBatch finish loading restarted!");
			break;
		}
	}

	restartControl(Ice::Current());
}

void FriendFinderInfoWorkerI::restartControl(const Ice::Current& current) {            //重启服务，释放内存
	MCE_INFO("[FriendFinderInfoWorkerI] restarted !");
	assert(false);
}

void FriendFinderInfoWorkerI::CreateViewCountData() {
	MCE_INFO("[FriendFinderInfoWorkerI] CreateViewCountData");

	string tableName = "view_count";
	int lowboundary = 0;
	int last = 0;

	map<int, int> viewCountMap;	

	do {
		size_t queryCount = 0;

		Statement sql;
		sql 
			<< "SELECT * FROM " << tableName
			<< " WHERE id>" << lowboundary
			<< " LIMIT 100000";

		try {
			queryCount = QueryRunner("user_view_count_preloader", CDbRServer).query( sql, ViewCountHandler(last, viewCountMap) );
		} catch (Ice::Exception& e) {
			MCE_WARN(time(NULL) << "[FriendFinderInfoWorkerI] createDataFromDB " << tableName << "Ice Exception : " << e.what() << "\n");
		} catch (...) {
			MCE_WARN(time(NULL) << "[FriendFinderInfoWorkerI] DB unknow Exception");
		}

		if (last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			MCE_INFO("[FriendFinderInfoWorkerI] createDataFromDB lowboundary(" << lowboundary << "at the end of " << tableName);
			break;
		}
	} while(true);

	MCE_INFO("viewCountMap size : " << viewCountMap.size());

	ViewCountDataPtr obj = new ViewCountData();

	map<int, int> tmpResult;
	for (map<int, int>::iterator iter = viewCountMap.begin();
			iter != viewCountMap.end(); ++iter) {
		if ((int)tmpResult.size() > 10000) {
			obj->SetData(tmpResult);
			tmpResult.clear();
		}

		tmpResult.insert( make_pair<int, int>(iter->first, iter->second) );
	}
	if (!tmpResult.empty()) {
		obj->SetData(tmpResult);
	}
	ServiceI::instance().addObject( obj, VIEW_COUNT, VIEW_INDEX );		
}

//*************************************************************************************************

void ViewCountData::SetData(const std::map<int, int>& viewCountMap) {
	MCE_INFO("[ViewCountData] SetData map size : " << viewCountMap.size());
	IceUtil::RWRecMutex::WLock lock(_init_flag_mutex);
	for(map<int, int>::const_iterator iter = viewCountMap.begin();
			iter != viewCountMap.end(); ++iter) {
		_viewCountMap.insert(make_pair<int, int>(iter->first, iter->second));
	}
}

void ViewCountData::GetDataOrderedByCount(std::vector<int>& idSeq) {
	MCE_INFO("[ViewCountData] GetDataOrderedByCount idSeq size : " << idSeq.size());
	IceUtil::RWRecMutex::RLock lock(_init_flag_mutex);
	vector< pair<int, int> > fr;
	for (vector<int>::iterator iter = idSeq.begin();
			iter != idSeq.end(); ++iter) {
		map<int, int>::iterator fIt = _viewCountMap.find(*iter);
		if (fIt != _viewCountMap.end()) {
			fr.push_back( make_pair<int, int>(fIt->first, fIt->second) );
		} else {
			fr.push_back( make_pair<int, int>(*iter, 0) );
		}
	}
	sort(fr.begin(), fr.end(), IsBigger);
	
	idSeq.clear();

	for (vector< pair<int, int> >::iterator iter = fr.begin();
			iter != fr.end(); ++iter) {
		idSeq.push_back(iter->first);
	}
}

//*************************************************************************************************

bool ViewCountHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int)row["id"] ? _last : (int)row["id"];
	
	int userId = (int)row["id"];
	int viewCount = (int)row["viewcount"];

	_viewCountMap.insert( make_pair<int, int>(userId, viewCount) );

	return true;
}

//*************************************************************************************************

Ice::ObjectPtr ViewCountFactory::create(::Ice::Int id) {
	return new ViewCountData();
}
