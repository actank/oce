/*
 * =====================================================================================
 *
 *       Filename:  SearchRelation.ice
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/14/2010 06:07:56 PM
 *       Revision:  none
 *       Compiler:  slice
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#ifndef __SEARCH_RELATION_ICE__
#define __SEARCH_RELATION_ICE__
#include <Util.ice>

module xce {
module searchrelation {

enum SourceType {USER, BLOG, SHARE, FEED, STATE, LETTER};
enum UserRelation {FRIEND, SELF, FRIEND2, BLACKLIST, LOVER, CONCERNER};
enum BlogPrivacy {PRIVACYNOTEXIST, TOALL, ONLYFRIEND, HASBLACKLIST, ONLYSELF, ENCRYPTION};
enum StatePrivacy {ALL, JUSTFRIEND};

struct SimpleSource {
  SourceType type;
  int sourceId;
};
//表示一个区间[minId, maxId)内的所有整数
struct Range {
  int minId;
  int maxId;
};

struct BigRange {
  long minId;
  long maxId;
};

struct VisitRelation {
  int userId;
  SourceType type;
  bool canVisit;
  MyUtil::IntSeq sourceIds;
  Range visitRange;
};

interface BlogCache {
  void setBlogOwner(int blogId, int userId);
  void setBlogOwnerBatch(MyUtil::IntSeq blogIds, MyUtil::IntSeq userIds);
  int getBlogOwner(int blogId);
  MyUtil::Int2IntMap getBlogOwnerBatch(MyUtil::IntSeq blogIds);
  BlogPrivacy getBlogPrivacy(int blogId);
  void setBlogPrivacy(int blogId, BlogPrivacy privacy);
  void addBlacklist(int blogId, int userId);
  void addBlacklistBatch(int blogId, MyUtil::IntSeq userIds);
  void setBlacklist(int blogId, MyUtil::IntSeq userIds);
  void deleteBlacklist(int blogId, int userId);
  void deleteBlacklistBatch(int blogId, MyUtil::IntSeq userIds);
  bool beInBlacklist(int blogId, int userId);
  MyUtil::IntSeq beInBlacklistBatch(int blogId, MyUtil::IntSeq userIds); 
};

interface UserRelationCache {
  void addFriendBatch(int userId, MyUtil::IntSeq friends);
  void addFriend(int userId, int friend);
  void setFriend(int userId, MyUtil::IntSeq friends);
  MyUtil::IntSeq getFriends(int userId);
  bool isFriend(int userId1, int userId2);
  void deleteFriendBatch(int userId, MyUtil::IntSeq friends);
  void deleteFriend(int userId, int friend);
  void deleteAllFriend(int userId);
};

interface StateCache {
  void setStateOwner(long stateId, int userId);
  int setNewestState(int userId, long stateId);
  StatePrivacy getStatePrivacy(long stateId);
  void setStatePrivacy(long stateId, StatePrivacy privacy);
};

interface FeedCache {
};

interface UserSourceRelation {
  bool canVisit(int userId, SimpleSource source, out int error);
  VisitRelation getVisitRelation(int userId, SourceType type, Range visitRange,out int error);
  UserRelation getUserRelation(int userId1, int userId2, out int error);
  int getOwner(SimpleSource source, out int error);
  bool belongToFriend(int userId, SimpleSource source, out int error);
};

};
};
#endif
