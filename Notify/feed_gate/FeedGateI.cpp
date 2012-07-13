#include "FeedGateI.h"

#include <cstring>
#include <boost/algorithm/string/replace.hpp>
#include "FeedGroupAdapter.h"
#include "pugixml.hpp"
#include "Notify/util/headurl.h"
#include "Notify/util/json_escape.h"
#include "Notify/util/schema_manager.h"
#include "Notify/util/time_stat.h"
#include "FeedContentAdapter.h"
#include "FeedGroupAdapter.h"
#include "FeedMemcLoaderAdapter.h"
#include <boost/lexical_cast.hpp>
#include <openssl/md5.h>
#include "FeedAssistantNAdapter.h"
#include "FeedAssistantNF55Adapter.h"
#include "FeedViewAdapter.h"
#include "FeedViewF55Adapter.h"
#include "boost/foreach.hpp"
using namespace std;
using namespace xce::notify;
// using namespace xce::feed;
using namespace MyUtil;
using namespace xce::feed;

static const char SOURCE_SEPERATOR = '-';

static const int kMinPageUid = 600000000;
static const int kMaxPageUid = 700000000;

static const int STYPE_STATUS = 502;
static const int STYPE_PHOTO_ONE = 701;
static const int STYPE_PHOTO_MORE = 709;
// static const int STYPE_ALBUM = 602;




static int GetVersion(int type){
	return (type & 0x00FF0000) > 16;
}

string UnescapeXml(string  body_string) {
//	return body_string;
	string res =body_string;
	try {

		res = boost::regex_replace(res, boost::regex("&lt;"),
				string("<"));
		res = boost::regex_replace(res, boost::regex("&gt;"),
				string(">"));
		res = boost::regex_replace(res, boost::regex("&apos;"),
				string("\""));
		res = boost::regex_replace(res, boost::regex("&quot;"),
				string("\""));
		res = boost::regex_replace(res, boost::regex("&#39;"),
				string("'"));
		res = boost::regex_replace(res, boost::regex("&amp;"),//&需要放在最后
				string("&"));
//		MCE_INFO("UnescapeXml. old:" << body_string << " new:" << res);

		return res;

	} catch (...) {
		MCE_WARN("UnescapeXml --> error  body_string = " << body_string);
		return res;
	}
}
class UnreadCountRequest: public Request {
public:
  UnreadCountRequest(FCGX_Request* r) :
    Request(r) {
  }
  virtual bool response();
};

class ReadAllRequest: public Request {
public:
  ReadAllRequest(FCGX_Request* r) :
    Request(r) {
  }
  virtual bool response();
};

class PagerLogRequest: public Request {
public:
  PagerLogRequest(FCGX_Request* r) :
    Request(r) {
  }
  virtual bool response();
};
class FidGetRequest: public Request {
public:
	FidGetRequest(FCGX_Request* r) : Request(r) { }
  virtual bool response();
};

class FeedRequestFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/feed/get") == 0 || strcmp(path, "/get2.feed") == 0) {
        return new FeedGetRequest(r);
      }
      if (strcmp(path, "/get.feed") == 0) {
        return new FeedGetRequest(r);
      }
      if (strcmp(path, "/feed/group_unread") == 0) {
        return new UnreadCountRequest(r);
      }
      if (strcmp(path, "/feed/read_all") == 0) {
        return new ReadAllRequest(r);
      }
      if (strcmp(path, "/feed/pager_log") == 0) {
        return new PagerLogRequest(r);
      }
      if (strcmp(path, "/feed/fid") == 0) {
      	MCE_INFO("FeedRequestFactory::Create . get fid");
        return new FidGetRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedGateI::instance(), service.createIdentity("M",
      ""));

  HeadStorage::instance().Load();

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service."
      + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 64);
  fcgi_server->RegisterRequestFactory(new FeedRequestFactory());
  fcgi_server->Start();
}

FeedGateI::FeedGateI() {
}

static bool IsTester(int actor) {
  if (actor == 220678672 || actor == 322856518 || actor == 347348947 || actor
      == 347349486 || actor == 347735846) {
    return true;
  } else {
    return false;
  }
}

