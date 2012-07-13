#ifndef XCE_FEED_REPLY_ADAPTER_H__
#define XCE_FEED_REPLY_ADAPTER_H__

#include "feed/arch_feed/adapter2.h"
#include "replynotify.h"

namespace xce {

inline reply::ReplyNotifyIndexManagerPrx CreateReplyIndexAdapter(int uid) {
  return Adapter::feed::Create<reply::ReplyNotifyIndexManagerPrx>("ReplyNotifyIndex@ReplyNotifyIndex", uid % 1);
}

inline reply::ReplyNotifyIndexManagerPrx CachedReplyIndexAdapter(int uid) {
  return Adapter::feed::Cached<reply::ReplyNotifyIndexManagerPrx>("ReplyNotifyIndex@ReplyNotifyIndex", uid % 1, 1);
}

}
#endif // XCE_FEED_REPLY_ADAPTER_H__
