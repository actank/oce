#ifndef XCE_FEED_TEST_ACTION_H__
#define XCE_FEED_TEST_ACTION_H__

#include <string>

#include "feed/cwf_feed/action.h"

namespace xce {

struct TestAction : public cwf::BaseAction {
  bool Match(const std::string& url) const;
  cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
};

}
#endif // XCE_FEED_TEST_ACTION_H__