class FeedDataFetcher{
public:
	static long GetFid(int uid,long source,int stype,int actor);
	static FeedDataPtr GetFeedDataFromFeedView(int uid,long fid);
	static FeedDataPtr GetFeedDataFromMemcached(int uid,long fid);
	static FeedDataPtr CallMemcLoader(int uid, Ice::Long fid);
};

long FeedDataFetcher::GetFid(int uid,long source,int stype,int actor){
	try{
		long fid = xce::feed::FeedAssistantNAdapter::instance().getFeedId(source,stype,actor);
		MCE_INFO("FeedDataFetcher::GetFid. uid:" << uid << " stype:" << stype << " source:" << source << " actor:" << actor << " fid:" << fid);
		return fid;
	}catch (Ice::Exception& e) {
		MCE_WARN("FeedDataFetcher::GetFid err : " << e << " uid=" << uid
				<< " stype:" << stype << " source:" << source << " actor:" << actor << " LINE::" << __LINE__);
		return 0;
	}
}

FeedDataPtr FeedDataFetcher::GetFeedDataFromFeedView(int uid,long fid){
	FeedDataSeq dataseq;
	try{
		vector<long> fids;
		fids.push_back(fid);
		dataseq = xce::feed::FeedViewAdapter::instance().GetByIds2View(uid,"",0,fids,3);
		if(dataseq.empty()){
			CallMemcLoader(uid, fid);
			dataseq = xce::feed::FeedViewAdapter::instance().GetByIds2View(uid,"",0,fids,3);
		}

		if(dataseq.empty() or !dataseq.at(0)){
			MCE_WARN("FeedDataFetcher::GetFeedDataFromFeedView . dataseq empty. uid=" << uid << " fid:" << fid << " LINE::" << __LINE__);
			return 0;
		}
		FeedDataPtr data = dataseq.at(0);
		return dataseq.at(0);

	}catch(Ice::Exception& e){
		MCE_WARN("FeedDataFetcher::GetFeedDataFromFeedView : call FeedView error " << e << " uid=" << uid << " LINE::" << __LINE__);
		return 0;
	}
}

FeedDataPtr FeedDataFetcher::GetFeedDataFromMemcached(int uid,long fid){
	try{
		FeedDataPtr data =	xce::feed::FeedContentAdapter::instance().getFeed(fid);
		if(!data){
			data = CallMemcLoader(uid, fid);
		}
		return data;
	}catch(Ice::Exception& e){
		MCE_WARN("FeedDataFetcher::GetFeedDataFromMemcached. uid:"<< uid<< " fid:" << fid << " error " << e);
		return 0;
	}
}

FeedDataPtr FeedDataFetcher::CallMemcLoader(int uid, Ice::Long fid) {
  vector<Ice::Long> fseq;
  fseq.push_back(fid);
  FeedContentDict fdict;
  try {
    fdict = FeedMemcLoaderAdapter::instance().GetFeedDict(fseq, 200);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedDataFetcher::CallMemcLoader: " << e << " uid:" << uid << " fid:"<<fid);
  }
  FeedContentDict::iterator fi = fdict.find(fid);
  if (fi == fdict.end())
    return 0;
  MCE_INFO("FeedDataFetcher::CallMemcLoader. suc uid:" << uid << " fid:" << fid );
  return fi->second->data;
}

FeedDataPtr GetXmlFromFeedView(int uid,long source,int stype,int actor){
	long feed_id = 0;
	try{
		feed_id = xce::feed::FeedAssistantNF55Adapter::instance().getFeedId(source,stype,actor);
	}catch (Ice::Exception& e) {
		MCE_WARN("FeedContentAdapter getFeed() err : " << e << " uid=" << uid
				<< " stype:" << stype << " source:" << source << " actor:" << actor << " LINE::" << __LINE__);
		return 0;
	}
	if(feed_id <=0 ){
		MCE_WARN("FeedGetRequest::response.get fid fail.get by source uid=" << uid
				<< " stype:" << stype << " source:" << source << " actor:" << actor << " feed_id:" << feed_id);
		return 0;
	}
	FeedDataSeq dataseq;
	try{
		vector<long> fids;
		fids.push_back(feed_id);
		dataseq = xce::feed::FeedViewF55Adapter::instance().GetByIds2View(uid,"",0,fids,3);
	}catch(Ice::Exception& e){
		MCE_WARN("FeedContentAdapter getFeed() err : call FeedView error " << e << " uid=" << uid
				<< " stype:" << stype << " source:" << source << " actor:" << actor << " LINE::" << __LINE__);
		return 0;
	}
	if(dataseq.empty() or !dataseq.at(0)){
		MCE_WARN("GetXmlFromFeedView getFeed() empty " << " uid=" << uid << " fid:" << feed_id
				<< " stype:" << stype << " source:" << source << " actor:" << actor << " LINE::" << __LINE__);
		return 0;
	}
	FeedDataPtr data = dataseq.at(0);
	MCE_INFO("@@ fid:" << data->feed << " xml:" << dataseq.at(0)->xml);
	return dataseq.at(0);
}


