#include "util.h"

#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;
using namespace std;

const char HEX2DEC[256] = {
/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
/* 0 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 1 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 2 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 3 */0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

/* 4 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 5 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 6 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 7 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* 8 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 9 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* A */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* B */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* C */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* D */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* E */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* F */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

std::string talk::http::UriDecode(const std::string & sSrc) {
	// Note from RFC1630:  "Sequences which start with a percent sign   
	// but are not followed by two hexadecimal characters (0-9, A-F) are reserved   
	// for future extension"   

	const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	const int SRC_LEN = sSrc.length();
	const unsigned char * const SRC_END = pSrc + SRC_LEN;
	const unsigned char * const SRC_LAST_DEC = SRC_END - 2; // last decodable '%'    

	char * const pStart = new char[SRC_LEN];
	char * pEnd = pStart;

	while (pSrc < SRC_LAST_DEC) {
		if (*pSrc == '%') {
			char dec1, dec2;
			if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)]) && -1 != (dec2 = HEX2DEC[*(pSrc + 2)])) {
				*pEnd++ = (dec1 << 4) + dec2;
				pSrc += 3;
				continue;
			}
		}

		*pEnd++ = *pSrc++;
	}

	// the last 2- chars   
	while (pSrc < SRC_END)
		*pEnd++ = *pSrc++;

	std::string sResult(pStart, pEnd);
	delete [] pStart;
	return sResult;
}

// Only alphanum is safe.   
const char SAFE[256] = {
/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
/* 0 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 3 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

/* 4 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
/* 6 */0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

/* 8 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

/* C */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

std::string talk::http::UriEncode(const std::string & sSrc) {
	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
	const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	const int SRC_LEN = sSrc.length();
	unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
	unsigned char * pEnd = pStart;
	const unsigned char * const SRC_END = pSrc + SRC_LEN;

	for (; pSrc < SRC_END; ++pSrc) {
		if (SAFE[*pSrc])
			*pEnd++ = *pSrc;
		else {
			// escape this char   
			*pEnd++ = '%';
			*pEnd++ = DEC2HEX[*pSrc >> 4];
			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
		}
	}

	std::string sResult((char *)pStart, (char *)pEnd);
	delete [] pStart;
	return sResult;
}

static const string
		week[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const string month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

std::string talk::http::gmtTime(time_t t) {
	struct tm time;
	time = *gmtime(&t);
	ostringstream res;
	//Expires: Mon, 26 Jul 1997 05:00:00 GMT
	res << month[time.tm_wday] << ", ";
	
	if(time.tm_mday < 10){
		res << 0;
	}
	res << time.tm_mday << " "
		<< month[time.tm_mon] << " " << time.tm_year+1900 << " ";
	if (time.tm_hour < 10) {
		res << 0;
	}
	res << time.tm_hour << ":";

	if (time.tm_min < 10) {
		res << 0;
	}
	res << time.tm_min << ":";

	if (time.tm_sec < 10) {
		res << 0;
	}
	res << time.tm_sec << " GMT";
	
	return res.str();

}

//bool talk::http::isSafeForUrlPart(char_type ch) {
//	/*
//	 if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') )
//	 return true;
//	 if (ch == '.' || ch == '-' || ch == '_' || ch == '\'')
//	 return true;
//	 return false;
//	 */
//
//	static const bool validChars[] = { false, false, false, false, false,
//			false, false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, true, false, false, false, false, false, true, true,
//					false, true, true, true, true, true, true, true, true,
//			true, true, false, false, false, false, false, false, false, true,
//					true, true, true, true, true, true, true, true, true, true,
//					true, true, true, true, true, true, true, true, true, true,
//					true, true, true, true, true, false, false, false, false,
//					true, false, true, true, true, true, true, true, true,
//			true, true, true, true, true, true, true, true, true, true, true,
//					true, true, true, true, true, true, true, true, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false, false, false, false, false, false,
//					false, false, false };
//
//	return validChars [(unsigned char) ch];
//}
//
//talk::http::char_type talk::http::parseHexSymbol(talk::http::char_type symbol)
//		throw (std::out_of_range) {
//	switch (symbol) {
//	case 'A':
//	case 'a':
//		return 10;
//	case 'B':
//	case 'b':
//		return 11;
//	case 'C':
//	case 'c':
//		return 12;
//	case 'D':
//	case 'd':
//		return 13;
//	case 'E':
//	case 'e':
//		return 14;
//	case 'F':
//	case 'f':
//		return 15;
//	case '0':
//	case '1':
//	case '2':
//	case '3':
//	case '4':
//	case '5':
//	case '6':
//	case '7':
//	case '8':
//	case '9':
//		return (symbol - 48 );
//	default:
//		throw std::out_of_range ("Invalid hex symbol");
//	}
//
//	return 0;
//}
//
//string talk::http::decodeUrl(const string& url) {
//	string::size_type pos = url.find('%'), prevPos = 0;
//	ostringstream res;
//
//	do {
//		res << replace_all_copy(url.substr(prevPos, pos - prevPos), "+", " ");
//
//		if (pos == string::npos)
//			break;
//
//		char_type ch = (parseHexSymbol(url [pos + 1]) << 4 )
//				+ parseHexSymbol(url [pos + 2]);
//		res << ch;
//
//		prevPos = pos + 3;
//		pos = url.find('%', prevPos);
//
//	} while ( true);
//
//	return res.str();
//}
//
//// input must be in UTF-8
//string talk::http::encodeUrlPart(const string& url) {
//	ostringstream encodedUrl;
//	const char hex[] = "0123456789ABCDEF";
//
//	string::const_iterator it = url.begin();
//	while (it != url.end()) {
//
//		if (isSafeForUrlPart(*it)) {
//			encodedUrl << *it;
//		} else {
//			encodedUrl << '%' << hex[(*it >> 4) & 0xF] << hex[*it & 0xF];
//		}
//
//		++it;
//	}
//
//	return encodedUrl.str();
//}
