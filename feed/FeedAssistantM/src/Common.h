/*
 * Common.h
 *
 *  Created on: Jan 14, 2011
 *      Author: yejingwei
 */

#ifndef COMMON_H_
#define COMMON_H_
//#include "RFeed.h"
#include <vector>
#include <string>
#include "RFeed.h"
using namespace std;

/*class FeedConfig;
 class ReplyData;
 class FeedReply;
 typedef IceUtil::Handle<FeedConfig> FeedConfigPtr;
 typedef IceUtil::Handle<FeedReply> FeedReplyPtr;
 typedef IceUtil::Handle<ReplyData> ReplyDataPtr;*/

namespace xce {
namespace feed {
class IndexLocator {
public:
  static bool
      LocateIndex(long source, int stype, int actor, FeedIndexSeq & res);
  static bool LocateFids(long source, int stype, int actor, vector<long> & res);
};
class FeedConfigBuilder {
public:
  static FeedConfigPtr Build(const map<string, string>& props);
};
class FeedConfigLocator {
public:
  static FeedConfigPtr Locate(long feedid);
};
class ReplyDataBuilder {
public:
  static ReplyDataPtr Build(long source, int stype, int actor,
      FeedReplyPtr & reply);
};
class MemcHandler {
public:
  static bool UpdateReply(const vector<long>& fids, const FeedReplyPtr & reply);
};

//bool IsTester(int uid);
static bool IsTester(int actor){
  if (actor == 220678672 || actor == 322856518 || actor == 347348947 || actor
      == 347349486 ||actor == 347735846){
    return true;
  }else{
    return false;
  }
}

}
}
#endif /* COMMON_H_ */
