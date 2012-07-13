#ifndef __NAME_CNVTR_H__
#define __NAME_CNVTR_H__

#include <iconv.h>
#include <string>
#include <Ice/Ice.h>

class NameCnvtr : public IceUtil::Shared
{
	public:
		NameCnvtr( const std::string& source_code, const std::string& trans_code, const std::string& target_code );
		NameCnvtr( const std::string& source_code, const std::string& target_code );
		~NameCnvtr();

		std::string cnvt_direct( const std::string& strIn );
		std::string cnvt_indirect( const std::string& strIn );
	private:
		std::string _source_code;
		std::string _trans_code;
		std::string _target_code;
		bool _legal;
		enum ErrNo{ _INIT, _NOERR, _E2BIG, _EILSEQ, _EINVAL, _UNKNOWN };
		std::string docnvt( const iconv_t& conv, const std::string& str, char** itIn, size_t& lenIn, ErrNo& errNo );
};
typedef IceUtil::Handle<NameCnvtr> NameCnvtrPtr;
#endif
