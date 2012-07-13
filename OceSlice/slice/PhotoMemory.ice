#ifndef __PHOTO_MEMORY_ICE__
#define __PHOTO__MEMORY_ICE__

#include <Util.ice>


module xce
{
  module feed
  {
    class PhotoContent
    {
      long photoid;
      int ownerid;
      string url;
      string albumname;
      string title;
      MyUtil::IntSeq tagusers;  
    };

    class PhotoRankInfo{
      long photoid;
      int  clickcount;  
      int  commentcount;  
      int  tagcount;
      int  time;  
    };

    struct PhotoItem{
      long photoid;
      int  ownerid;
      int  friendfrank;
    };
    
	
    struct ContentItem{
      long photoid;
      int  ownerid;
    };

    sequence<PhotoContent> PhotoContentSeq;
    sequence<PhotoRankInfo> PhotoRankInfoSeq;
    sequence<long> PhotoSeq;
    dictionary<long,PhotoContent> PhotoContentDict;
    dictionary<long,PhotoRankInfo> PhotoRankInfoDict;
    dictionary<long,MyUtil::LongSeq> PhotoSeqDict;


    sequence<ContentItem> ContentItemSeq;
    
    sequence<PhotoItem> PhotoItemSeq;

    class UserPhotoSeq{
      int  userid;
      ContentItemSeq  itemList; 
    };

    sequence<UserPhotoSeq> UserPhotoSeqList;

    class PhotoMemoryManager{
      PhotoContentSeq GetPhotoContentSeq(int userid,int begin, int end);
      PhotoContentSeq GetFriendPhotoContentSeq(int userid,int friendid, int begin, int end);
      void SetRead(int userid, long photoid);
      void RemoveFriendPhotos(int userid, int friendid);
      void RemoveDispatchedPhotos(int userid, long photoid);
    };
    class PhotoMemoryGate {
      bool ReloadTemplate();
      string GetPhotos(int userid, int begin, int limit);
      string GetFriendPhotos(int userid,int friend, int begin, int limit);
    };

    class PhotoDataGatheringManager{
      //void  GatherPhotoData(int ownerid, MyUtil::LongSeq photoIds,MyUtil::Int2IntMap friend2Rank);

      void  GatherPhotoList(int ownerid,MyUtil::LongSeq photoIds,MyUtil::Int2IntMap friend2Rank);
      void  GatherUpLoadPhotos(int ownerid,MyUtil::LongSeq photoIds);

      void  IncCommentCount(long photoid);
      void  IncTagCount(long photoid, int tagid);
      void  SetRead(int userid, long photoid);
      void  Test();
    };	
};
};
#endif
