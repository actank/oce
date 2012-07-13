#include "IPCollector.h"
using namespace com::xiaonei::xce;

void help()
{
        cout << "Usage: IPCollector [OPTIONS]" << endl;
        cout << "-h, --help\tDisplay this help and exit." << endl;
        cout << "-c, --call\tFunction Name. ( \"FBI\" or \"IR\" )" << endl;
        cout << "-d, --date\tDate of ip_log_xxx for scanning. ( e.g. \"20091128\". scanning yesterday by default )" << endl;
}

int main (int argc, char * const argv[]) {
        string function_name = "";
        string date = "";
        if (argc<2)
        {
                help();
                return 1;
        }

#define OPT(_a1, _a2)   else if ( !strcmp(argv[i], _a1) || !strcmp(argv[i], _a2) )
        int i;
        for (i=1; i<argc; i++)
        {
                if ( i == 0);
                OPT("-h", "--help")     {help(); return 0;}
                else if (i + 1 == argc) break;
                OPT("-c", "--call")	function_name = argv[++i];
                OPT("-d", "--date")	date = argv[++i];
                else break;
        }
#undef OPT

        if (i!=argc)
        {
                cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
                return 1;
        }

	if (function_name=="")
	{
		cout << "Need Function Name." << endl;
		return 1;
	} else if(function_name=="FBI"){
		cout << "[main] starting Collection." << endl;
		IPCollector collector( date );
		collector.collectForFBI();
		cout << "[main] finished Collection." << endl;
		return 0;
	} else if(function_name=="IR"){
		cout << "[main] starting Collection." << endl;
		IPCollector collector( date );
		collector.collectForIR();
		cout << "[main] finished Collection." << endl;
		return 0;
	} else{
		cout << "Need Function Name: only \"FBI\" and \"IR\" supported." << endl;
		return 1;
	}
}