string DumpProps(const map<std::string, std::string> & props){
	ostringstream os;
	typedef map<std::string, std::string>::value_type vt;
	using namespace boost;
	BOOST_FOREACH(const vt & p,props){
		os << " " << p.first << ":" << p.second;
	}
	return os.str();
}
bool FeedGetRequest::response() {
  //
  // get2.feed?id=12356287, 或者
  // get2.feed?actor=240599893&stype=2&source=356287
  //
  // 照片新鲜事:
  // http://notify.renren.com/get2.feed?stype=701&source=3597193176&actor=268728918
  // 状态新鲜事:
  // http://notify.renren.com/get2.feed?stype=502&source=1205826707&actor=240599893
  //
  TimeStat ts, tstotal;
  float response_t = 0.0f;
  float getfeedbyid_t = 0.0f;
  float memcloader = 0.0f;
  float getfeedbysource_t = 0.0f;
  float xml_t = 0.0f;
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }

  if (!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
    return false;
  }
  MCE_INFO("FeedGetRequest::response. uid << " << _login_uid << " props " << DumpProps(_props));
  string emptyRet = "Content-type: text/html\r\n\r\n{}";

  MCE_DEBUG("ticket verify ok");

  xce::feed::FeedDataPtr feed_data = 0;

  Ice::Long feed_id = getPropertyLong("id");
  Ice::Int stype = 0;
  MCE_INFO("FeedGetRequest::response. uid:" <<_login_uid);

  stype = getPropertyInt("stype");
  Ice::Long source = getPropertyLong("source");
  Ice::Int actor = getPropertyInt("actor");

  response_t = ts.getTime();
  string xml;

  if(feed_id > 0){
  	MCE_WARN("FeedGetRequest::response. uid:"<< _login_uid << " feed_id:" << feed_id << " should no happen");
  }

  feed_id = FeedDataFetcher::GetFid(_login_uid,source,stype,actor);

  if(feed_id <= 0){
  	FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
  	return false;
  }

  if(stype == 701 or stype == 708){
  	feed_data = FeedDataFetcher::GetFeedDataFromFeedView(_login_uid,feed_id);
  } else {
  	feed_data = FeedDataFetcher::GetFeedDataFromMemcached(_login_uid,feed_id);
  }
  if(!feed_data){
  	FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
  	return false;
  }

  xml = feed_data->xml;
  stringstream ss;
  ss << "Content-type: text/html; charset=utf-8\r\n\r\n{";
  ss << "\"feed_id\":" << feed_data->feed;
  pugi::xml_document doc;
  try{
  if (doc.load(xml.c_str())) {
    if (stype == STYPE_STATUS) {
    	XmlParser::ParseStatus(stype,doc,ss);
    } else if (stype == STYPE_PHOTO_ONE || stype == STYPE_PHOTO_MORE) {
    	XmlParser::ParsePhotoOneOrPhotoMore(stype,doc,ss);
    } else if (stype == 708) {
    	XmlParser::ParsePhotoOneReplied(stype,doc,ss);
    }else if(stype == 110 or stype == 5030){
    	if(GetVersion(feed_data->type) == 1){
    		XmlParser::ParseVideoShareV1(stype,doc,ss);
    	}else{
    		XmlParser::ParseVideoShare(stype,doc,ss);
    	}
    }else if(stype == 103){
    	if(GetVersion(feed_data->type) == 1){
    		MCE_INFO("FeedGetRequest::response. version:1. fid:" << feed_data->feed << " type:" << 103 << " line:" << __LINE__);
    		XmlParser::ParsePhotoShareV1(stype,doc,ss);
    	}else{
    		MCE_INFO("FeedGetRequest::response. version:0. fid:" << feed_data->feed << " type:" << 103 << " line:" << __LINE__);
    		XmlParser::ParsePhotoShare(stype,doc,ss);
    	}
    }
  }
  }
  catch (Ice::Exception& e) {
      MCE_WARN("FeedGetRequest::response: xml uid:" << _login_uid << " err:"  << e << __LINE__);
      FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
      return false;
    } catch (std::exception& e) {
      MCE_WARN("FeedGetRequest::response: xml uid:" << _login_uid << " err:" << e.what() << __LINE__);
      FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
      return false;
    }
  xml_t = ts.getTime();

  //string time = doc.child("message").child("xfeed").child("time").first_child().value();

  ss << '}';

  MCE_INFO("FeedGetRequest::response.uid:"<< _login_uid << " feed_id:" << feed_id << " stype:" << stype << " actor:" << actor << " source:" << source << " content:" << ss.str());

  ostringstream oss;
  oss << " response:" << response_t << " getfeedbyid:" << getfeedbyid_t
      << " memcloader:" << memcloader << " getfeedbysource:"
      << getfeedbysource_t << " xml:" << xml_t << " total:"
      << tstotal.getTime();
