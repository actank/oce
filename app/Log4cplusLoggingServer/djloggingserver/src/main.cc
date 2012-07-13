#include "djhandler.h"

void help()
{
        cout << "Usage: djloggingserver.create_time [OPTIONS]" << endl;
        cout << "-h, --help\tDisplay this help and exit." << endl;
        cout << "-p, --port\tBind to TCP-port. ( e.g. \"8080\" )" << endl;
        cout << "-f, --file\tName of local logging file. ( e.g. \"/data/logs/click/click.log\" )" << endl;
        cout << "-s, --size\tMaximal size(Bytes) of output file. ( 10*1024*1024 by default )" << endl;
        cout << "-i, --index\tNumber of backup output file. ( \"10000\" by default)" << endl;
}

int main (int argc, char * const argv[]) {
        if (argc<2)
        {
                help();
                return 1;
        }

        int port = -1;
        string path = "";
	long max_file_size = 10*1024*1024;
	int max_backup_index = 10000;

#define OPT(_a1, _a2)   else if ( !strcmp(argv[i], _a1) || !strcmp(argv[i], _a2) )
        int i;
        for (i=1; i<argc; i++)
        {
                if ( i == 0);
                OPT("-h", "--help")     {help(); return 0;}
                else if (i + 1 == argc) break;
                OPT("-p", "--port")	port = boost::lexical_cast<int>( argv[++i] );
                OPT("-f", "--file")	path = argv[++i];
                OPT("-s", "--size")	max_file_size = boost::lexical_cast<long>( argv[++i] );
                OPT("-i", "--index")	max_backup_index = boost::lexical_cast<int>( argv[++i] );
                else break;
        }
#undef OPT
        if (i!=argc)
        {
                cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
                return 1;
        }

	if (port==-1) {
		cout << "Need TCP-port. ( e.g. \"8081\" )" << endl;
		return 1;
	} else if(path=="") {
		cout << "Need name of local logging file. ( e.g. \"/data/logs/click/click.log\" )" << endl;
		return 1;
	} else{
		cout << "[main] starting server." << endl;
		logging::SocketLoggingServer server( port );
		server.Start( DjHandler(path,max_file_size,max_backup_index,"INFO") );
		cout << "[main] server stoped." << endl;
		return 0;
	}
}

