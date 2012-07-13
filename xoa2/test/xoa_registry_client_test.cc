#include "xoa2/client/xoa_registry_client.h"
#include "LogWrapper.h"

#include <iostream>
using namespace std;

int main(int argc, char argv) {
  MCE_INIT("./zk_test.log", "DEBUG");
  XoaRegistryClient client("localhost");
  string value;
  vector<string> children;
  client.Get("/xoa2/com/renren/xoa/search", &value);
  cout << "---------------- : " << value << endl;
  client.GetChildren("/xoa2/com/renren/xoa/search", &children);
//client.Get("/xoa2/renren/search", buffer, sizeof(buffer) - 1, 1);
//client.Get("/xoa2/renren/search", buffer, sizeof(buffer) - 1, 1);
//client.Get("/xoa2/renren/search", buffer, sizeof(buffer) - 1, 1);
  cout << "children size : " << children.size() << endl;
  for(size_t i = 0; i < children.size(); ++i) {
    cout << "\tchild " << i << " : " << children[i] << endl;
  }

  sleep(10000);
  return 0;
}