//  MCE_INFO("FeedGetRequest::response.feed_id from assistant:" << feed_id << " feed_id in data:" << feed_data->feed << " xml size : " << feed_data->xml.size() << " uid:" << _login_uid << " #COST#" << oss.str());


//  if(feed_data->feed != feed_id){
//    MCE_WARN("FeedGetRequest::response. nomatch feed_id from assistant:" << feed_id << " feed_id in data:" << feed_data->feed << "!!!!!! uid:" << _login_uid);
//    FCGX_FPrintF(_fcgi_out, "%s", emptyRet.c_str());
//    return false;
//  }else{
//    MCE_INFO("FeedGetRequest::response. domatch feed_id from assistant:" << feed_id << " feed_id in data:" << feed_data->feed << "!!!!!! uid:" << _login_uid);
//  }

  FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
  return true;
}

bool UnreadCountRequest::response() {
  TimeStat tstotal;
  if (!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  if (!Request::response()) {
    FCGX_PutS("Content-type: text/html\r\n\r\nverify failure", _fcgi_out);
    return false;
  }

  MyUtil::IntSeq groups;
  std::vector<std::string> strs;
  string group_list = _post_body;
  if (group_list.empty()) {
    group_list = getProperty("gl");
  }
  boost::split(strs, group_list, boost::is_any_of(","),
      boost::token_compress_on);
  for (size_t i = 0; i < strs.size(); ++i) {
    Ice::Int group_id = 0;
    try {
      group_id = boost::lexical_cast<Ice::Int>(strs[i]);
    } catch (boost::bad_lexical_cast & e) {
    }
    if (group_id > 0) {
      groups.push_back(group_id);
    }
  }

  stringstream output;
  output << "Content-type: text/html\r\n\r\n{\'host\':" << _login_uid
      << ",\'unread\':{";
  string emptyRet = "Content-type: text/html\r\n\r\n{}";
  TimeStat tg;
  MyUtil::Int2IntMap unread;
  try{
  unread =
      xce::feed::FeedGroupAdapter::instance().GetGroupsUnreadCountOneThread(_login_uid,
          groups);
  } catch (Ice::Exception& e) {
      MCE_WARN("UnreadCountRequest::response "<< " uid=" << _login_uid
        << " group_list:" << group_list << " err:" << e);
      FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
      return false;
  }
  MCE_INFO("uid " << _login_uid << " get groups unread count : " << unread.size() << "/" << groups.size());
  float group_t = tg.getTime();
  for (size_t i = 0; i < groups.size(); ++i) {
    if (i > 0) {
      output << ',';
    }

    MyUtil::Int2IntMap::const_iterator it = unread.find(groups[i]);
    int cnt = 0;
    if (it != unread.end()) {
      cnt = it->second;
    }
    //    output << '\'' << groups[i] << "\':" << (it != unread.end()) ? it->second : 0;
    output << '\'' << groups[i] << "\':" << cnt;
    MCE_INFO("UnreadCountRequest::response. uid:" << _login_uid << " gid:" << groups[i] << " first:" << it->first << " count:" << it->second << " cnt:" << cnt);
  }
  output << "}}";

  if (_login_uid == 347348947) {
    MCE_INFO("output:" << output.str());
  }
  FCGX_PutS(output.str().c_str(), _fcgi_out);
  MCE_INFO("UnreadCountRequest::response. uid:" << _login_uid << " #COST# " << " FeedGroupAdapter:" << group_t << " total:" << tstotal.getTime());

  return true;
}

bool ReadAllRequest::response() {
  // http://notify.renren.com/feed/read_all?groupid=502&userid=332
  TimeStat ttotal;
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }
  if (!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
    return false;
  }
  string emptyRet = "Content-type: text/html\r\n\r\n{}";

  MCE_DEBUG("ticket verify ok");

  int groupid = getPropertyInt("groupid");
  //  int userid = getPropertyInt("userid");
  if (groupid == -1) {
    MCE_INFO("ReadAllFeedRequest::response param error.");
    FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
    return false;
  }

  try {
    FeedGroupAdapter::instance().ReadAllFeed(groupid, _login_uid);
    MCE_INFO("ReadAllFeedRequest::response. groupid:" << groupid << " userid:" << _login_uid);
  } catch (Ice::Exception& e) {
    MCE_WARN("ReadAllFeedRequest::response. call FeedGroupAdapter err : " << e << " uid=" << _login_uid);
  }
  FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
  MCE_INFO("ReadAllRequest::response uid:" << _login_uid << " #COST# total:" << ttotal.getTime());
  return true;
}

