#include <MonitorAdapter.h>
#include <iostream>

using namespace std;
using namespace monitor;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "StopHeartBeat <subject>" << endl;
    return -1;
  }
  MonitorHeartBeaterAdapter::instance().remove(string(argv[1]));
  cout << "Stop " << string(argv[1]) << endl;
  return 0;
}

