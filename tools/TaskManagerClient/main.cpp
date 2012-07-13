#include <iostream>
#include <sstream>
#include <Ice/Ice.h>
#include <TaskManagerAPI.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace Ice;
using namespace xce::svcapi;

void help() {
  cerr << "\tclear <level>" << endl;
  cerr << "\tsize <level>" << endl;
  cerr << "\tconfig <level> <min> <max> <stack>" << endl;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cerr << "tmcli <endpoint> <command> ..." << endl;
    return -1;
  }
  try {
    Ice::PropertiesPtr properties = Ice::createProperties();
    properties->load("../etc/admin.cfg");
    InitializationData initializationData;
    initializationData.properties = properties;
    CommunicatorPtr communicator = initialize(initializationData);

  	ObjectPrx object = communicator->stringToProxy("TMA@"+string(argv[1]));
	  TaskManagerAPIPrx manager = TaskManagerAPIPrx::uncheckedCast(object);
    if (string(argv[2]) == "clear") {
      if (argc != 4) {
        help();
        return -1;
      }
      int level = boost::lexical_cast<int>(argv[3]);
      manager->clear(level);
    } else if (string(argv[2]) == "size") {
      if (argc != 4) {
        help();
        return -1;
      }
      int level = boost::lexical_cast<int>(argv[3]);
      cout << manager->size(level) << endl;
    } else if (string(argv[2]) == "config") {
      if (argc != 7) {
        help();
        return -1;
      }
      int level = boost::lexical_cast<int>(argv[3]);
      int min = boost::lexical_cast<int>(argv[4]);
      int max = boost::lexical_cast<int>(argv[5]);
      int stack = boost::lexical_cast<int>(argv[6]);
      manager->config(level, min, max, stack);
    } else {
      help();
    }
  } catch (const Ice::Exception& e) {
    cerr << e.what() << endl;
    return -1;
  } catch (...) {
    cerr << "unknown exception" << endl;
    return -1;
  }
  
	return 0;
}