bool PagerLogRequest::response() {
  // http://notify.renren.com/feed/pager_log?stype=701&source=3597193176&actor=268728918&empty=1

  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }
  if (!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
    return false;
  }
  string emptyRet = "Content-type: text/html\r\n\r\n{}";

  MCE_DEBUG("ticket verify ok");

  Ice::Int stype = getPropertyInt("stype");
  Ice::Long source = getPropertyLong("source");
  Ice::Int actor = getPropertyInt("actor");
  Ice::Int empty = getPropertyInt("empty");
  if (stype < 0 || source < 0 || actor < 0) {
    MCE_DEBUG("PagerLogRequest::response param error. uid:" << _login_uid);
    FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
    return false;
  }
  MCE_INFO("PagerLogRequest::response. uid:" << _login_uid << " source:" << source
      << " stype:" << stype << " actor:" << actor << " empty/error:" << empty );
  FCGX_FPrintF(_fcgi_out, emptyRet.c_str());
  return true;
}

void XmlParser::ParseStatus( int stype ,pugi::xml_document & doc,stringstream & ss){
	pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
	ss << ',' << "\"host_pic\":\"" << GetHeadUrl(
			host.child("tinyimg").first_child().value()) << "\"";

	pugi::xml_node status = doc.child("f").child("status");
	string content = status.child("title").first_child().value();
	boost::replace_all(content, "&apos;", "&quot;");
	ss << ',' << "\"content\":\"" << JsonEscape(content) << "\"";
	ss << ',' << "\"timestamp\":"
			<< status.child("time").first_child().value();
}

//701 709
void XmlParser::ParsePhotoOneOrPhotoMore( int stype ,pugi::xml_document & doc,stringstream & ss){
	pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
	ss << ',' << "\"host_pic\":\""
			<< host.child("tinyimg").first_child().value() << "\"";

	pugi::xml_node album = doc.child("f").child("album");
	if (album) {
		ss << ',' << "\"album_id\":\""
				<< album.child("id").first_child().value() << "\"";

		ss << ',' << "\"album_title\":\""
				<< UnescapeXml(album.child("title").first_child().value()) << "\"";

		ss << ',' << "\"photos\":" << "[";

		int i = 0;
		for (pugi::xml_node photo = album.child("photo"); photo; photo
		= album.next_sibling("photo")) {
			if (i++ > 0) {
				ss << ",";
			}
			if (false) {
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
	                		<< "\"," << "\"image\":\""
	                		<< photo.child("image").first_child().value() << "\","
	                		<< "\"big_image\":\""
	                		<< photo.child("bigImage").first_child().value() << "\","
	                		<< "\"desc\":\"" << photo.child("digest").first_child().value()
	                		<< "\"}";
			} else {
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
	                		<< "\"," << "\"image\":\""
	                		<< photo.child("image").first_child().value() << "\","
	                		<< "\"big_image\":\""
	                		<< photo.child("bigImage").first_child().value() << "\","
	                		<< "\"desc\":\"" << photo.child("digest").first_child().value()
	                		<< "\"," << "\"url\":\"" << "http://photo.renren.com/photo/"
	                		<< host.child("id").first_child().value() << "/photo-"
	                		<< photo.child("id").first_child().value()
	                		//                << album.child("id").first_child().value()<< "/" << "photo-"<<photo.child("id").first_child().value()
	                		<< "\"}";
			}
		}
		ss << "]";
	}
}

