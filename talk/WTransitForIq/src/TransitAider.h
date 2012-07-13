#ifndef _TRANSITAIDER_H_
#define _TRANSITAIDER_H_


#include "Singleton.h"
//#include "TaskManager.h"
//#include "Pipe.h"


#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>

namespace talk {
namespace wtransit {


using namespace MyUtil;
// using namespace com::xiaonei::talk;
// using namespace com::xiaonei::talk::common;
// using namespace mop::hi::svc::dispatch;
// using namespace xce::feed;
// using namespace pugi;


class QtfEscape : public Singleton<QtfEscape>{
public:
	QtfEscape(){
		for (int i = 0; i < 128; i++)
			iscmd[i] = !(('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z') || ('0'
																			  <= i && i <= '9'));
		for (int i = 128; i < 256; i++)
			iscmd[i] = false;
		std::string str = ".,;!?%()/<># ";
		for (size_t i = 0; i < str.size(); i++)
			iscmd[(size_t) str[i]] = false;
	}
	std::string escape(const std::string& text){
		std::ostringstream ss;
		for (size_t i = 0; i < text.size(); ++i) {
			if (iscmd[(unsigned char) text[i]])
				ss << '`';
			ss << text[i];
		}
		return ss.str();
	}
private:
	bool iscmd[256];
};
	
	
class UrlParser{
public:
	static string getAid(const string& url){
		if (url.empty()) {
			return "";
		}
		size_t begin = url.find("id=");
		if (begin != string::npos && begin + 3 < url.length()) {
			size_t end = url.find('&', begin + 3);
			if (end != string::npos) {
				return url.substr(begin + 3, end - begin - 3);
			}
		}
		return "";
	}
	static string getId(const string& url, const string& name){
		if (url.empty()) {
			return "";
		}
		ostringstream os;
		size_t begin = url.find(name);
		if (begin != string::npos && begin + name.length() < url.length()) {
			for(size_t i = begin+name.length(); i < url.length(); i++){
				if(url[i]>='0' && url[i]<='9'){
					os << url[i];
				}else{
					break;
				}
			}
		}
		return os.str();
	}
};








class TemplateFactory : public MyUtil::Singleton<TemplateFactory>{
private:

public:
};

	

};
};

#endif /* _TRANSITAIDER_H_ */
