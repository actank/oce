#ifndef __STRING_H__
#define __STRING_H__

#include <string>
#include <Ice/Ice.h>
#include <vector>
#include <zlib.h>
#include <bzlib.h>
#include <boost/tokenizer.hpp>
#include <openssl/md5.h>
#include <cmath>
#include <Util.h>

namespace MyUtil {
using namespace std;

const int GZ_CHUNK = 16384;
const int BZ_CHUNK = 16384;

struct StrUtil {
	// 假设t由[0-9a-f]组成,并且超过6字节
	static int gen_md5_seed(const std::string &t, int mod) {
	    const int max_numb = 6;
	    size_t sz = t.size();
	    long j = 0;
	    for (size_t i = 0; i < sz && i < max_numb - 1; ++i) {
	        if (t[i] <= '9' && t[i] >= '0' )
	            j = (j<<4) + (t[i] - 48);
	        else
	            j = (j<<4) + (t[i] - 87);
	    }
	    return j%mod;
	}
	//    static string trim(const char* s);
	static string fixString(const string& s) {
		static const string notallow("'\"\\");
		string res;
		string::size_type pos = 0, fpos;
		unsigned char ch;

		while (true) {
			fpos = s.find_first_of(notallow, pos);
			if (fpos == string::npos)
				break;
			res.append(s, pos, fpos - pos);
			ch = s[fpos];
			if (ch == '\'')
				res += "\\'";
			else if (ch == '"')
				res += "\"";
			else if (ch == '\\')
				res += "\\\\";
			pos = fpos + 1;
		}
		res.append(s, pos, s.length() - pos);
		return res;
	}
	static string toUpper(const string& s) {
		string upper = s;
		transform(upper.begin(), upper.end(), upper.begin(),
				static_cast<int(*)(int)> (toupper));
		return upper;
	}
	static string toLower(const string& s) {
		string lower = s;
		transform(lower.begin(), lower.end(), lower.begin(),
				static_cast<int(*)(int)> (tolower));
		return lower;
	}
	static vector<string> split(const string& strStr, const string& strSep) {
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(strSep.c_str());
		StringArray ret;
		tokenizer token(strStr, sep);
		for (tokenizer::iterator it = token.begin(); it != token.end(); ++it) {
			ret.push_back(*it);
		}
		return ret;
	}
	static string md5(const string& s) {
		unsigned char md[16];
		uint i;

		MD5((const unsigned char *) s.c_str(), s.length(), md);

		char str[32 + 1];
		memset(str, 0, sizeof(str));
		for (i = 0; i < sizeof(md); i++) {
			sprintf(str + 2 * i, "%02x", md[i]);
		}

		return str;
	}
	static void string_replace(string & strBig, const string & strsrc,
			const string &strdst) {
		string::size_type pos = 0;
		string::size_type srclen = strsrc.size();
		string::size_type dstlen = strdst.size();
		while ((pos = strBig.find(strsrc, pos)) != string::npos) {
			strBig.replace(pos, srclen, strdst);
			pos += dstlen;
		}
	}
	static Ice::Long hashcode(const string& str) {
		Ice::Long __h = 0;
		for (size_t i = 0; i < str.length(); ++i)
			__h = 5 * __h + str[i];
		return std::abs(__h);
	}
	static string gzip_uncompress(const string& source, int level =
			Z_BEST_COMPRESSION) {
		int ret;
		unsigned char out[GZ_CHUNK];

		z_stream strm;
		ostringstream dest;

		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;

		ret = inflateInit(&strm);
		if (ret != Z_OK) {
			return "";
		}

		strm.avail_in = source.length();
		if (strm.avail_in == 0) {
			return "";
		}
		strm.next_in = (Bytef*) source.c_str();
		do {
			strm.avail_out = GZ_CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			if (ret == Z_STREAM_ERROR) {
				inflateEnd(&strm);
				return "";
			}
			switch (ret) {
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				return "";
			}
			dest << string((char*) out, GZ_CHUNK - strm.avail_out);
		} while (strm.avail_out == 0);

		inflateEnd(&strm);
		if (ret == Z_STREAM_END) {
			return dest.str();
		} else {
			return "";
		}
	}
	static string gzip_compress(const string& source, int level =
			Z_BEST_COMPRESSION) {
		int ret;
		unsigned char out[GZ_CHUNK];

		z_stream strm;
		ostringstream dest;

		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		ret = deflateInit(&strm, level);

		if (ret != Z_OK) {
			return "";
		}

		strm.avail_in = source.length();
		strm.next_in = (Bytef*) source.c_str();
		do {
			strm.avail_out = GZ_CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, Z_FINISH);
			if (ret == Z_STREAM_ERROR) {
				deflateEnd(&strm);
				return "";
			}
			dest << string((char*) out, GZ_CHUNK - strm.avail_out);
		} while (strm.avail_out == 0);

