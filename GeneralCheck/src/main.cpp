#include "CheckManager.h"
#include "CheckHandlerFactory.h"
#include "IdProducer.h"
#include "CheckHandler.h"
#include "check/CheckDataHandler.h"
#include "check/CheckDataManager.h"
#include "interface/GetDataHandler.h"
#include "factory/GetDataHandlerFactory.h"
#include "ConnectionPoolManager.h"
#include "interface/CompareHandler.h"
#include "factory/CompareHandlerFactory.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace xce::generalcheck;
using namespace com::xiaonei::xce;

int main( int argc, char** argv ) {
	if(argc == 1) {
		cout << "Must input at least one parameter to use!" << endl;
		return 0;
	}
	if(argc == 2) {
		CheckHandlerPtr checkHandler = CheckHandlerFactory::getHandler( argv[1] );
		if(!checkHandler) {
			cout << "No " << argv[1] << " such CheckHandler!" << endl;
			return 0;
		}

		cout << "debug Check start!" << endl;	
		ConnectionPoolManager::instance().initialize();
		CheckManagerPtr checkManager = new CheckManager( checkHandler, new BatchUsersIdProducer );
		timeval tB;
		gettimeofday( &tB, NULL );
		checkManager->check();
		timeval tE;
		gettimeofday( &tE, NULL );
		cout << "debug Check finish, cost " << ( tE.tv_sec - tB.tv_sec ) / 60 << " minutes " << ( tE.tv_sec - tB.tv_sec ) % 60 << " seconds!" << endl;
	} else {
		bool is_status_compare = false;
		string str_argv1(argv[1]);
		string str_argv2(argv[2]);
		if(str_argv1.compare("StatusCache") == 0 || str_argv1.compare("StatusCache") == 0 || str_argv2.compare("StatusBuddyByIdLoader") == 0 
			|| str_argv2.compare("StatusBuddyByIdLoader") == 0) {
			is_status_compare = true;
		}
		GetDataHandlerPtr getSourceHandler = GetDataHandlerFactory::getHandler(argv[1]);
		if(getSourceHandler == 0) {
			cout << "No " << argv[1] << " such GetDataHandler!" << endl;
			return 1;
		}
		vector<GetDataHandlerPtr> getDesHandler;
		for(int i = 2; i < argc - 1; ++i) {
			GetDataHandlerPtr getTempDesHandler = GetDataHandlerFactory::getHandler(argv[i]);
			if(getTempDesHandler == 0) {
				cout << "No " << argv[1] << " such GetDataHandler!" << endl;
			} else {
				getDesHandler.push_back(getTempDesHandler);
			}		
		}
		if(getDesHandler.size() == 0) {
			cout << "No suitable getDesHandler GetDataHandler!" << endl;
			return 1;
		}
		CompareHandlerPtr compareHandler = CompareHandlerFactory::getHandler(argv[argc - 1]);
		if(compareHandler == 0) {
			cout << "No " << argv[argc - 1] << " such CompareHandler!" << endl;
			return 1;
		}	
		MCE_INIT("./check.log", "INFO");
		if(is_status_compare) {
			cout << "Starting to compare cache status data" << endl;
			MyUtil::ObjectMap status = getSourceHandler->getData(MyUtil::LongSeq());
			cout << "get data from " << argv[1] << " finished" << endl;
			MyUtil::ObjectMap status_from_loader = getDesHandler[0]->getData(MyUtil::LongSeq());
			cout << "get data from " << argv[2] << " finished" << endl;
			compareHandler->compare(status, status_from_loader, getSourceHandler->name_, getDesHandler[0]->name_);
			return 0;
		}
		CheckDataHandlerPtr checkDataHandler = new CheckDataHandler(getSourceHandler, getDesHandler, compareHandler, 10, 10, 4, 300);
		cout << "debug CheckData start!" << endl;	
		ConnectionPoolManager::instance().initialize();
		CheckDataManagerPtr checkDataManager = new CheckDataManager(checkDataHandler, new BatchUsersIdProducer);
		timeval tB;
		gettimeofday( &tB, NULL );
		checkDataManager->check();
		timeval tE;
		gettimeofday( &tE, NULL );
		cout << "debug CheckData finish, cost " << ( tE.tv_sec - tB.tv_sec ) / 60 << " minutes " << ( tE.tv_sec - tB.tv_sec ) % 60 << " seconds!" << endl;
	}
	return 0;
}


