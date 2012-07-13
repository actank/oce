#include <Ice/Ice.h>
#include <ObjectCacheDemo.h>
#include <iostream>

using namespace std;
using namespace xce::demos;

void usage(string exe){
	cout << "Usage:" << endl;
	cout << "--------------------------------" << endl;
	cout << exe << " -load : load 1000000 objects" << endl;
	cout << exe << " -preload : preload 1000000 objects" << endl;
	cout << exe << " -reload : reload 1000000 objects" << endl;
	cout << exe << " -get : get sample email of objects" << endl;
	cout << exe << " -get id : get email by id, id should be an integer" << endl;
	cout << exe << " -set id email : set email of id, id should be an integer" << endl;
	cout << exe << " -touch : touch sample email of objects" << endl;
	cout << exe << " -touch id : touch email of id, id should be an integer" << endl;
}

int main(int argc, char * argv[]) {
	int status = 0;
	Ice::CommunicatorPtr ic;
	try {
		ic = Ice::initialize(argc, argv);
		Ice::ObjectPrx base = ic->stringToProxy("Object:default -p 10044");
		ObjectCacheDemoPrx object = ObjectCacheDemoPrx::checkedCast(base);
		if (!object)
			throw "Invalid proxy";
		if(argc == 2){
			string command(argv[1]);
			if(command == "-load"){
				object->load();
			}else if(command == "-preload"){
				object->preload();
			}else if(command == "-reload"){
				object->reload();
			}else if(command == "-get"){
				cout << "map size before get: " << object->size() << endl;
				for(long id = 100000; id <= 1000000; id += 100000){
					cout << "email of " << id << " : " << object->getEmail(id) << endl;
				}
				cout << "map size after get: " << object->size() << endl;
			}else if(command == "-touch"){
				cout << "map size before touch : " << object->size() << endl;
				for(long id = 100000; id <= 1000000; id += 100000){
					cout << "email of " << id << " : " << object->touchEmail(id) << endl;
				}
				cout << "map size after touch : " << object->size() << endl;
			}else{
				string exe(argv[0]);
				usage(exe);
			}
		}else if(argc == 3){
			string command(argv[1]);
			if(command == "-get"){
				int id = atoi(argv[2]);
				cout << "email of " << id << " : " << object->getEmail(id) << endl;
			}else if(command == "-touch"){
				int id = atoi(argv[2]);
				cout << "email of " << id << " : " << object->touchEmail(id) << endl;
			}else{
				string exe(argv[0]);
				usage(exe);
			}
		}else if(argc == 4){
			string command(argv[1]);
			if(command == "-set"){
				int id = atoi(argv[2]);
				string email(argv[3]);
				object->setEmail(id, email);
			}else{
				string exe(argv[0]);
				usage(exe);
			}
		}else{
			string exe(argv[0]);
			usage(exe);
		}
	} catch (const Ice::Exception & ex) {
		cerr << ex << endl;
		status = 1;
	} catch (const char * msg) {
		cerr << msg << endl;
		status = 1;
	}
	if (ic) {
		try {
			ic->destroy();
		} catch (const Ice::Exception & ex) {
			cerr << ex << endl;
			status = 1;
		}
	}
	return status;
}
