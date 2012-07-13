#ifndef __DISTCACHE_ICE__
#define __DISTCACHE_ICE__

#include <Ice/BuiltinSequences.ice>

module xce {
module distcache {
module server {
  ["java:type:java.util.ArrayList<Long>"] sequence<long> LongList;
  interface MetaServer {
    bool alloc(long id, int block);
    LongList get(int block);
  };
  sequence<byte> ByteList;
  dictionary<long, ByteList> DataTable;
 
  class CASData {
	  ByteList data;
	  long cas;
  };
  dictionary<long, CASData> CASDataTable;

  interface WriteServer {
        bool set(string clustername, long id, int block, ByteList data, bool isSync);
        bool addBatch(string clustername, DataTable  bulkDatas, bool isSync);
		LongList setBatch(string clustername, DataTable datas, bool isSync);
        bool setCAS(string clustername, long id, int block, ByteList data, long cas);
		LongList setBatchCAS(string clustername, CASDataTable datas);
		void setLogLevel(string level);
   };

 interface LoadServer {
		void loadByNode();
   };

};
};
};
#endif

