#include <Ice/Ice.h>
#include <Tour.h>

using namespace std;
using namespace com::xiaonei::service::tour;

class TourClient : public Ice::Application {
public:
	virtual int run(int, char *[]);
	virtual void interruptCallback(int);
};

int main(int argc, char* argv[]) {
	TourClient app;
	return app.main(argc, argv, "etc/node.config");
}

int TourClient::run(int argc, char* argv[]) {
	callbackOnInterrupt();
	
		CalcPrx calc = CalcPrx::checkedCast(
				communicator()->stringToProxy("O@A")->ice_twoway()->ice_timeout(-1)->ice_secure(false));
		if (!calc) {
				cerr << argv[0] << ": invalid proxy" << endl;
				return EXIT_FAILURE;
		}
		cout << "calc->op1(2, 3): " << calc->op1(2, 3) <<endl;
		cout << "calc->op2(2, 3): " << calc->op2(2, 3) << endl;

		return EXIT_SUCCESS;
}

void
TourClient::interruptCallback(int)
{
    try
    {
        communicator()->destroy();
    }
    catch(const IceUtil::Exception& ex)
    {
        cerr << appName() << ": " << ex << endl;
    }
    catch(...)
    {
        cerr << appName() << ": unknown exception" << endl;
    }
    exit(EXIT_SUCCESS);
}
