#include <boost/algorithm/string.hpp>

#define XCE_CONFIG_CUSTOM
#define XCE_CONFIG_GRID_NAME "ConfigGrid"
#define XCE_CONFIG_GRID_LOCATOR "ConfigGrid/Locator:tcp -h 10.22.202.47 -p 4061"
#include "arch/config_adapter.h"


using namespace xce;
using namespace std;

ConfigPrx& ad() {
  // 1 use env first
  char* proxy = getenv("CONFIG_PROXY");
  if (proxy) {
    // 2 new communicator
    int argc = 0;
    char* argv[] = {0};
    Ice::CommunicatorPtr c = Ice::initialize(argc, argv);

    // 3 ConfigPrx
    RegistryLocator locator(c);
    static ConfigPrx p_(locator, proxy);
    return p_;
  }

  return CachedConfigPrx();
}

void help() {
	std::cout << "Usage:" << std::endl;
	std::cout << " h             : print help info." << std::endl;
	std::cout << " up attr value : update \'attr\' to \'value\'." << std::endl;
	std::cout << " q attr        : query \'attr\'." << std::endl;
	std::cout << " ex            : exit." << std::endl;
}

int main(int, char**) {
  std::cout << "Config Server Client 0.81. 'h' for help"<< std::endl;
	
	std::cout << ">>>";
	char buf[256];
  buf[255] = 0;
	std::vector<std::string> args;

	while(cin.getline(buf, 255))
	{
    //ad()->Update(argv[1], argv[2]);
	  boost::split(args, buf, boost::is_any_of("\t "));
		if(!args.empty())
		{
			if(args[0] == "h")
			{
				help();
			}
			else if(args[0] == "ex")
			{
		    cout << "Good bye." << endl;
				break;
			}
			else if(args[0] == "up")
			{
				if(args.size() < 3)
				{
		      cout << "usage : up 'attr' 'value'." << endl;
				}
				else
				{
					ad()->Update(args[1], args[2]);
		      cout << args[1] << " udpated." << endl;
				}
			}
      else if(args[0] == "q")
			{
				if(args.size() < 2)
				{
		      cout << "usage : q 'attr'." << endl;
				}
				else
				{
					string v = ad()->Query(args[1]);
		      cout << args[1] << " = " << v << endl;
				}
			}
			else
			{
		    cout << "Unknown command : " << buf << endl;
			}
		}
		cout << ">>>";
	}
	return 0;
}
