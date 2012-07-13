#include <iostream>
#include <ServiceI.h>
#include <ReplicaCluster.h>

using namespace std;
using namespace MyUtil;
using namespace Ice;
using namespace xce::serverstate;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cerr << "hastatuscli <endpoint> <command> ..." << endl;
    return -1;
  }

  Ice::PropertiesPtr properties = Ice::createProperties();
  properties->load("../etc/admin.cfg");
  InitializationData initializationData;
  initializationData.properties = properties;
  CommunicatorPtr communicator = initialize(initializationData);

  ObjectPrx object = communicator->stringToProxy("SSBD@"+string(argv[1]));
  ServerStateBackDoorPrx manager = ServerStateBackDoorPrx::uncheckedCast(object);

  if (string(argv[2]) == "set") {
    map<string, vector<string> > m;
    vector<string> v;
    switch (argc) {
      case 4:
        v.push_back(string(argv[3]));
        m.insert(make_pair<string, vector<string> >("setstatus", v));
        break;
      case 5:
        v.push_back(string(argv[3]));
        v.push_back(string(argv[4]));
        m.insert(make_pair<string, vector<string> >("setstatus", v));
        break;
      default:
        cerr << "number of argv is wrong" << endl;
        return -1;
    }
    manager->control(m);
  } else if (string(argv[2]) == "get") {
    map<string, vector<string> > m;
    vector<string> v;
    switch (argc) {
      case 3:
        m.insert(make_pair<string, vector<string> >("getstatus", vector<string>()));
        break;
      case 4:
        v.push_back(argv[3]);
        m.insert(make_pair<string, vector<string> >("getstatus", v));
        break;
    }
    map<string, string> r =  manager->control(m);
    for (map<string, string>::iterator i = r.begin(); i != r.end(); ++i) {
      cout << i->first << " " << i->second << endl;
    }
  }
  return 0;
}

