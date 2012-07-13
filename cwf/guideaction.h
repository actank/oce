#ifndef CWF_GUIDEACTION_H__
#define CWF_GUIDEACTION_H__

#include "cwf/action.h"

namespace cwf {

class GuideAction : public BaseAction {
 public:
  virtual bool Match(const std::string& url) const;
  virtual HttpStatusCode Process(Request*, Response*);
 private:
  void SetHeader(Response*);
 private:
  static int counter_;
};

}

#endif

