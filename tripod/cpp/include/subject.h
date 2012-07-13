/*
 * subject.h
 *
 *  Created on: 2011-3-15
 *      Author: guanghe.ge
 */

#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <vector>
#include <string>
#include "observer.h"
#include <boost/thread/shared_mutex.hpp>

namespace com {
namespace renren {
namespace tripod {

class Subject {
public:
	Subject();
	~Subject();

	void RegisterObserver(ObserverPtr o);
	void RemoveObserver(ObserverPtr o);

	void NotifyObservers(const std::string& path);
private:
	std::vector<ObserverPtr> observers_;
	boost::shared_mutex mutex_;
};
}
}
}
#endif /* SUBJECT_H_ */
