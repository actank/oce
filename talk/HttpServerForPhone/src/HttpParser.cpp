#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "IceLogger.h"
#include "HttpParser.h"
#include "Request.h"
#include "Session.h"

using namespace std;
using namespace talk::http;
using namespace boost;
using namespace MyUtil;

bool HttpParser::feed(const SessionPtr& session, string& data) {
	MCE_DEBUG("HttpParser::feed --> data:"<<data);
	//_tmp += data;
	if (_head.length() + data.length() + _postData.length() + _tmp.length()
			> 1024*20) {
		return false;
	}
	bool f = true;
	_tmp += data;
	//data = "";
	while (f) {
		size_t p = string::npos;
		if (_head.empty()) {
			p = _tmp.find("\r\n\r\n");
		} else {
			_postData += _tmp;
			_tmp = "";
//			MCE_DEBUG("HttpParser::feed --> post_data:"<<_postData
//					<< " length:"<<_postData.length());
			if (_postData.length() >= _contentLength && _contentLength >=0) {
				if (_props["method"] == "post") {

					if (_contentLength == 0) {
						_props["body"] = "";
					} else {
						_props["body"] = _postData.substr(0, _contentLength-1);

					}
					RequestPtr r = new Request();
					if (!r->init(_props)) {
						return false;
					}

					session->recvRequest(r);
					session->response(r);
					_head = "";
					_postData = "";
					_tmp = "";
					_props.clear();
					_contentLength = 0;
					//					session->shutdown();
					return true;
				}
			}
			return true;
		}
		if (p != string::npos) {
			_head = _tmp.substr(0, p);
			_tmp = _tmp.substr(p+4);

			vector<string> splitVec;
			split(splitVec, _head, is_any_of("\r\n"), token_compress_on);

			vector<string> tmpVec;
			split(tmpVec, splitVec.at(0), is_space(), token_compress_on);
			if (tmpVec.size() < 3) {
				return false;
			}
			_props["method"] = to_lower_copy(tmpVec.at(0));
			_props["path"] = tmpVec.at(1);
			size_t pos;
			const size_t offset = strlen("HTTP/");
			if ((pos = tmpVec.at(2).find('.', offset)) != string::npos) {
				_props["version_high"] = tmpVec.at(2).substr(offset, pos - offset);
				_props["version_low"] = tmpVec.at(2).substr(pos + 1);

			} else {
				_props["version_high"] = tmpVec.at(2).substr(offset);
				_props["version_low"] = "0";
			}
			for (vector<string>::iterator it = splitVec.begin()+1; it
					!= splitVec.end(); ++it) {
				pos = it->find(':');
				if (pos == string::npos) {
					//throw std::exception();
					return false;
				}

				string name = trim_copy(it->substr(0, pos));
				name = to_lower_copy(name);
				string value = trim_copy(it->substr(pos + 1));
				_props[name] = value;
				if (name == "content-length") {
					try {
						_contentLength = boost::lexical_cast<int>(value);
					} catch(std::exception& e) {
						MCE_WARN("HttpParser::feed --> parse content length err:"<< e.what());
					}
				}
			}
			if (_props["method"] == "get") {
				RequestPtr r = new Request();
				if (!r->init(_props)) {

					return false;
				}
				_head = "";
				_postData = "";
				_tmp = "";
				_props.clear();
				_contentLength = 0;
				//				RequestHandler::instance().handle(session,r);
				session->recvRequest(r);
				session->response(r);
				return true;

			} else if (_props["method"] == "post") {
				continue;
			} else {

				return false;
			}
		} else {
			return true;
		}
	}

	return true;
}

