#ifndef __TRIPOD_ICE__
#define __TRIPOD_ICE__
[["cpp:include:vector"]]
#include <Ice/BuiltinSequences.ice>

module com {
  module renren {
    module tripod {

      ["java:type:java.util.ArrayList<Long>"]
        sequence<long> LongList;

      class TripodData {
        int block;
        long id;
        Ice::ByteSeq data;
      };
      sequence<TripodData> TripodDataSeq;

      interface LogicServer {
        bool add(string name, TripodDataSeq data, bool isSync);
        LongList set(string name, TripodDataSeq data, bool isSync);
        void setLogLevel(string level);
      };

      interface WriteServer {
        bool add(string name, TripodDataSeq data, bool isSync);
        LongList set(string name, TripodDataSeq data, bool isSync);
        void setLogLevel(string level);
      };

      interface MetaServer {
        bool alloc(long id, int block);
        LongList get(int block);
      };

      interface LoadServer {
        void monitor(string clustername);
        void load(string clustername, string node, int begin, int end);
        void migrate(string clustername, string source, string dest, int begin, int end);
      };
    };
  };
};
#endif

