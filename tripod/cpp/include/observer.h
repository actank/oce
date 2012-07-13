/*
 * observer.h
 *
 *  Created on: 2011-3-15
 *      Author: guanghe.ge
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <boost/shared_ptr.hpp>

namespace com {
namespace renren {
namespace tripod {

class Observer {
public:
	virtual void Update(const std::string& path) = 0;
};
typedef boost::shared_ptr<Observer> ObserverPtr;
}
}
}
#endif /* OBSERVER_H_ */
