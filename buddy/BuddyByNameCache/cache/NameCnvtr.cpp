#include "NameCnvtr.h"
#include <boost/lexical_cast.hpp>
#include <errno.h>

NameCnvtr::NameCnvtr( const std::string& source_code, const std::string& trans_code, const std::string& target_code ) :
	_source_code( source_code ),
	_trans_code( trans_code ),
	_target_code( target_code ){
		if( (_source_code==_target_code)
				|| (_source_code==_trans_code)
				|| (_trans_code==_target_code) ){
			_legal = false;
		} else{
			_legal = true;
		}
}

NameCnvtr::NameCnvtr( const std::string& source_code, const std::string& target_code ) :
	_source_code( source_code ),
	_trans_code( "" ),
	_target_code( target_code ){
		if( (_source_code==_target_code)
				|| (_source_code==_trans_code)
				|| (_trans_code==_target_code) ){
			_legal = false;
		} else{
			_legal = true;
		}
}

NameCnvtr::~NameCnvtr(){
}

std::string NameCnvtr::cnvt_direct( const std::string& strIn ){
	iconv_t _conv_s_t;
	if( _legal ){
		_conv_s_t = iconv_open( _target_code.c_str(), _source_code.c_str() );
	}else{
		return strIn;
	}
	iconv( _conv_s_t, NULL, NULL, NULL, NULL );
	std::string res;
	size_t lenIn = strIn.length();
	char *itIn = const_cast<char*>( strIn.c_str() );
	while( (lenIn>0) && (itIn<strIn.c_str()+strIn.length()) ){
		ErrNo errNo = _INIT;
		std::string strOut = docnvt( _conv_s_t, strIn, &itIn, lenIn, errNo );
		res += strOut;
		if( errNo==_EILSEQ || errNo==_EINVAL || errNo==_UNKNOWN ){
			res += itIn[0];
			++itIn;
			--lenIn;
		}
	}
	iconv_close( _conv_s_t );
	return res;
}

std::string NameCnvtr::cnvt_indirect( const std::string& strIn ){ 
	iconv_t _conv_s_t;
	iconv_t _conv_s_m;
	iconv_t _conv_m_t;
	if( _legal ){
		_conv_s_t = iconv_open( _target_code.c_str(), _source_code.c_str() );
		_conv_s_m = iconv_open( _trans_code.c_str(), _source_code.c_str() );
		_conv_m_t = iconv_open( _target_code.c_str(), _trans_code.c_str() );
	}else{
		return strIn;
	}
	iconv( _conv_s_t, NULL, NULL, NULL, NULL );
	iconv( _conv_s_m, NULL, NULL, NULL, NULL );
	iconv( _conv_m_t, NULL, NULL, NULL, NULL );
	std::string res;
	size_t lenIn = strIn.length();
	char *itIn = const_cast<char*>( strIn.c_str() );
	while( (lenIn>0) && (itIn<strIn.c_str()+strIn.length()) ){
		ErrNo errNo = _INIT;
		std::string strOut = docnvt( _conv_s_t, strIn, &itIn, lenIn, errNo );
		res += strOut;
		if( errNo!=_EILSEQ && errNo!=_EINVAL && errNo!=_UNKNOWN ){
			continue;
		}

		errNo = _INIT;
		std::string strTmp = docnvt( _conv_s_m, strIn, &itIn, lenIn, errNo );
		if( strTmp.size() != 0 ){
			char *itTmp = const_cast<char*>( strTmp.c_str() );
			errNo = _INIT;
			size_t lenTmp = strTmp.length();
			strOut = docnvt( _conv_m_t, strTmp, &itTmp, lenTmp, errNo);
			res += strOut;
		} else if( errNo==_EILSEQ || errNo==_EINVAL || errNo==_UNKNOWN ){
			res += itIn[0];
			++itIn;
			--lenIn;
		}
	}
	iconv_close( _conv_s_t );
	iconv_close( _conv_s_m );
	iconv_close( _conv_m_t );
	return res;
}

std::string NameCnvtr::docnvt( const iconv_t& conv, const std::string& str, char** itIn, size_t& lenIn, ErrNo& errNo ){
	char buffer[ str.length()*2+10 ];
	size_t lenOut = sizeof(buffer) - 5;
	char *itOut = buffer;
	std::string res;
	int convres = iconv( conv, itIn, &lenIn, &itOut, &lenOut );
	if( itOut != buffer ){
		itOut[0] = '\0';
		res += buffer;
	}
	if( convres == 0){
		errNo = _NOERR;
	} else if( errno == E2BIG){
		lenOut = sizeof(buffer) - 1;
		lenIn -= strlen(buffer);
		errNo = _E2BIG;
	} else if( errno == EILSEQ){
		errNo = _EILSEQ;
	} else if( errno == EINVAL){
		errNo = _EINVAL;
	} else {
		errNo = _UNKNOWN;
	}
	return res;
}

