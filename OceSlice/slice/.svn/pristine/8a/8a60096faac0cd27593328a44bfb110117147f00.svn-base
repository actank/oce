#ifndef __EDM_FEED_EDM_CACHE__
#define __EDM_FEED_EDM_CACHE__

#include <Util.ice>

module xce {
module edm {
struct FeedSchoolInfo {
    int id;
    int enrollYear;
  };
sequence<FeedSchoolInfo> FeedSchoolInfoSeq;
class FeedUserInfo {
  int userId;
  int limitCheck;
  int gender;
  int age;
  int stage;
  string province;
  string city;
  FeedSchoolInfoSeq universitySeq;
  FeedSchoolInfoSeq highschoolSeq;
};
sequence<int> FriendIdSeq;
class FeedSeedInfo {
  //bool real;
  int configId;
  int type;
  int adType;
  int limitType;
  int interactiveFlag;
  string name;
  string title;
  string url;
  string body;
  string bodyUrl;
  string logo;
  string mediaSrc;
  string socialTitle;
  int appId;
  FriendIdSeq friendId;
  int displayFriendType;
  string likePageLink;
  string likeName;
};

interface FeedEdmCache {
	//----------------------------------------------------------------------------------------------------
	FeedSeedInfo SelectEdmById(int edmid);
	FeedSeedInfo SelectEdmByType(int userId, int type);
 	//---------------------------------------------------------------------------------------------------- 
	FeedSeedInfo SelectEdm(int userId, MyUtil::Str2StrMap params, FeedUserInfo user);
	FeedSeedInfo SelectEdmForPhone(int userId, MyUtil::Str2StrMap params, FeedUserInfo user);

  void SetSendingFlag(int edmId);
  void SetSuspendFlag(int edmId);
  void UpdateEdmInfo(int edmId);
  void SetForComplete();
  string LookUpEdm(int edmId);
  //void SetSendingFlag(int type, int edmId);
  //void SetSuspendFlag(int type, int edmId);
};

};
};
#endif
