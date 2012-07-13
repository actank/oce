#include <Ice/Ice.h>
//#include <training/ObjectCacheDemo/impl/ObjectCacheDemoI.h>
#include "../impl/ObjectCacheDemoI.h"
int main(int argc, char* argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter
            = ic->createObjectAdapterWithEndpoints("SimplePrinterAdapter", "default -p 10044");
        ObjectCacheDemoPtr object;
        if(argc == 1){
        	object = new ObjectCacheDemoI(adapter, 1000000);
        }else if(argc == 2){
        	object = new ObjectCacheDemoI(adapter, atoi(argv[1]));
		}else{
			cout << argv[0] << " : create objects with default capacity (1000000) " << endl;
			cout << argv[0] << " size : create objects with giving capacity" << endl;
			return 1;
		}
        adapter->add(object,ic->stringToIdentity("Object"));
        adapter->activate();
        ic->waitForShutdown();
    } catch (const Ice::Exception & e) {
        cerr << e << endl;
        status = 1;
    } catch (const char * msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception & e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}


