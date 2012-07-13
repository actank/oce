/*
 * subject.cc
 *
 *  Created on: 2011-3-16
 *      Author: guanghe.ge
 */
#include "subject.h"
#include "observer.h"
#include "logger.h"

using namespace com::renren::tripod;

Subject::Subject() {
}
Subject::~Subject() {

}
void Subject::RegisterObserver(ObserverPtr o) {
	try {
		MCE_DEBUG("Subject::RegisterObserver");
		boost::unique_lock<boost::shared_mutex> lock(mutex_);
		observers_.push_back(o);
	} catch (std::exception& e) {
		MCE_WARN("Subject::RegisterObserver " << e.what());
	} catch (boost::exception& e) {
		MCE_WARN("Subject::RegisterObserver exception");
	}
}

void Subject::RemoveObserver(ObserverPtr o) {
	try {
		MCE_DEBUG("Subject::RemoveObserver");
		boost::unique_lock<boost::shared_mutex> lock(mutex_);
		for (std::vector<ObserverPtr>::iterator it = observers_.begin(); it
				!= observers_.end(); ++it) {
			if ((*it) == o) {
				observers_.erase(it);
			}
		}
	} catch (std::exception& e) {
		MCE_WARN("Subject::RemoveObserver " << e.what());
	} catch (boost::exception& e) {
		MCE_WARN("Subject::RemoveObserver exception");
	}
}

void Subject::NotifyObservers(const std::string& path) {
	try {
		boost::shared_lock<boost::shared_mutex> lock(mutex_);
		for (std::vector<ObserverPtr>::const_iterator it = observers_.begin(); it
				!= observers_.end(); ++it) {
			(*it)->Update(path);
		}
	} catch (std::exception& e) {
		MCE_WARN("Subject::NotifyObserver " << e.what());
	} catch (boost::exception& e) {
		MCE_WARN("Subject::NotifyObserver exception");
	}
}
