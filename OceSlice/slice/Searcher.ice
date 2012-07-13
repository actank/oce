#ifndef _SEARCHINTERFACE_ICE
#define _SEARCHINTERFACE_ICE

#include <Util.ice>

module com {
  module renren {
    module search {
      interface Searcher {
        MyUtil::ByteSeq search(MyUtil::ByteSeq request, int count);
      };

      interface Broker {
        MyUtil::ByteSeq search(MyUtil::Str2StrMap condition, int offset, int limit);
      };
    };
  };
};

#endif
