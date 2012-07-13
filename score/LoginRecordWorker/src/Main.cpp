#include "LoginRecordWorker.h"
using namespace com::xiaonei::xce;

void help()
{
        cout << "Usage: LoginRecordWorker [OPTIONS]" << endl;
        cout << "-h, --help\tDisplay this help and exit." << endl;
        cout << "-r, --record\tRecord the stat result to DB." << endl;
        cout << "-o, --orange\tOrange verify. Find wrong orange setting. (follow \"verify\" to print only,\"correct\" to correct them)" << endl;
        cout << "-w, --writefile\tWrite into file the id who's orange. Need file name to record results. ( \"/data/xce/ids.txt\" etc. )" << endl;
}

int main (int argc, char * const argv[]) {
        if (argc<2)
        {
                help();
                return 1;
        }

	bool r_ = false;
	bool o_ = false;
	bool oc_ = false;
        string orange_check_type = "";
	bool w_ = false;
        string orange_record_file = "";

#define OPT(_a1, _a2)   else if ( !strcmp(argv[i], _a1) || !strcmp(argv[i], _a2) )
        int i;
        for (i=1; i<argc; i++)
        {
                if ( i == 0);
                OPT("-h", "--help")     	{help(); return 0;}
                OPT("-r", "--record")     	{r_ = true;}
                else if (i + 1 == argc) break;
                OPT("-o", "--orange")		{o_ = true; orange_check_type = argv[++i];}
                OPT("-w", "--writefile")	{w_ = true; orange_record_file = argv[++i];}
                else break;
        }
#undef OPT

        if (i!=argc)
        {
                cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
		help();
                return 1;
        }

	if ( o_ ){
		if( orange_check_type=="verify" ){
			cout << "Need award record file Name." << endl;
			oc_ = false;
		} else if( orange_check_type=="correct" ){
			cout << "Need award record file Name." << endl;
			oc_ = true;
		} else{
			cout << "Need orange check type(\"verify\" or \"correct\")." << endl;
			return 1;
		}
	}

	if ( w_ && orange_record_file=="" )
	{
		cout << "Need orange record file Name." << endl;
		return 1;
	}

	cout << "[main] starting task." << endl;
	LoginRecordWorker worker;
	worker.work(o_,oc_,r_,w_,orange_record_file);
	cout << "[main] finished task." << endl;
	return 0;
}

