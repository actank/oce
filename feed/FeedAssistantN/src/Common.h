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
#include "XceLoggerAdapter.h"
using namespace com::xiaonei::service;
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
class ReplyLogger: public Singleton<ReplyLogger>{
public:
  void Init();
  bool AddReply(Ice::Long fid,Ice::Long source, Ice::Int stype,
      Ice::Int actor, const FeedReplyPtr& reply);
private:
  XceLoggerPtr logger_;
  vector<string> path_;
};
class ReplyXmlParser: public Singleton<ReplyXmlParser>{
public:
	int ParseReplier(const string & xml);
};

}
}
#endif /* COMMON_H_ */
