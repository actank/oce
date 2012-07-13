#include <MonitorAdapter.h>

using namespace monitor;

int main(int argc, char* argv[]) {
  MonitorAnalyzerAdapter::instance().reload();
  return 0;
}

