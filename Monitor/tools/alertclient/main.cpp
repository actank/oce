#include <MonitorAdapter.h>
#include <iostream>
#include <boost/lexical_cast.hpp>

using namespace monitor;
using namespace std;
using namespace MyUtil;
using namespace boost;

int main(int argc, char* argv[]) {
  if (argc != 5) {
    cout << "AlertClient <subject> <message> <type> <interval>" << endl;
    return -1;
  }
  MonitorAlerterAdapter::instance().alert(string(argv[1]), string(argv[2]), lexical_cast<int>(string(argv[3])), lexical_cast<int>(string(argv[4])));
  return 0;
}
