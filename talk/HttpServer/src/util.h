#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <stdexcept>
#include <time.h>

namespace talk{
namespace http{

using namespace std;

string UriDecode(const std::string & sSrc);
string UriEncode(const std::string & sSrc);

string gmtTime(time_t t);

//typedef std::string::value_type	char_type;
//
//char_type parseHexSymbol(char_type symbol) throw (std::out_of_range);
//
//bool isSafeForUrlPart(char_type ch);
//string decodeUrl(const string& url);
//string encodeUrlPart(const string& url);

}
}

#endif /*UTIL_H_*/
