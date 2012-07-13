#ifndef ADMEMORY_ICE

#include <Util.ice>
module xce {
module ad {

struct MemoryInfo {
  float weight;
  byte click;
  short pv;
};

dictionary<long, MemoryInfo> MemoryInfoMap;

dictionary<byte, short> Byte2ShortMap;
sequence<short> ShortSeq;
dictionary<byte, ShortSeq> Click2PvMap;

interface AdMemory {
  MemoryInfoMap GetMemoryWithWeight(int uid);
  void Pv(int uid, long group);
  void PvBatch(int uid, MyUtil::LongSeq group);
  void Click(int uid, long group);
  void Reload();
  Byte2ShortMap AveragePv();
  Click2PvMap AllClickedPv();
};

};
};
#define ADMEMORY_ICE
#endif
