/*
 * =====================================================================================
 *
 *       Filename:  TypeParser.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月22日 16时06分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __TYPE_PARSER_HPP__
#define __TYPE_PARSER_HPP__

#include <boost/lexical_cast.hpp>

namespace xce {
namespace feed {
	
struct WeiredType {
  unsigned type       : 16;
  unsigned version    : 8;
  unsigned reversed0  : 2;
  unsigned real_type  : 2;  
  unsigned mini_merge : 2;
  unsigned news_merge : 2;

  WeiredType(unsigned int v) {
    BOOST_STATIC_ASSERT(sizeof(WeiredType) == sizeof(int));
     type = v&0xffff;
     v >>= 16;
     version = v & 0xff;
     v >>= 8;
     reversed0 = v & 0x3;
     v >>= 2;
     real_type = v & 0x3;
     v >>= 2;
     mini_merge = v & 0x3;
     v >>= 2;
     news_merge = v & 0x3;
  }
};

}
}

#endif
