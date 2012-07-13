#include "FriendFinderNewComerCollector.h"
using namespace com::xiaonei::xce;

void help()
{
        cout << "Usage: FriendFinderNewComerCollector [OPTIONS]" << endl;
        cout << "-h, --help\tDisplay this help and exit." << endl;
        cout << "-u, --user\tDeploy 1 user id. ( need user id : \"24496255\" etc. )" << endl;
        cout << "-d, --days\tDeploy all users from days ago. ( need int type days : \"30\" etc. )" << endl;
}

int main (int argc, char * const argv[]) {
        string user_id = "";
        string in_days = "";
	bool has_user = false;
	bool has_days = false;
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
                OPT("-u", "--user")	{has_user=true; user_id=argv[++i];}
                OPT("-d", "--days")	{has_days=true; in_days=argv[++i];}
                else break;
        }
#undef OPT

        if (i!=argc)
        {
                cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
                return 1;
        }

        int userId = 0;
        int inDays = 0;

	if ( has_user && has_days ){
		cout << "Need user or days option. Choose one" << endl;
		help();
		return 1;
	} else if ( !has_user && !has_days ){
		cout << "Need user or days option." << endl;
		help();
		return 1;
	} else if( has_user ){
		try{
			userId = boost::lexical_cast<int>( user_id );
		}catch( ... ){
			cout << "Unreconized user id: " << user_id << endl;
			return 1;
		}
		cout << "[main] starting Collection. userId=" << userId << endl;
		FriendFinderNewComerCollector collector;
		collector.deployByUser(userId);
		cout << "[main] finished Collection." << endl;
		return 0;
	} else{
		try{
			inDays = boost::lexical_cast<int>( in_days );
		}catch( ... ){
			cout << "Unreconized days option: " << in_days << endl;
			return 1;
		}
		cout << "[main] starting Collection. inDays=" << inDays << endl;
		FriendFinderNewComerCollector collector;
		collector.deployByDays(inDays);
		cout << "[main] finished Collection." << endl;
		return 0;
	}
}

