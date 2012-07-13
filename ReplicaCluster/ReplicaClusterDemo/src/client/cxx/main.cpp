#include "ClientInterface.h"
#include "ReplicaClusterDemo.h"
#include <time.h> 

using namespace std;
using namespace MyUtil;
using namespace xce::clusterstate;
using namespace xce::replicaclusterdemo;

int main() {

	int cluster = 3;
	ClientInterface<DemoManagerPrx> _cl("ReplicaClusterDemo", "M", cluster, 10, "ControllerDemo");

	while ( true ) {
		for ( size_t i = 0; i < cluster; i++ ) {
			time_t t = time(0);
			char tmp[64]; 
			strftime(tmp, sizeof(tmp), "%X",localtime(&t) ); 
			
			try {
				cout << tmp << "\t" << i << " : " << _cl.getProxy(i)->PrintServer() << endl;
			} catch (...) {
				cout << tmp << "\t" << i << " : Proxy Exception " << i << endl;
			}
		}
		cout << endl;
		sleep(10);
	}
}
