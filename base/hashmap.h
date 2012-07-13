#ifndef XCE_BASE_HASHMAP_H__
#define XCE_BASE_HASHMAP_H__

#ifdef __GNUC__

#include <ext/hash_map>
#include <ext/hash_set>
#if 0
namespace __gnu_cxx {
  template<> struct hash<std::string> {
    size_t operator()( const std::string& x ) const {
      return hash< const char* >()( x.c_str() );
    }
  };
}
#endif
#include "UtilCxx/src/hash_map_string.h"

namespace std {
  using __gnu_cxx::hash_map;
  using __gnu_cxx::hash_set;

  //using __gnu_cxx::hash;
}

#elif defined(_MSC_VER)

#include <hash_map>
#include <hash_set>

namespace std {
  using stdext::hash_map;
  using stdext::hash_set;

  template<typename T>
  struct hash {
    size_t operator()(const T& t) {
      return stdext::hash_value(t);
    }
  };
}

#endif

// using TR1's 
// #include <unordered_map>
// typedef std::unordered_map<std::string, int> stringmap;
// typedef std::unordered_map<int, int> intmap;

#endif // XCE_BASE_HASHMAP_H__

