// #include <boost/test/minimal.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "feed/arch_feed/config.h"

#include "feed/arch_feed/config_adapter.h"
#include "cache/commoncache.h"

using namespace xce;

void proc_get(const std::string& key) {
  std::cout << Config(key, "default value") << std::endl;
}

void proc_put(const std::string& key, const std::string& val) {
  // static SvcDescriptorAdapter tm = CachedSvcDescriptor();
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 0;

  if (std::string("get") == argv[1]) {
    proc_get(argv[2]);
    return 0;
  }

  if (std::string("put") == argv[1]) {
    proc_put(argv[2], argv[3]);
    return 0;
  }
  return 0;
}