void XmlParser::ParsePhotoOneReplied( int stype ,pugi::xml_document & doc,stringstream & ss){
	pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
	ss << ',' << "\"host_pic\":\""
			<< host.child("tinyimg").first_child().value() << "\"";

	pugi::xml_node album = doc.child("f").child("album");
	if (album) {
		ss << ',' << "\"album_id\":\""
				<< album.child("id").first_child().value() << "\"";
		ss << ',' << "\"album_title\":\""
				<< album.child("title").first_child().value() << "\"";
		// ss << ',' << "\"album_url\":\"" << album.child("url").first_child().value() << "\"";
		// ss << ',' << "\"album_count\":\"" << album.child("count").first_child().value() << "\"";
		ss << ',' << "\"photos\":" << "[";

		int i = 0;
		for (pugi::xml_node photo = album.child("photo"); photo; photo
		= album.next_sibling("photo")) {
			if (i++ > 0) {
				ss << ",";
			}
			if (false) {
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
	                		<< "\"," << "\"image\":\""
	                		<< photo.child("image").first_child().value() << "\","
	                		<< "\"big_image\":\""
	                		<< photo.child("bigImage").first_child().value() << "\","
	                		<< "\"desc\":\"" << photo.child("digest").first_child().value()
	                		<< "\"}";
			} else {
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
	                		<< "\"," << "\"image\":\""
	                		<< photo.child("image").first_child().value() << "\","
	                		<< "\"big_image\":\""
	                		<< photo.child("bigImage").first_child().value() << "\","
	                		<< "\"desc\":\"" << photo.child("digest").first_child().value()
	                		<< "\"," << "\"url\":\"" << "http://photo.renren.com/photo/"
	                		<< host.child("id").first_child().value() << "/photo-"
	                		<< photo.child("id").first_child().value() << "\"}";

			}
		}
		ss << "]";
	}
}


void XmlParser::ParseVideoShare( int stype ,pugi::xml_document & doc,stringstream & ss){

  pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	string host_id = host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
//	ss << ',' << "\"host_pic\":\"" << host.child("tinyimg").first_child().value()
//			<< "\"";

  ss << ',' << "\"host_pic\":\"" << GetHeadUrl(
      host.child("tinyimg").first_child().value()) << "\"";

	pugi::xml_node share = doc.child("f").child("share");
	if (share) {
		ss << ',' << "\"share_id\":\"" << share.child("id").first_child().value()
				<< "\"";
		pugi::xml_node video;
		if(stype == 110){
			video = share.child("video");
		}else{
			video = share.child("connect").child("video");
		}

		if (video) {
			ss << ',' << "\"headtitle\":\"" << UnescapeXml(video.child("title").first_child().value())
					<< "\"";

			ss << ',' << "\"image\":\"" << video.child("image").first_child().value()
					<< "\"";

			ss << ',' << "\"url\":\"" << video.child("url").first_child().value()
					<< "\"";
			ss << ',' << "\"fullurl\":\""
					<< video.child("fullurl").first_child().value() << "\"";
		}

	}
}

