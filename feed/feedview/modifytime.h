#ifndef XCE_FEED_MODIFY_TIME_H__
#define XCE_FEED_MODIFY_TIME_H__

#include "ctemplate/template_modifiers.h"

namespace xce {

class FriendlyTimeModifier : public ctemplate::TemplateModifier {
public:
  virtual void Modify(const char* in, size_t inlen,
    const ctemplate::PerExpandData* per_expand_data,
    ctemplate::ExpandEmitter* outbuf,
    const std::string& arg) const;

  virtual bool MightModify(const ctemplate::PerExpandData* per_expand_data,
    const std::string& arg) const {
      return true;
  }
};

extern FriendlyTimeModifier friendly_time;

}
#endif //XCE_FEED_MODIFY_TIME_H__
