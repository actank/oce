#include <MonitorAdapter.h>

using namespace monitor;

int main(int argc, char* argv[]) {
  MonitorAlerterAdapter::instance().reload();
  return 0;
}