		if (strm.avail_in != 0) {
			deflateEnd(&strm);
			return "";
		}

		if (ret != Z_STREAM_END) {
			deflateEnd(&strm);
			return "";
		}

		deflateEnd(&strm);
		return dest.str();
	}
	static string bzip_compress(const string& source) {
		int ret;
		char out[BZ_CHUNK];

		bz_stream strm;
		ostringstream dest;

		strm.bzalloc = NULL;
		strm.bzfree = NULL;
		strm.opaque = NULL;
		strm.total_in_lo32 = 0;
		strm.total_in_hi32 = 0;
		strm.total_out_lo32 = 0;
		strm.total_out_hi32 = 0;

		ret = BZ2_bzCompressInit(&strm, 9, 0, 0);
		if (ret != BZ_OK) {
			return "";
		}

		strm.avail_in = source.length();
		strm.next_in = (char*) source.c_str();
		do {
			strm.avail_out = BZ_CHUNK;
			strm.next_out = out;
			ret = BZ2_bzCompress(&strm, BZ_FINISH);
			if (ret == BZ_SEQUENCE_ERROR) {
				BZ2_bzCompressEnd(&strm);
				return "";
			}
			dest << string(out, BZ_CHUNK - strm.avail_out);
		} while (strm.avail_out == 0);

		if (strm.avail_in != 0) {
			BZ2_bzCompressEnd(&strm);
			return "";
		}

		if (ret != BZ_STREAM_END) {
			BZ2_bzCompressEnd(&strm);
			return "";
		}

		BZ2_bzCompressEnd(&strm);
		return dest.str();
	}
	static string bzip_uncompress(const string& source) {
		int ret;
		char out[BZ_CHUNK];

		bz_stream strm;
		ostringstream dest;

		strm.bzalloc = NULL;
		strm.bzfree = NULL;
		strm.opaque = NULL;
		strm.avail_in = 0;
		strm.next_in = NULL;
		strm.total_in_lo32 = 0;
		strm.total_in_hi32 = 0;
		strm.total_out_lo32 = 0;
		strm.total_out_lo32 = 0;

		ret = BZ2_bzDecompressInit(&strm, 0, 0);
		if (ret != BZ_OK) {
			return "";
		}

		strm.avail_in = source.length();
		if (strm.avail_in == 0) {
			return "";
		}
		strm.next_in = (char*) source.c_str();
		do {
			strm.avail_out = BZ_CHUNK;
			strm.next_out = out;
			ret = BZ2_bzDecompress(&strm);
			if (ret == BZ_DATA_ERROR) {
				BZ2_bzDecompressEnd(&strm);
				return "";
			}
			switch (ret) {
			case BZ_DATA_ERROR:
			case BZ_PARAM_ERROR:
			case BZ_DATA_ERROR_MAGIC:
			case BZ_MEM_ERROR:
				BZ2_bzDecompressEnd(&strm);
				return "";
			}
			dest << string(out, BZ_CHUNK - strm.avail_out);
		} while (strm.avail_out == 0);

		BZ2_bzDecompressEnd(&strm);
		if (ret == BZ_STREAM_END) {
			return dest.str();
		} else {
			return "";
		}
	}

};

}
;
#endif
