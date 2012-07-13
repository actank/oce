#ifndef XCE_FEED_RFEED_ADAPTER_H__
#define XCE_FEED_RFEED_ADAPTER_H__

#include "arch/adapter2.h"
#include "RFeed.h"
#include "FeedStatLog.h"

namespace xce {

inline feed::FocusManagerPrx CreateFocusAdapter(int uid) {
  return Adapter::feed::Create<feed::FocusManagerPrx>("M@FeedFocus", uid % 5);
}

inline feed::FocusManagerPrx CachedFocusAdapter(int uid) {
  return Adapter::feed::Cached<feed::FocusManagerPrx>("M@FeedFocus", uid % 5, 5);
}




inline feed::FeedItemManagerPrx CreateFeedItemAdapter(int uid) {
  return Adapter::feed::Create<feed::FeedItemManagerPrx>("FIM@FeedItem", uid % 20);
}

inline feed::FeedItemManagerPrx CachedFeedItemAdapter(int uid) {
  return Adapter::feed::Cached<feed::FeedItemManagerPrx>("FIM@FeedItem", uid % 20, 20);
}

inline feed::FeedItemManagerPrx CreateFeedMiniAdapter(int uid) {
  return Adapter::minifeed::Create<feed::FeedItemManagerPrx>("FIM@FeedMini", uid % 20);
}

inline feed::FeedItemManagerPrx CachedFeedMiniAdapter(int uid) {
  return Adapter::minifeed::Cached<feed::FeedItemManagerPrx>("FIM@FeedMini", uid % 20, 20);
}

/*
inline feed::FeedItemManagerPrx CachedFeedMiniAdapter2(int uid) {
  return Adapter::minifeed::Cached<feed::FeedItemManagerPrx>("FIM@FeedMini", 50, 50);
} */

inline feed::FeedItemManagerPrx CreateFeedSchoolAdapter(int school_id) {
  return Adapter::feedschool::Create<feed::FeedItemManagerPrx>("FIM@FeedSchool", school_id % 2);
}

inline feed::FeedItemManagerPrx CachedFeedSchoolAdapter(int school_id) {
  return Adapter::feedschool::Cached<feed::FeedItemManagerPrx>("FIM@FeedSchool", school_id % 2, 2);
}

inline feed::FocusManagerPrx CreateFeedFocusAdapter(int uid) {
  return Adapter::feed::Create<feed::FocusManagerPrx>("M@FeedFocus", uid % 5);
}

inline feed::FocusManagerPrx CachedFeedFocusAdapter(int uid) {
  return Adapter::feed::Cached<feed::FocusManagerPrx>("M@FeedFocus", uid % 5, 5);
}

inline feed::FeedNewsManagerPrx CreateFeedNews2Adapter(int uid) {
  return Adapter::feed::Create<feed::FeedNewsManagerPrx>("FIM@FeedNews", uid % 20);
}

inline feed::FeedNewsManagerPrx CachedFeedNews2Adapter(int uid) {
  return Adapter::feed::Cached<feed::FeedNewsManagerPrx>("FIM@FeedNews", uid % 20, 20);
}

/*
inline feed::FeedNewsManagerPrx CachedFeedNews3Adapter(int uid) {
  return Adapter::feed::Cached<feed::FeedNewsManagerPrx>("FIM@FeedNews", 50, 50);
} */

inline feed::FeedGroupManagerPrx CachedFeedGroupAdapter(int gid) {
  return Adapter::feed::Cached<feed::FeedGroupManagerPrx>("M@FeedGroup", gid % 10, 10);
} 

inline feed::FeedStatLogPrx CachedFeedStatLogAdapter(int uid) {
  feed::FeedStatLogPrx r = Adapter::feed::Cached<feed::FeedStatLogPrx>("M@FeedStatLog", uid % 1, 1);
  if (r) return r->ice_datagram();
  return r;
}
}
#endif // XCE_FEED_RFEED_ADAPTER_H__
