#include "MD5Util.h"

using namespace xce::md5;
using namespace std;

long MD5Util::stringToMD5(const string& source) {
	unsigned char* c_md;
	c_md = MD5((unsigned char*)source.c_str(), strlen(source.c_str()), NULL);
	long c_value = *(long *)c_md;
	return c_value;
}

long MD5Util::stringSeqToMD5(const vector<string>& source) {
	string dest;
	for(vector<string>::const_iterator iter=source.begin(); iter!=source.end(); ++iter) {
		dest += *iter;
	}
	unsigned char* c_md;
	c_md = MD5((unsigned char*)dest.c_str(), strlen(dest.c_str()), NULL);
	long c_value = *(long *)c_md;
	return c_value;
}
