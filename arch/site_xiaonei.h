#ifndef XCE_CWF_SITE_XIAONEI_H__
#define XCE_CWF_SITE_XIAONEI_H__

namespace cwf {
class User;
struct Request;
}

namespace xce {

cwf::User* Authorize(cwf::Request*);

}
#endif // XCE_CWF_SITE_XIAONEI_H__
