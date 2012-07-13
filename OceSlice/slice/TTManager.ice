#ifndef TT_MANAGER_ICE
#define TT_MANAGER_ICE
module xce {
  module searchcache {
    sequence<int> IntSeq;
    sequence<byte> ByteSeq;
    dictionary<int, ByteSeq> Int2ByteSeqMap; 
    interface TTManager {
      bool Update(int userId, ByteSeq userInfo);
      bool Insert(int userId, ByteSeq userInfo);
      bool Inserts(Int2ByteSeqMap userId2UserInfoMap);
      bool Get(int userId, out ByteSeq userInfo);
      bool Gets(IntSeq userIds, out Int2ByteSeqMap userId2UserInfoMap);
      bool Remove(int userId);
      bool RemoveMemIds(IntSeq userIds);
      bool InsertMemIds(IntSeq userIds);
      bool GetMemIdState(int userId);
      int GetMemIdsCount();
      int GetMemIdsListSize();
      bool Traversal(int offset, out int travelLength, out Int2ByteSeqMap userId2UserInfoMap);
      IntSeq TraversalKey(int offset, int requestNum);
    };
  };
};
#endif