void XmlParser::ParseVideoShareV1( int stype ,pugi::xml_document & doc,stringstream & ss){

  pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	string host_id = host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
//	ss << ',' << "\"host_pic\":\"" << host.child("tinyimg").first_child().value()
//			<< "\"";

  ss << ',' << "\"host_pic\":\"" << GetHeadUrl(
      host.child("tinyimg").first_child().value()) << "\"";

	pugi::xml_node share = doc.child("f").child("share");
	if (share) {
		string share_id = share.child("id").first_child().value();
		string headtitle = UnescapeXml(share.child("title").first_child().value());
		string image = share.child("thumbFullUrl").first_child().value();
		string url = share.child("meta").child("flashUrl").first_child().value();//flash url
		string fullurl = share.child("url").first_child().value();
		ss << ',' << "\"share_id\":\"" << share_id
				<< "\"";
		ss << ',' << "\"headtitle\":\"" << headtitle
				<< "\"";

		ss << ',' << "\"image\":\"" << image
				<< "\"";

		ss << ',' << "\"url\":\"" << url
				<< "\"";
		ss << ',' << "\"fullurl\":\"" << fullurl
				<< "\"";

	}
}

void XmlParser::ParsePhotoShare( int stype ,pugi::xml_document & doc,stringstream & ss){
	pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
	ss << ',' << "\"host_pic\":\"" << host.child("tinyimg").first_child().value()
			<< "\"";


	pugi::xml_node album = doc.child("f").child("share").child("album");
	if (album) {
		ss << ',' << "\"album_id\":\"" << album.child("id").first_child().value()
				<< "\"";

		ss << ',' << "\"album_title\":\"" << UnescapeXml(
				album.child("title").first_child().value()) << "\"";

		ss << ',' << "\"photos\":" << "[";

		int i = 0;
		for (pugi::xml_node photo = album.child("photo"); photo; photo
				= album.next_sibling("photo")) {
			if (i++ > 0) {
				ss << ",";
			}
			if (false) {
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
						<< "\"," << "\"image\":\""
						<< photo.child("image").first_child().value() << "\","
						<< "\"big_image\":\""
						<< photo.child("bigImage").first_child().value() << "\","
						<< "\"desc\":\"" << photo.child("digest").first_child().value()
						<< "\"}";
			} else if(true){
				string photourl = photo.child("url").first_child().value();
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
						<< "\"," << "\"image\":\""
						<< photo.child("image").first_child().value() << "\","
						<< "\"big_image\":\""
						<< photo.child("bigImage").first_child().value() << "\","
						<< "\"desc\":\"" << UnescapeXml(photo.child("digest").first_child().value())
						<< "\"," << "\"url\":\"" << UnescapeXml(photourl)
				//                << album.child("id").first_child().value()<< "/" << "photo-"<<photo.child("id").first_child().value()
						<< "\"}";
			} else {
				ss << "{" << "\"id\":\"" << photo.child("id").first_child().value()
						<< "\"," << "\"image\":\""
						<< photo.child("image").first_child().value() << "\","
						<< "\"big_image\":\""
						<< photo.child("bigImage").first_child().value() << "\","
						<< "\"desc\":\"" << UnescapeXml(photo.child("digest").first_child().value())
						<< "\"," << "\"url\":\"" << "http://photo.renren.com/photo/"
						<< host.child("id").first_child().value() << "/photo-"
						<< photo.child("id").first_child().value()
				//                << album.child("id").first_child().value()<< "/" << "photo-"<<photo.child("id").first_child().value()
						<< "\"}";
			}
		}
		ss << "]";
	}
}

void XmlParser::ParsePhotoShareV1( int stype ,pugi::xml_document & doc,stringstream & ss){
	pugi::xml_node host = doc.child("f").child("from");
	ss << ',' << "\"host_id\":" << host.child("id").first_child().value();
	ss << ',' << "\"host_name\":\"" << JsonEscape(
			host.child("name").first_child().value()) << "\"";
	ss << ',' << "\"host_pic\":\"" << GetHeadUrl(host.child("tinyimg").first_child().value())
			<< "\"";

	pugi::xml_node meta = doc.child("f").child("share").child("meta");
	if (meta) {
		ss << ',' << "\"album_id\":\"" << meta.child("albumid").first_child().value()
				<< "\"";

		ss << ',' << "\"album_title\":\"" << UnescapeXml(
				meta.child("albumname").first_child().value()) << "\"";

		ss << ',' << "\"photos\":" << "[";

		int i = 0;
		string photoid = doc.child("f").child("share").child("resourceId").first_child().value();
		string image = doc.child("f").child("share").child("thumbFullUrl").first_child().value();
		string big_image = "";
		string url = doc.child("f").child("share").child("url").first_child().value();
		string desc = JsonEscape(doc.child("f").child("share").child("title").first_child().value());

		string photourl = doc.child("f").child("share").first_child().first_child().value();
		ss << "{" << "\"id\":\"" << photoid	<< "\","
				<< "\"image\":\"" << image << "\","
				<< "\"big_image\":\""	<< big_image << "\","
				<< "\"desc\":\"" << UnescapeXml(desc)				<< "\"," << "\"url\":\"" << UnescapeXml(url)
				<< "\"}";
		ss << "]";
	}
}

