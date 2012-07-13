/*
 * data.h
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#ifndef DATA_H_
#define DATA_H_

#include <vector>
#include <boost/shared_ptr.hpp>

namespace com {
namespace renren {
namespace tripod {

typedef std::vector<char> Data;
typedef boost::shared_ptr<Data> DataPtr;

}
}
}

#endif /* DATA_H_ */
