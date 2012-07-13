#include "UnivLinkFriends.h"
using namespace com::xiaonei::xce;

void help()
{
        cout << "Usage: UnivLinkFriends [OPTIONS]" << endl;
        cout << "-h, --help\tDisplay this help and exit." << endl;
        cout << "-f, --file\tFile for output.(\"/mnt/dsk11/sg/PREFIX_\" .etc)" << endl;
}

int main (int argc, char * const argv[]) {
        string fileNamePatt = "";
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
                OPT("-f", "--file")	fileNamePatt = argv[++i];
                else break;
        }
#undef OPT

        if (i!=argc)
        {
                cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
                return 1;
        }

	if (fileNamePatt=="")
	{
		cout << "Need File Name." << endl;
		return 1;
	} else{
		cout << "[main] starting Collection. file: " << fileNamePatt << endl;
		UnivLinkFriends collector( fileNamePatt );
		collector.collectUnivData();
		cout << "[main] finished Collection." << endl;
		return 0;
	}
}

