#include "url_encode.h"

namespace xce {
namespace notify {

static string CharToHex(char c)
{
	string result;
	char first;
	char second;
	
	first = (c & 0xF0) / 16;
	first += first > 9 ? 'A' - 10 : '0';
	second = c & 0x0F;
	second += second > 9 ? 'A' - 10 : '0';
	
	result.append(1, first);
	result.append(1, second);
	
	return result;
}

string UrlEncode(const string &url)
{
	string result;
	string::const_iterator iter;
	
	for(iter = url.begin(); iter != url.end(); ++iter) 
	{
		switch(*iter) 
		{
		case ' ':
			result.append(1, '+');
			break;
			// alnum
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case '0': case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9':
			// mark
		case '-': case '_': case '.': case '!': case '~': case '*': case '\'': 
		case '(': case ')':
			result.append(1, *iter);
			break;
			// escape
		default:
			result.append(1, '%');
			result.append(CharToHex(*iter));
			break;
		}
	}	
	return result;
}

}
}

