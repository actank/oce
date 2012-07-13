#ifndef XCE_CACHE_COMMONCACHE_ICE__
#define XCE_CACHE_COMMONCACHE_ICE__

module xce {

sequence<int> IntSequence;
sequence<byte> Buffer;
sequence<Buffer> BufferSequence;
dictionary<int, Buffer> Int2BufferMap;

sequence<string> StringList;
dictionary<string, string> StringMap;

struct CacheState {
  bool update;
  long time;
};

interface Cache {
  // for java
  ["cpp:const"] Buffer GetByInt(int key);
  ["cpp:const"] Int2BufferMap GetsByInt(IntSequence keys);

  void PutByInt(int key, Buffer val);
  void PutsByInt(IntSequence keys, BufferSequence vals);

  void RemoveByInt(int key);
  void RemovesByInt(IntSequence keys);

  bool isValid();
  void setValid(bool state);

  CacheState State();
  void SetState(CacheState cs);

  // 
  ["cpp:const"] string Get(string key);
  ["cpp:const"] StringMap Gets(StringList keys);

  void Put(string key, string val);
  void Puts(StringList key, StringList val);

  void Remove(string key);
  void Removes(StringList key);
};

};
#endif // XCE_CACHE_COMMONCACHE_ICE__
