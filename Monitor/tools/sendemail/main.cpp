#include <util/cpp/AlertClient.h>

using namespace MyUtil;

int main(int argc, char* argv) {
  AlertClient::alertEmail("ning.zhao@renren-inc.com","title", "contentcnm");
  return 0;
}

