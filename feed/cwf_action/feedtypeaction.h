#ifndef XCE_FEED_FEEDTYPE_ACTION_H__
#define XCE_FEED_FEEDTYPE_ACTION_H__

#include <string>

#include "cwf/action.h"

namespace xce {

struct FeedTypeAction : public cwf::BaseAction {
  bool Match(const std::string& url) const;
  cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);

#if 0
  virtual cwf::User* Authorize(cwf::Request*);
  virtual const std::string& template_name() const {
    return "";
  }
  virtual bool Match(const std::string& url) const;
  virtual cwf::HttpStatusCode GenerateHeader(cwf::Request* request, cwf::Header* header);
  virtual ctemplate::TemplateDictionary* Process(cwf::Request* request);

  FeedTypeAction() : template_name_("replylist.tpl") {}
#endif
};

}
#endif // XCE_FEED_FEEDTYPE_ACTION_H__
