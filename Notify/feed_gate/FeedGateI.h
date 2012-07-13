#ifndef _FEED_GATEI_H_
#define _FEED_GATEI_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "Notify.h"
#include "RFeed.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"
#include "pugixml.hpp"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace ctemplate;

const string NOTIFY_FCGI_HOST = "notify.renren.com";
  
class FeedGetRequest : public Request{
public:
  FeedGetRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class FeedGateI : public FeedGate, public MyUtil::Singleton<FeedGateI> {
public:
  FeedGateI();
};


class XmlParser/*:public MyUtil::Singleton<XmlParser>*/{
public:
	static void ParseStatus(int stype,pugi::xml_document & doc, stringstream & ss);
	static void ParsePhotoOneOrPhotoMore(int stype,pugi::xml_document & doc, stringstream & ss);
	static void ParsePhotoOneReplied(int stype,pugi::xml_document & doc, stringstream & ss);


	static void ParseVideoShare(int stype,pugi::xml_document & doc, stringstream & ss);
	static void ParseVideoShareV1(int stype,pugi::xml_document & doc, stringstream & ss);
	static void ParsePhotoShare(int stype,pugi::xml_document & doc, stringstream & ss);
	static void ParsePhotoShareV1(int stype,pugi::xml_document & doc, stringstream & ss);
};


}
}

#endif // _FEED_GATEI_H_

