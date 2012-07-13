#ifndef _MD5UTIL_H_
#define _MD5UTIL_H_
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <vector>
#include <openssl/md5.h>

#include <openssl/crypto.h>

#ifdef CHARSET_EBCDIC
#include <openssl/ebcdic.h>
#endif
namespace MyUtil{

class MD5Util {
public:
	static long stringToMD5(const std::string& source)
	{
		return M_MD5((unsigned char*)source.c_str(), strlen(source.c_str()), NULL);
	}
	
	static long stringSeqToMD5(const std::vector<std::string>& source)
	{
		string dest;
		for(vector<string>::const_iterator iter=source.begin(); iter!=source.end(); ++iter)
		{
			dest += *iter;
		}
		return M_MD5((unsigned char*)dest.c_str(), strlen(dest.c_str()), NULL);
	}

protected:

static long M_MD5(const unsigned char *d, size_t n, unsigned char *md)
{
	MD5_CTX c;
	unsigned char m[MD5_DIGEST_LENGTH];

	if (md == NULL) md=m;
	
	if (!MD5_Init(&c))
		return 0;

#ifndef CHARSET_EBCDIC
	MD5_Update(&c,d,n);
#else
	{
		char temp[1024];
		unsigned long chunk; 
		
		while (n > 0)
		{
			chunk = (n > sizeof(temp)) ? sizeof(temp) : n;
			ebcdic2ascii(temp, d, chunk);
			MD5_Update(&c,temp,chunk);
			n -= chunk;
			d += chunk;
		}
	}
#endif
	MD5_Final(md,&c);
	OPENSSL_cleanse(&c,sizeof(c)); /* security consideration */
	long res = *(long*)md;
	return res;
}

};

}
#endif
