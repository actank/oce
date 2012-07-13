#ifndef XCE_CWF_FOO_ACTION_H__
#define XCE_CWF_FOO_ACTION_H__

#include "cwf/action.h"
#include "cwf/stream.h"

namespace cwf {

struct EmptyAction : public BaseAction {
  bool Match(const std::string& url) const {
    return url == "/foo";
  }

  cwf::HttpStatusCode Process(Request*, Response* res) {
    static const std::string kDefaultContentType("text/html; charset=utf-8");
    res->header().set_status_code(cwf::HC_OK, "OK");
    res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
    res->OutputHeader();
    res->WriteRaw("<h1>foo</h1>");
    return cwf::HC_OK;
  }
};


}
#endif // XCE_CWF_FOO_ACTION_H__