struct FidCost{

};

bool checkMd5(string uid, const string & key) {
//	return true;

	const static string kGetFidKey = "renrenclub$^%&^%&$%^&";
	string orig = uid;
	uid += kGetFidKey;
	unsigned char* md5 = MD5((unsigned char*) uid.c_str(), uid.size(), NULL);
	ostringstream res;
	for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		res << hex << setw(2) << setfill('0') << (unsigned int) md5[i];
	}
	string res_str = res.str();

	return res.str() == key;
}

bool FidGetRequest::response() {
	//
	// dispatch.notify?schema=32&type=1
	// POST BODY : source=123&from=1231&....
	//

	if (!_fcgi_out) {
		MCE_WARN("get notify fcgi_out null");
		return false;
	}
  string emptyRet = "Content-type: text/html\r\n\r\n{}";

//	long source = getPropertyLong("source");
//	int stype = getPropertyInt("stype");
//	int actor = getPropertyInt("actor");

  Ice::Int stype = getPropertyInt("stype");
  Ice::Long source = getPropertyLong("source");
  Ice::Int actor = getPropertyInt("actor");

	string post_body = UriDecode(getProperty("post_body"));
	if(source <0 || stype < 0|| actor < 0){
		MCE_INFO("FidGetRequest. source,stype,actor not ok. line:" << __LINE__);
		FCGX_FPrintF(_fcgi_out,
				"Content-type: text/html\r\n\r\n-2,schema type or post is empty");
		return false;
	}

	long sourceid ;
	int stypeid;
	int actorid;
	try{
		using namespace boost;
		sourceid = lexical_cast<long>(source);
		stypeid = lexical_cast<int>(stype);
		actorid = lexical_cast<int>(actor);
	}catch(...){
		MCE_INFO("FidGetRequest source,stype,actor not ok. line:" << __LINE__);
		FCGX_FPrintF(_fcgi_out,
				"Content-type: text/html\r\n\r\n-2,cast source stype actor error");
		return false;
	}

	map<string,string> post_map;
	std::vector<std::string> strs;
	boost::split(strs, post_body, boost::is_any_of("&"), boost::token_compress_on);
	for (size_t i = 0; i < strs.size(); ++i) {
		size_t pos = strs[i].find('=');
		if (pos != string::npos) {
			post_map.insert(make_pair(strs[i].substr(0, pos), strs[i].substr(pos
					+ 1)));
		}
	}
	ostringstream seed_os;
	seed_os << source << "|" << stype << "|" << actor;
	string seed = seed_os.str();
	map<string,string>::iterator it = post_map.find("key");
	if(it == post_map.end()){
		FCGX_FPrintF(_fcgi_out,
				"Content-type: text/html\r\n\r\n-6,no found key,");
		MCE_INFO("FidGetRequest no found key line:" << __LINE__);
		return false;
	}
	if (!checkMd5(seed, it->second)) {
		FCGX_FPrintF(_fcgi_out,
				"Content-type: text/html\r\n\r\n-6,MD5 not match,");
		MCE_INFO("FidGetRequest md5 not match. line:" << __LINE__);
		return false;
	}

	long fid = FeedAssistantNAdapter::instance().getFeedId(source,stype,actor);
	MCE_INFO("FidGetRequest: source:" << source << " stype:" << stype << " actor:" << actor
			<< " fid:" << fid
			<< " "<< __LINE__);

  stringstream output;
  output << "Content-type: text/html\r\n\r\n{\'fid\':" << fid << "}";
	FCGX_FPrintF(_fcgi_out,output.str().c_str());
	MCE_INFO("FidGetRequest:" << __LINE__);

	return true;
};
