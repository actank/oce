
#include "compress.h"
#include "IceLogger.h"

#include <zlib.h>


//static unsigned char  gzheader[10] = { 0×1f, 0×8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, OS_CODE };
int compress__(Bytef *dest,   uLongf *destLen,
                                  const Bytef *source, uLong sourceLen,
                                  int level = Z_BEST_COMPRESSION)

{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
//    MCE_DEBUG("----ZIP FLAG1:"<<zlibCompileFlags());
#ifdef MAXSEG_64K
//    MCE_DEBUG("----ZIP FLAG2:"<<zlibCompileFlags());
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
//    MCE_DEBUG("----ZIP FLAG3:"<<zlibCompileFlags());
    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
//    MCE_DEBUG("----ZIP FLAG4:"<<zlibCompileFlags()<< " avail_out:"<< stream.avail_out << " desc len int:"<<(uInt)*destLen << " long:"<< (uLong)*destLen<< "  (uLong)stream.avail_out = "<< (uLong)stream.avail_out<< " *destLen"<<*destLen );
    if ((uLong)stream.avail_out != (uInt)*destLen) return Z_BUF_ERROR;
//    MCE_DEBUG("----ZIP FLAG5:"<<zlibCompileFlags());

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;
    stream.adler = crc32(0L, Z_NULL, 0);
    
    //-MAX_WBITS
//    MCE_DEBUG("----ZIP FLAG:"<<zlibCompileFlags());
    err = deflateInit2(&stream, level, Z_DEFLATED,MAX_WBITS+16,8,Z_DEFAULT_STRATEGY);
    if (err != Z_OK) return err;
    
//    MCE_DEBUG("----ZIP FLAG:"<<zlibCompileFlags());
    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}
//


//int gzcompress(Bytef *data, uLong ndata, 
//	Bytef *zdata, uLong *nzdata)
//{
//	z_stream c_stream;
//	int err = 0;
//
//	if(data && ndata > 0)
//	{
//		c_stream.zalloc = (alloc_func)0;
//		c_stream.zfree = (free_func)0;
//		c_stream.opaque = (voidpf)0;
//		if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 
//                    -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
//		c_stream.next_in  = data;
//		c_stream.avail_in  = ndata;
//		c_stream.next_out = zdata;
//		c_stream.avail_out  = *nzdata;
//		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) 
//		{
//			if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
//		}
//        if(c_stream.avail_in != 0) return c_stream.avail_in;
//		for (;;) {
//			if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
//			if(err != Z_OK) return -1;
//		}
//		if(deflateEnd(&c_stream) != Z_OK) return -1;
//		*nzdata = c_stream.total_out;
//		return 0;
//	}
//	return -1;
//}


std::string com::xiaonei::talk::util::gzip_compress(const std::string& source) {

	if (source.empty()) {
		return "";
	}

	int len = source.length()*2;
	char* tmp = new char[len];


	int err = compress__((Bytef*)tmp, (uLongf*)&len,
			(Bytef*) source.c_str(), (uLongf)source.length());
//	
//	MCE_DEBUG("out:"<< len);
//	MCE_DEBUG("GZIP   err1:"<< err << "  ok:"<<Z_OK);
	if (err == Z_OK) {
		
		std::string res((char*)tmp, len);
//		MCE_DEBUG("data1:"<< "  len:"<<len);
		delete [] tmp;

//		MCE_DEBUG("data1:"<< "  len:"<<len);
		return res;
	}
	MCE_WARN("zlib err:"<<err);
	delete [] tmp;
	return "";
}





//std::string com::xiaonei::talk::util::gzip_compress(const std::string& data) {
//	std::string compressed;
//	boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
//	out.set_auto_close(true);
//	
//	out.push(boost::iostreams::gzip_compressor());
//	out.push(boost::iostreams::back_inserter(compressed));
//	boost::iostreams::copy(boost::make_iterator_range(data), out);
//	out.pop();
//	out.reset();
//	return compressed;
//
//}
