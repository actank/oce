#ifndef XCE_ARCH_CONFIG_H__
#error you should use #include "arch/config.h"
#endif

namespace detail {
// MyUtil::Str2StrMap => Ice::PropertiesPtr
inline Ice::PropertiesPtr MakeProperties(const MyUtil::Str2StrMap& context) {
  // 貌似最简单的办法是构建一个 StringSeq --a=b
  Ice::StringSeq q;
  for (MyUtil::Str2StrMap::const_iterator i=context.begin(); i!=context.end(); ++i)
    q.push_back(std::string("--") + i->first + "=" + i->second);
  return Ice::createProperties(q);
}

}

