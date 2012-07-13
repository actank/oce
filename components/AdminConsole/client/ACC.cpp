#include <iostream>
#include <sstream>
#include <Ice/Ice.h>
#include <AdminConsole.h>
#include <readline/history.h>
#include <readline/readline.h>

using namespace std;
using namespace Ice;
using namespace xce::adminconsole;

typedef struct {
	char *name; 
	rl_icpfunc_t *func;
	char *doc;
} COMMAND;

/*COMMAND commands[] = {
  { "quit", com_quit, "Quit using Fileman" },
  { "exit", com_exit, "Rename FILE to NEWNAME" },
  { "stat", com_stat, "Print out statistics on FILE" },
  { "view", com_view, "View the contents of FILE" },
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};*/
/*
char * dupstr(int s) {
	char *r;
	r = xmalloc (strlen (s) + 1);
	strcpy (r, s);
	return (r);
}


char *command_generator (const char *text, int state)
{
	static int list_index, len;
	char *name;

	if (!state) {
		list_index = 0;
		len = strlen (text);
	}

	while (name = commands[list_index].name) {
		list_index++;
		if (strncmp (name, text, len) == 0) {
			return (dupstr(name));
		}
	}
	return ((char *)NULL);
}


char **fileman_completion (const char *text, int start, int end)
{
	char **matches;
	matches = (char **)NULL;
	if (start == 0) {
		matches = rl_completion_matches (text, command_generator);
	}
	return (matches);
}*/


void initialize_readline ()
{
	rl_readline_name = ">>>";
//	rl_attempted_completion_function = fileman_completion;
}

void help()
{
	cout << "Usage: ACC [OPTIONS]" << endl << endl;
	cout << "-?, --help\tDisplay this help and exit." << endl;
	cout << "-I, --help\tSynonym for -" << endl;
	cout << "-o, --object\tObject name for service." << endl;
	cout << "-a, --adapter\tAdapter name for service." << endl;
	cout << "-q, --query\tIcegrid query. (default: XiaoNei)" << endl;
	cout << "-h, --host\tConnect to host. (default: XiaoNeiRegistry)" << endl;
	cout << "-p, --port\tPort number to use for connection. (default: 12000)" << endl;
	cout << "-i, --instance\tIcegrid instanceName. (default: XiaoNei)" << endl;
	cout << "-P, --protocol\tprotocol to use for connection. (default: default)" <<endl;
}

int main(int argc, char* argv[])
{
	string objectName = "";
	string adapter = "";
	string query = "XiaoNei";
	string instance = "XiaoNei";
	int port = 12000;
	string host = "XiaoNeiRegistry";
	string protocol = "default";
	if (argc < 2)
	{
		help();
		return 1;
	}

#define OPT(_a1, _a2)	else if ( !strcmp(argv[i], _a1) || !strcmp(argv[i], _a2) )

	int i;
	for (i = 1; i < argc; i++)
	{
		if ( i == 0);
		OPT("-?", "--help")	{help(); break;}

		else if (i + 1 == argc)	break;

		OPT("-o", "--object")	objectName = argv[++i];
		OPT("-a", "--adapter")	adapter = argv[++i];
		OPT("-q", "--query")	query = argv[++i];
		OPT("-h", "--host")	host = argv[++i];
		OPT("-p", "--port")	port = atoi(argv[++i]);
		OPT("-i", "--instance")	instance = argv[++i];
		OPT("-P", "--protocol")	protocol = argv[++i];
		else break;
	}

#undef OPT

	if ( i!=argc )
	{
		cout << "ERROR: malformed or unknown option near '" << argv[i] << "'." << endl;
		return 1;
	}

	if (adapter == "")
	{
		cout << "Need Adapter Name." << endl;
		return 1;
	}

	if (objectName == "")
	{
		cout << "Need Object Name." << endl;
		return 1;
	}

	ostringstream endpoint;
	endpoint << objectName << "@" << adapter;

	ostringstream locator;
	locator << query << "/Locator:" << protocol << " -h " << host << " -p " << port;
	PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("IceGrid.InstanceName", instance);
	properties->setProperty("Ice.Default.Locator", locator.str());
	InitializationData initializationData;
	initializationData.properties = properties;
	CommunicatorPtr communicator = initialize(initializationData);
	ObjectPrx object = communicator->stringToProxy(endpoint.str());
	AdminConsoleManagerPrx adminConsoleManager = AdminConsoleManagerPrx::checkedCast(object);

	initialize_readline();

	while (1)
	{
		char* line = readline (">>> ");
		if (!line)
		{
			break;
		}
		add_history(line);
		string order = string(line);
		if (order == "quit" || order == "exit")
		{
			break;
		}
		if (order == "help")
		{
			cout << "help\t\t\tPrint this message."<<endl;
			cout << "exit, quit\t\tExit this program."<<endl<<endl;
		}
		cout << adminConsoleManager->parse(order) << endl;
		free(line);
	}
	return 0;
}

