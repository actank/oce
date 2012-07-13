#include "PageCacheAdapter.h"
#include <boost/lexical_cast.hpp>

using namespace talk::http;
using namespace boost;

void help()
{
        cout << "Usage: LoginRecordWorker [OPTIONS]" << endl;
        cout << "-h, --help\tDisplay this help and exit." << endl;
        cout << "-i, --index\tThe operating index from 0 to 9." << endl;
        cout << "-r, --read\tRead sign of some Index." << endl;
        cout << "-w, --write\tWrite sign of some Index. Need value(0 or 1 )." << endl;
}

int main (int argc, char * const argv[]) {
        if (argc<2)
        {
                help();
                return 1;
        }

	int index = -1;
	int value = -1;
	bool r_ = false;
	bool w_ = false;

#define OPT(_a1, _a2)   else if ( !strcmp(argv[i], _a1) || !strcmp(argv[i], _a2) )
        int i;
        for (i=1; i<argc; i++)
        {
                if ( i == 0);
                OPT("-h", "--help")     	{ help(); return 0; }
                OPT("-r", "--read")     	{ r_ = true;}
                else if (i + 1 == argc) break;
                OPT("-i", "--index")		{ index = boost::lexical_cast<int>(argv[++i]); }
                OPT("-w", "--write")		{ w_ = true; value = boost::lexical_cast<int>(argv[++i]); }
                else break;
        }
#undef OPT

        if (i!=argc)
        {
                cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
		help();
                return 1;
        }

	cout << "[main] starting task. index=" << index << " value=" << value << " r_=" << r_ << " w_=" << w_ << endl;

	if ( r_ && index>=0 && index<=9 ){
		int sign = PageCacheAdapter::instance().GetVideoViewPower(index);
		cout << "Read sign for index " << index << " : " << sign << endl;
	}

	if ( w_ && index>=0 && index<=9 && (value==0||value==1) ){
		int signold = PageCacheAdapter::instance().GetVideoViewPower(index);
		PageCacheAdapter::instance().SetVideoViewPower(index,value);
		int signnew = PageCacheAdapter::instance().GetVideoViewPower(index);
		cout << "Write sign for index " << index << " : from " << signold << " to " << signnew << endl;
	}

	cout << "[main] finished task." << endl;
	return 0;
}

