#ifndef __FEEDUSERRECORD_ICE__
#define __FEEDUSERRECORD_ICE__


module xce {
module feed {
  struct FeedUserRecordData{
    long newsfeedid;
    long favoritefeedid;
  };
  class FeedUserRecordRes{
    FeedUserRecordData data;
  };
  class FeedUserRecord {
    void              Set(int uid, long feedid, int type);
    FeedUserRecordRes Get(int uid);
	
  };

};
};

#endif
