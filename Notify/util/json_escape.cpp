#include <sstream>
#include <iomanip>
#include "json_escape.h"
using namespace std;

namespace xce {
namespace notify {

string JsonEscape(const string & s)
{
	unsigned char c;
	string dest;

	for (size_t i = 0; i < s.size(); ++i)  
	{ 
		c = s[i]; 
		if ((c == '\\') || (c == '"') || (c == '>')) 
		{ 
			dest += '\\'; 
			dest += c; 
		} else if (c == '\b') {
		  dest += "\\b"; 
    } else if (c == '\t') {
	    dest += "\\t"; 
    } else if (c == '\n') {
			dest += "\\n"; 
    } else if (c == '\f') {
		  dest += "\\f"; 
    } else if (c == '\r') {
		  dest += "\\r"; 
    } else { 
			if (c < ' ') { 
				//t = "000" + Integer.toHexString(c);
        std::ostringstream oss;
        oss << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<int>(c);
        dest += oss.str();
        //char t[8];
        //snprintf(t, 7, "\\u%04x", (unsigned char)c);
        //t[7] = 0;
        //dest += t;
			} else { 
				dest += c; 
			} 
		}
	} 
	return dest;
}


}
}
